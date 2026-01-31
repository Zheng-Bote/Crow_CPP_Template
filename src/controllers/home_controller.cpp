/**
 * SPDX-FileComment: Home Controller Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file home_controller.cpp
 * @brief Implementation of HomeController routes.
 * @version 0.1.0
 * @date 2026-01-31
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#include "controllers/home_controller.hpp"
#include <nlohmann/json.hpp>
#include "rz_config.hpp" // For version info

namespace rz::controllers {

void HomeController::registerRoutes(crow::SimpleApp& app) {
    
    // Root endpoint
    CROW_ROUTE(app, "/")
    ([]() {
        nlohmann::json response;
        response["app"] = rz::config::PROG_LONGNAME;
        response["version"] = rz::config::VERSION;
        response["status"] = "running";
        response["message"] = "Welcome to the CPP App Server";
        
        return crow::response(response.dump());
    });

    // Status endpoint
    CROW_ROUTE(app, "/status")
    ([]() {
        return crow::response(200, "OK");
    });
}

} // namespace rz::controllers
