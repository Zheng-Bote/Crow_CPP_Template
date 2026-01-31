/**
 * @file rz_config.hpp.in
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief configuration
 * @version 0.15.1
 * @date 2026-01-25
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <cstdint>
#include <string_view>

namespace rz {
namespace config {
constexpr std::string_view PROJECT_NAME = "CPPAppServer";
constexpr std::string_view PROG_LONGNAME = "Example CPP Application Server";
constexpr std::string_view PROJECT_DESCRIPTION = "Example CPP Application Server";
constexpr std::string_view PROG_LICENSE = "MIT";

constexpr std::string_view EXECUTABLE_NAME = "CPPAppServer";

constexpr std::string_view VERSION = "0.1.0";
constexpr std::int32_t PROJECT_VERSION_MAJOR { 0 };
constexpr std::int32_t PROJECT_VERSION_MINOR { 1 };
constexpr std::int32_t PROJECT_VERSION_PATCH { 0 };

constexpr std::string_view PROJECT_HOMEPAGE_URL = "https://github.com/Zheng-Bote/web_req_be";
constexpr std::string_view AUTHOR = "ZHENG Robert";
constexpr std::string_view CREATED_YEAR = "2026";
constexpr std::string_view ORGANIZATION = "ZHENG Robert";
constexpr std::string_view DOMAIN = "net.hase-zheng";

constexpr std::string_view CMAKE_CXX_STANDARD = "c++23";
constexpr std::string_view CMAKE_CXX_COMPILER =
    "Clang 20.1.8";
constexpr std::string_view QT_VERSION_BUILD = "";
} // namespace config
} // namespace rz
