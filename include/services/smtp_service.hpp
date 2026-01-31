/**
 * SPDX-FileComment: SMTP Service Header
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file smtp_service.hpp
 * @brief Service for sending emails using mailio and inja templates.
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
#include <expected>
#include <nlohmann/json.hpp>

namespace rz::services {

/**
 * @brief Service class to handle email sending with template rendering.
 */
class SmtpService {
public:
    /**
     * @brief Sends an email with the specified template data.
     * 
     * @param to_email Recipient email address.
     * @param lang Language code (e.g., "en", "de"). Defaults to "en".
     * @param data JSON data for INJA template rendering. 
     *             Should contain 'subject' key if the template expects it, 
     *             or we can pass it separately. 
     *             Convention: JSON should contain "subject" key for the email subject,
     *             and other keys for body rendering.
     * @return std::expected<void, std::string> Success or error message.
     */
    static std::expected<void, std::string> sendEmail(
        const std::string& to_email, 
        const std::string& lang, 
        const nlohmann::json& data
    );
};

} // namespace rz::services
