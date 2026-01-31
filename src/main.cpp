/**
 * SPDX-FileComment: Main Entry Point
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file main.cpp
 * @brief Main entry point for the C++23 Crow Webserver.
 * @version 0.1.2
 * @date 2026-01-31
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#include <crow.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <print>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <csignal>
#include <functional>

#include "rz_config.hpp"
#include "utils/app_config.hpp"
#include "controllers/home_controller.hpp"
#include "controllers/system_controller.hpp"
#include "services/database_service.hpp" // Added include

namespace fs = std::filesystem;

// -- Global Signal Handler Wrapper --
std::function<void(int)> g_shutdown_handler;

void signal_handler(int signum) {
    if (g_shutdown_handler) {
        g_shutdown_handler(signum);
    }
}

// -- Helper Functions --

std::string get_current_time_str() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

int main() {
    // 1. Load Configuration First
    auto& config = rz::utils::AppConfig::getInstance();
    const std::string env_file = "data/CPPAppServer.env";
    auto config_result = config.load(env_file);

    // 2. Logging Setup
    std::string logDir = config.getString("LOG_DIR", "./data/logs");
    std::string logLevelStr = config.getString("LOG_LEVEL", "info");
    std::string projName(rz::config::EXECUTABLE_NAME); 

    // Ensure log directory exists
    try {
        if (!fs::exists(logDir)) {
            fs::create_directories(logDir);
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to create log directory: " << e.what() << std::endl;
        return 1;
    }

    std::string logFile = logDir + "/" + projName + ".log";

    try {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFile, 1024 * 1024 * 5, 3);
        
        std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
        auto logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());
        
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
        spdlog::set_default_logger(logger);

        if (logLevelStr == "debug") spdlog::set_level(spdlog::level::debug);
        else if (logLevelStr == "warn") spdlog::set_level(spdlog::level::warn);
        else if (logLevelStr == "error") spdlog::set_level(spdlog::level::err);
        else spdlog::set_level(spdlog::level::info);

        // Flush on Error AND periodically every 3 seconds
        spdlog::flush_on(spdlog::level::err);
        spdlog::flush_every(std::chrono::seconds(3));

        spdlog::info("Logging initialized. Level: {}, File: {}", logLevelStr, logFile);

    } catch (const spdlog::spdlog_ex &ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        return 1;
    }

    // --- STARTUP LOGS ---
    spdlog::info("Starting {} v{}", std::string(rz::config::PROG_LONGNAME), std::string(rz::config::VERSION));
    spdlog::info("Server Start Time: {}", get_current_time_str());
    
    if (!config_result) {
         spdlog::error("Configuration Error: {}", config_result.error());
    } else {
         spdlog::info("Using Configuration File: {}", env_file);
    }

    // 3. Initialize Database
    if (auto res = rz::services::DatabaseService::getInstance().init(); !res) {
        spdlog::error("Database Initialization Failed: {}", res.error());
        return 1;
    }

    // 4. Setup Crow Application
    crow::SimpleApp app;

    // 5. Register Controllers / Routes
    rz::controllers::HomeController::registerRoutes(app);
    rz::controllers::SystemController::registerRoutes(app);

    // 5. Configure App Settings
    uint16_t port = config.getServerPort();
    uint16_t threads = config.getServerThreads();
    
    if (logLevelStr == "debug") {
        app.loglevel(crow::LogLevel::Debug);
    } else {
        app.loglevel(crow::LogLevel::Info);
    }

    // 6. Signal Handling Setup
    g_shutdown_handler = [&](int signum) {
        spdlog::info("Interrupt signal ({}) received. Stopping server...", signum);
        app.stop();
    };
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    spdlog::info("Server listening on port {}", port);

    // 7. Run Server
    auto& app_runner = app.port(port).multithreaded();
    if (threads > 0) {
        app_runner.concurrency(threads);
    }
    app_runner.run();

    // 8. Shutdown Logs
    int exitCode = 0; // Assuming clean exit if run() returns
    spdlog::info("Server End Time: {}", get_current_time_str());
    spdlog::info("Server shutting down with code: {}", exitCode);

    // IMPORTANT: Flush logs immediately now
    spdlog::shutdown();

    return exitCode;
}
