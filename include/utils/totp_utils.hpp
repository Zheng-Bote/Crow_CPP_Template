/**
 * @file totp_utils.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief TOTP Utilities (Google Authenticator compatible)
 * @version 0.16.0
 * @date 2026-01-31
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <string>
#include <cstdint>
#include <vector>

namespace rz {
namespace utils {

class TotpUtils {
public:
  // Generates a random 16-byte secret (Base32 encoded)
  static std::string generateSecret();

  // Creates the URL for the QR code (otpauth://totp/...)
  // Default parameter 'issuer' must be in the header
  static std::string getProvisioningUri(const std::string &userEmail,
                                    const std::string &secret,
                                    const std::string &issuer = "CakePlanner");

  // Checks if the code is valid for the given secret (tolerates +- 1 time step)
  static bool validateCode(const std::string &secret, const std::string &code);

private:
  // IMPORTANT: These private helpers must remain declared here
  static std::vector<uint8_t> base32Decode(const std::string &secret);
  static int64_t getCurrentTimeStep();
  static std::string generateCodeForStep(const std::vector<uint8_t> &keyBytes,
                                     int64_t timeStep);
};

} // namespace utils
} // namespace rz