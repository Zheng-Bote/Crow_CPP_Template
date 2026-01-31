/**
 * SPDX-FileComment: System Controller Header
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file system_controller.hpp
 * @brief Controller for system-related routes (health, info).
 * @version 0.1.0
 * @date 2026-01-31
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#pragma once

#include <crow.h>

namespace rz::controllers {

/**
 * @brief Controller handling system diagnostics and information.
 */
class SystemController {
public:
    /**
     * @brief Registers routes associated with this controller to the Crow app.
     * @param app Reference to the Crow application.
     */
    static void registerRoutes(crow::SimpleApp& app);
};

} // namespace rz::controllers
