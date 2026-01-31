/**
 * @file password_utils.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Password Hashing Utilities
 * @version 0.16.0
 * @date 2026-01-31
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

// (OWASP Recommendations).
// Time Cost(t) : 3 Iterations
// Memory Cost(m) : 64 MB(65536 KB)
// Parallelism(p) : 4 Threads

#include "utils/password_utils.hpp"
#include "argon2.h"
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>

// Internal constants for Argon2id security
const uint32_t T_COST = 3;      // 3 iterations
const uint32_t M_COST = 65536;  // 64 MiB RAM usage
const uint32_t PARALLELISM = 4; // 4 Lanes
const uint32_t SALT_LEN = 16;   // 16 Bytes Salt
const uint32_t HASH_LEN = 32;   // 32 Bytes Output Hash

// Namespace rz::utils
namespace rz {
namespace utils {

/**
 * @brief Hashes a plain text password using Argon2id.
 *
 * Uses settings recommended by OWASP:
 * - Time Cost: 3 iterations
 * - Memory Cost: 64 MiB
 * - Parallelism: 4 threads
 *
 * @param plainText The password to hash.
 * @return The encoded hash string (including salt and parameters) or empty string on failure.
 */
std::string PasswordUtils::hashPassword(const std::string &plainText) {
  uint8_t salt[SALT_LEN];
  std::random_device rd;
  std::uniform_int_distribution<uint8_t> dist(0, 255);
  for (uint32_t i = 0; i < SALT_LEN; ++i) {
    salt[i] = dist(rd);
  }

  size_t encodedLen = argon2_encodedlen(T_COST, M_COST, PARALLELISM, SALT_LEN,
                                        HASH_LEN, Argon2_id);
  std::vector<char> encoded(encodedLen);

  int result = argon2id_hash_encoded(
      T_COST, M_COST, PARALLELISM, plainText.c_str(), plainText.length(), salt,
      SALT_LEN, HASH_LEN, encoded.data(), encodedLen);

  if (result != ARGON2_OK) {
    std::cerr << "Argon2 use failed, Error Code:" << result << std::endl;
    return std::string();
  }

  return std::string(encoded.data());
}

/**
 * @brief Verifies a password against an encoded hash.
 *
 * @param plainText The plain text password to verify.
 * @param encodedHash The Argon2id encoded hash to verify against.
 * @return True if the password matches, false otherwise.
 */
bool PasswordUtils::verifyPassword(const std::string &plainText,
                                   const std::string &encodedHash) {
  if (encodedHash.empty())
    return false;

  int result =
      argon2id_verify(encodedHash.c_str(), plainText.c_str(), plainText.length());

  return (result == ARGON2_OK);
}

/**
 * @brief Generates a random password.
 *
 * @param length Length of the password.
 * @return Random string.
 */
std::string PasswordUtils::generateRandomPassword(int length) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    // sizeof(charset) includes the null terminator, so subtract 1.
    // However, for valid indices 0..(size-2), we use max_index - 1 in distribution.
    const size_t charset_len = sizeof(charset) - 1;

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<size_t> distribution(0, charset_len - 1);

    std::string password;
    password.reserve(length);
    for (int i = 0; i < length; ++i) {
        password.push_back(charset[distribution(generator)]);
    }
    return password;
}

} // namespace utils
} // namespace rz