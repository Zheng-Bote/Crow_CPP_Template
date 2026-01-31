/**
 * @file token_utils.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief JWT Token Utilities
 * @version 0.16.0
 * @date 2026-01-31
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <string>
#include <jwt-cpp/jwt.h>
#include <optional>

namespace rz {
namespace utils {

struct TokenPayload {
  std::string userId;
  std::string email;
  bool isAdmin;
};

class TokenUtils {
public:
  // Generates a token, valid for 24h
  static std::string generateToken(const std::string &userId, const std::string &email,
                               bool isAdmin);

  // Verifies the token and returns payload (or nullopt on error)
  static std::optional<TokenPayload> verifyToken(const std::string &rawToken);
};

} // namespace utils
} // namespace rz