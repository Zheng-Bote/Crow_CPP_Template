/**
 * SPDX-FileComment: Database Service Header
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file database_service.hpp
 * @brief Singleton service for SQLite database management.
 * @version 0.1.0
 * @date 2026-01-31
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include <functional>
#include <expected>
#include <mutex>

namespace rz::services {

struct User {
    std::string uuid;
    std::string name;
    std::string email;
};

struct NotificationConfig {
    std::string user_uuid;
    bool email_enabled;
    bool html_email;
    bool push_enabled;
    std::string language;
};

/**
 * @brief Service to handle SQLite database operations.
 */
class DatabaseService {
public:
    static DatabaseService& getInstance();

    /**
     * @brief Initializes the database connection and creates tables if missing.
     * @return std::expected<void, std::string>
     */
    std::expected<void, std::string> init();

    /**
     * @brief Get a user by UUID.
     */
    std::expected<User, std::string> getUser(const std::string& uuid);

    /**
     * @brief Get notification configuration for a user.
     */
    std::expected<NotificationConfig, std::string> getNotificationConfig(const std::string& user_uuid);

    /**
     * @brief Create a dummy user and config for testing purposes (Upsert).
     */
    std::expected<void, std::string> createOrUpdateUser(const User& user, const NotificationConfig& config);

private:
    DatabaseService() = default;
    ~DatabaseService();
    DatabaseService(const DatabaseService&) = delete;
    DatabaseService& operator=(const DatabaseService&) = delete;

    sqlite3* m_db = nullptr;
    std::mutex m_mutex;
    bool m_initialized = false;

    std::expected<void, std::string> executeQuery(const std::string& query);
};

} // namespace rz::services
