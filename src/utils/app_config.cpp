/**
 * SPDX-FileComment: Application Configuration Loader Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file app_config.cpp
 * @brief Implementation of AppConfig class.
 * @version 0.1.0
 * @date 2026-01-31
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#include "utils/app_config.hpp"

#include <dotenv.h>
#include <cstdlib>
#include <print>
#include <filesystem>

namespace rz::utils {

AppConfig& AppConfig::getInstance() {
    static AppConfig instance;
    return instance;
}

std::expected<void, std::string> AppConfig::load(const std::string& env_path) {
    if (m_loaded) {
        return {};
    }

    if (!std::filesystem::exists(env_path)) {
        return std::unexpected("Environment file not found: " + env_path);
    }

    try {
        dotenv::init(env_path.c_str());
        m_loaded = true;
    } catch (const std::exception& e) {
        return std::unexpected(std::string("Failed to load .env file: ") + e.what());
    }

    return {};
}

uint16_t AppConfig::getServerPort() const {
    return static_cast<uint16_t>(getInt("SERVER_PORT", 8080));
}

uint16_t AppConfig::getServerThreads() const {
    return static_cast<uint16_t>(getInt("SERVER_THREADS", 0));
}

std::string AppConfig::getString(const std::string& key, std::string_view default_value) const {
    const char* val = std::getenv(key.c_str());
    return val ? std::string(val) : std::string(default_value);
}

int AppConfig::getInt(const std::string& key, int default_value) const {
    const char* val = std::getenv(key.c_str());
    if (!val) return default_value;
    try {
        return std::stoi(val);
    } catch (...) {
        return default_value;
    }
}

} // namespace rz::utils
