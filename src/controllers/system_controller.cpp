/**
 * SPDX-FileComment: System Controller Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file system_controller.cpp
 * @brief Implementation of SystemController routes.
 * @version 0.1.0
 * @date 2026-01-31
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#include "controllers/system_controller.hpp"
#include "rz_config.hpp"
#include "utils/app_config.hpp"
#include "services/notification_service.hpp"
#include "services/database_service.hpp"
#include <chrono>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <sstream>

namespace rz::controllers {

void SystemController::registerRoutes(crow::SimpleApp &app) {

  // Health Check Endpoint
  CROW_ROUTE(app, "/system/health_check")
  ([]() {
    nlohmann::json response;
    response["status"] = "ok";

    // Simple timestamp for the health check
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    response["timestamp"] = ss.str();

    return crow::response(response.dump());
  });

  // System Info Endpoint
  CROW_ROUTE(app, "/system/system_info")
  ([]() {
    nlohmann::json response;

    // Project Info
    response["project"]["name"] = rz::config::PROJECT_NAME;
    response["project"]["long_name"] = rz::config::PROG_LONGNAME;
    response["project"]["description"] = rz::config::PROJECT_DESCRIPTION;
    response["project"]["license"] = rz::config::PROG_LICENSE;
    response["project"]["executable"] = rz::config::EXECUTABLE_NAME;
    response["project"]["homepage"] = rz::config::PROJECT_HOMEPAGE_URL;

    // Version Info
    response["version"]["full"] = rz::config::VERSION;
    response["version"]["major"] = rz::config::PROJECT_VERSION_MAJOR;
    response["version"]["minor"] = rz::config::PROJECT_VERSION_MINOR;
    response["version"]["patch"] = rz::config::PROJECT_VERSION_PATCH;

    // Author / Organization
    response["author"]["name"] = rz::config::AUTHOR;
    response["author"]["organization"] = rz::config::ORGANIZATION;
    response["author"]["domain"] = rz::config::DOMAIN;
    response["author"]["created_year"] = rz::config::CREATED_YEAR;

    // Build Info
    response["build"]["std"] = rz::config::CMAKE_CXX_STANDARD;
    response["build"]["compiler"] = rz::config::CMAKE_CXX_COMPILER;

    return crow::response(response.dump());
  });

    // Test Email Route
    CROW_ROUTE(app, "/system/test_email")
    ([]() {
        auto& db = rz::services::DatabaseService::getInstance();
        auto& config = rz::utils::AppConfig::getInstance();

        std::string adminName = config.getString("SERVER_ADMIN_NAME", "Admin Test");
        std::string adminEmail = config.getString("SERVER_ADMIN_EMAIL", "admin@example.com");

        // 1. Ensure Test User Exists
        rz::services::User user{
            "test-admin-01", 
            adminName, 
            adminEmail
        };
        rz::services::NotificationConfig notifConfig{
            user.uuid, true, true, false, "en"
        };

        if (auto res = db.createOrUpdateUser(user, notifConfig); !res) {
            return crow::response(500, "Failed to create test user: " + res.error());
        }

    // 2. Prepare System Info Data
    nlohmann::json sysInfo;
    sysInfo["project"] = rz::config::PROG_LONGNAME;
    sysInfo["version"] = rz::config::VERSION;
    sysInfo["compiler"] = rz::config::CMAKE_CXX_COMPILER;

    std::string message = "System Status Report:<br>";
    message += "Project: " + std::string(rz::config::PROG_LONGNAME) + "<br>";
    message += "Version: " + std::string(rz::config::VERSION) + "<br>";
    message +=
        "Compiler: " + std::string(rz::config::CMAKE_CXX_COMPILER) + "<br>";

    // 3. Send Notification
    nlohmann::json payload;
    payload["subject"] = "System Info Test";
    payload["title"] = "System Information";
    payload["message"] = message;
    payload["app_name"] = rz::config::PROG_LONGNAME;
    payload["has_link"] = false;

    auto result =
        rz::services::NotificationService::notifyUser(user.uuid, payload);

    if (!result) {
      return crow::response(500, "Failed to send email: " + result.error());
    }

    return crow::response(200, "Email sent successfully to " + user.email);
  });
}

} // namespace rz::controllers
