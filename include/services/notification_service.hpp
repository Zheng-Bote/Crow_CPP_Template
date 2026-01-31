/**
 * SPDX-FileComment: Notification Service Header
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file notification_service.hpp
 * @brief Service for dispatching notifications based on user preferences.
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
#include <expected>
#include <nlohmann/json.hpp>

namespace rz::services {

/**
 * @brief Service to handle multi-channel notifications.
 */
class NotificationService {
public:
    /**
     * @brief Send a notification to a user based on their UUID and preferences.
     * 
     * @param user_uuid The UUID of the user to notify.
     * @param data JSON data containing the payload (subject, message, variables for templates).
     * @return std::expected<void, std::string> Success or error message.
     */
    static std::expected<void, std::string> notifyUser(const std::string& user_uuid, nlohmann::json data);
};

} // namespace rz::services
