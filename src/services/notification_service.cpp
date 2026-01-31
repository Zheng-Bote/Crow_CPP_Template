/**
 * SPDX-FileComment: Notification Service Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file notification_service.cpp
 * @brief Implementation of NotificationService.
 * @version 0.1.0
 * @date 2026-01-31
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#include "services/notification_service.hpp"
#include "services/database_service.hpp"
#include "services/smtp_service.hpp"
#include <spdlog/spdlog.h>

namespace rz::services {

std::expected<void, std::string> NotificationService::notifyUser(const std::string& user_uuid, nlohmann::json data) {
    auto& db = DatabaseService::getInstance();

    // 1. Fetch User
    auto user_res = db.getUser(user_uuid);
    if (!user_res) {
        spdlog::warn("Notification failed: User {} not found.", user_uuid);
        return std::unexpected("User not found");
    }
    User user = user_res.value();

    // 2. Fetch Config
    auto config_res = db.getNotificationConfig(user_uuid);
    if (!config_res) {
        spdlog::warn("Notification failed: Could not fetch config for user {}.", user_uuid);
        return std::unexpected("Config fetch failed");
    }
    NotificationConfig config = config_res.value();

    // 3. Inject User Data into Template Payload if missing
    if (!data.contains("name")) {
        data["name"] = user.name;
    }

    bool notified_any = false;

    // 4. Dispatch Email
    if (config.email_enabled) {
        // Check if user wants HTML or Plain Text (SmtpService currently assumes HTML template, 
        // but we can pass a flag or choose a different template if needed. 
        // For this implementation, we simply send the standard email.)
        
        spdlog::info("Dispatching email to {} ({})", user.name, user.email);
        auto email_res = SmtpService::sendEmail(user.email, config.language, data);
        if (!email_res) {
            spdlog::error("Failed to send email to {}: {}", user.email, email_res.error());
            // We don't return here, we try other channels if available
        } else {
            notified_any = true;
        }
    }

    // 5. Dispatch Push (Placeholder)
    if (config.push_enabled) {
        spdlog::info("Push notification enabled for User {}, but service not implemented yet.", user.name);
        // Implement push logic here...
    }

    if (!notified_any && config.email_enabled) {
        return std::unexpected("Failed to send notification via enabled channels.");
    }

    return {};
}

} // namespace rz::services
