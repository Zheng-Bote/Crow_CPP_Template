/**
 * SPDX-FileComment: SMTP Service Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file smtp_service.cpp
 * @brief Implementation of SmtpService.
 * @version 0.1.0
 * @date 2026-01-31
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#include "services/smtp_service.hpp"
#include "utils/app_config.hpp"
#include <mailio/message.hpp>
#include <mailio/smtp.hpp>
#include <mailio/dialog.hpp> // Required for ssl_options_t
#include <boost/asio/ssl.hpp> // Required for verify_none
#include <inja/inja.hpp>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace rz::services {

std::expected<void, std::string> SmtpService::sendEmail(
    const std::string& to_email, 
    const std::string& lang, 
    const nlohmann::json& data
) {
    auto& config = rz::utils::AppConfig::getInstance();

    // 1. Get SMTP Config
    std::string smtp_server = config.getString("SMTP_SERVER", "localhost");
    int smtp_port = config.getInt("SMTP_PORT", 587);
    std::string smtp_user = config.getString("SMTP_USERNAME", "");
    std::string smtp_pass = config.getString("SMTP_PASSWORD", "");
    std::string smtp_from = config.getString("SMTP_FROM", "");
    std::string starttls_str = config.getString("SMTP_STARTTLS", "true");
    bool use_starttls = (starttls_str == "true" || starttls_str == "1");

    // 2. Determine Template Path
    std::string template_dir_path = config.getString("MAIL_TEMPLATE_DIR", "./data/templates");
    std::string target_lang = lang.empty() ? "en" : lang;
    
    std::string template_filename = "email_template_" + target_lang + ".html";
    std::filesystem::path template_path = std::filesystem::path(template_dir_path) / template_filename;

    if (!std::filesystem::exists(template_path)) {
        if (target_lang != "en") {
            template_path = std::filesystem::path(template_dir_path) / "email_template_en.html";
        }
        
        if (!std::filesystem::exists(template_path)) {
            std::string err = "Template not found: " + template_path.string();
            spdlog::error(err);
            return std::unexpected(err);
        }
    }

    // 3. Render Template
    nlohmann::json render_data = data;
    if (!render_data.contains("has_link")) render_data["has_link"] = false;
    if (!render_data.contains("title")) render_data["title"] = "Notification";

    std::string rendered_body;
    try {
        inja::Environment env;
        rendered_body = env.render_file(template_path.string(), render_data);
    } catch (const std::exception& e) {
        std::string err = "Template rendering failed: " + std::string(e.what());
        spdlog::error(err);
        return std::unexpected(err);
    }

    // 4. Construct Message
    try {
        mailio::message msg;
        msg.from(mailio::mail_address("App Server", smtp_from));
        msg.add_recipient(mailio::mail_address("", to_email));
        
        std::string subject = "Notification";
        if (data.contains("subject") && data["subject"].is_string()) {
            subject = data["subject"].get<std::string>();
        }
        msg.subject(subject);
        
        msg.content_transfer_encoding(mailio::mime::content_transfer_encoding_t::QUOTED_PRINTABLE);
        msg.content_type(mailio::message::media_type_t::TEXT, "html", "utf-8");
        msg.content(rendered_body);

        // 5. Send via SMTP
        if (use_starttls) {
            mailio::smtps conn(smtp_server, smtp_port);
            
            // Fix: Disable strict SSL verification to avoid handshake errors in dev/some envs
            mailio::dialog_ssl::ssl_options_t ssl_opt;
            ssl_opt.method = boost::asio::ssl::context::tls;
            ssl_opt.verify_mode = boost::asio::ssl::verify_none;
            conn.ssl_options(ssl_opt);

            conn.authenticate(smtp_user, smtp_pass, mailio::smtps::auth_method_t::START_TLS);
            conn.submit(msg);
        } else {
            mailio::smtp conn(smtp_server, smtp_port);
            conn.authenticate(smtp_user, smtp_pass, mailio::smtp::auth_method_t::LOGIN);
            conn.submit(msg);
        }
        
        spdlog::info("Email sent to {} (Lang: {})", to_email, target_lang);

    } catch (const std::exception& e) {
        std::string err = "SMTP Error: " + std::string(e.what());
        spdlog::error(err);
        return std::unexpected(err);
    }

    return {};
}

} // namespace rz::services
