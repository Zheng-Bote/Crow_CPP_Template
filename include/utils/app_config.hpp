/**
 * SPDX-FileComment: Application Configuration Loader
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file app_config.hpp
 * @brief Singleton class to manage application configuration loaded from .env file.
 * @version 0.1.0
 * @date 2026-01-31
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#pragma once

#include <string>
#include <string_view>
#include <cstdint>
#include <expected>

namespace rz::utils {

/**
 * @brief Singleton class to access configuration variables.
 */
class AppConfig {
public:
    /**
     * @brief Get the singleton instance.
     * @return AppConfig& Reference to the singleton instance.
     */
    static AppConfig& getInstance();

    /**
     * @brief Load configuration from the specified .env file.
     * @param env_path Path to the .env file.
     * @return std::expected<void, std::string> Success or error message.
     */
    std::expected<void, std::string> load(const std::string& env_path);

    // -- Accessors --

    /**
     * @brief Get the server port.
     * @return uint16_t Port number.
     */
    [[nodiscard]] uint16_t getServerPort() const;

    /**
     * @brief Get the number of server threads.
     * @return uint16_t Number of threads.
     */
    [[nodiscard]] uint16_t getServerThreads() const;

    /**
     * @brief Get a string configuration value by key.
     * @param key Environment variable key.
     * @param default_value Value to return if key is not found.
     * @return std::string Value or default.
     */
    [[nodiscard]] std::string getString(const std::string& key, std::string_view default_value = "") const;

    /**
     * @brief Get an integer configuration value by key.
     * @param key Environment variable key.
     * @param default_value Value to return if key is not found.
     * @return int Value or default.
     */
    [[nodiscard]] int getInt(const std::string& key, int default_value = 0) const;

private:
    AppConfig() = default;
    ~AppConfig() = default;
    AppConfig(const AppConfig&) = delete;
    AppConfig& operator=(const AppConfig&) = delete;

    bool m_loaded = false;
};

} // namespace rz::utils
