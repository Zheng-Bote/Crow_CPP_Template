/**
 * @file password_utils.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Password Hashing Utilities
 * @version 0.16.0
 * @date 2026-01-31
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <string>

namespace rz {
namespace utils {

class PasswordUtils {
public:
  /**
   * Creates an Argon2id hash.
   * Return format: $argon2id$v=19$m=65536,t=3,p=4$...salt...$hash...
   */
  static std::string hashPassword(const std::string &plainText);

  /**
   * Verifies a password against an Argon2 hash string.
   */
  static bool verifyPassword(const std::string &plainText,
                             const std::string &encodedHash);

  /**
   * Generates a random variable-length safe string.
   */
  static std::string generateRandomPassword(int length = 12);
};

} // namespace utils
} // namespace rz