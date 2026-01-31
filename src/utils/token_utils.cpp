/**
 * @file token_utils.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief JWT Token Utilities Implementation
 * @version 0.16.0
 * @date 2026-01-31
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "utils/token_utils.hpp"
#include "utils/app_config.hpp" // Using AppConfig instead of EnvLoader
#include <chrono>
#include <iostream>

// Access JSON Traits for Bool conversion
using json_value = jwt::traits::kazuho_picojson::value_type;

namespace rz {
namespace utils {

// Helper to get secret from AppConfig
static std::string getSecret() {
  auto& config = rz::utils::AppConfig::getInstance();
  // Using AppConfig which handles env loading
  std::string secret = config.getString("SERVER_JWT_SECRET", "");
  
  if (secret.empty()) {
    std::cerr
        << "WARNING: SERVER_JWT_SECRET not set! Using unsafe default."
        << std::endl;
    return "CHANGE_ME_IN_PRODUCTION_THIS_IS_UNSAFE";
  }
  return secret;
}

/**
 * @brief Generates a JWT (JSON Web Token) for a user.
 *
 * The token contains the user ID, email, and admin status as claims.
 * It is signed with the secret key and expires after 24 hours.
 *
 * @param userId The ID of the user.
 * @param email The email address of the user.
 * @param isAdmin True if the user is an administrator.
 * @return The generated JWT as a string.
 */
std::string TokenUtils::generateToken(const std::string &userId, const std::string &email,
                                  bool isAdmin) {
  auto now = std::chrono::system_clock::now();

  auto token = jwt::create()
                   .set_issuer("CakePlanner")
                   .set_type("JWS")
                   .set_issued_at(now)
                   .set_expires_at(now + std::chrono::hours(24))
                   .set_payload_claim("uid", jwt::claim(userId))
                   .set_payload_claim("sub", jwt::claim(email))
                   .set_payload_claim("adm", jwt::claim(json_value(isAdmin)))
                   .sign(jwt::algorithm::hs256{getSecret()});

  return token;
}

/**
 * @brief Verifies and decodes a JWT.
 *
 * Checks the signature and expiration time.
 *
 * @param rawToken The raw JWT string.
 * @return An optional TokenPayload containing the user info if verification succeeds, std::nullopt otherwise.
 */
std::optional<TokenPayload>
TokenUtils::verifyToken(const std::string &rawToken) {
  try {
    auto verifier = jwt::verify()
                        .allow_algorithm(jwt::algorithm::hs256{getSecret()})
                        .with_issuer("CakePlanner");

    auto decoded = jwt::decode(rawToken);
    verifier.verify(decoded);

    TokenPayload payload;
    payload.userId = decoded.get_payload_claim("uid").as_string();
    payload.email = decoded.get_payload_claim("sub").as_string();

    if (decoded.has_payload_claim("adm")) {
      auto claim = decoded.get_payload_claim("adm");
      if (claim.get_type() == jwt::json::type::boolean)
        payload.isAdmin = claim.as_boolean();
      else
        payload.isAdmin = false;
    } else {
      payload.isAdmin = false;
    }

    return payload;

  } catch (const std::exception &e) {
    // std::cerr << "Token Verify Failed: " << e.what() << std::endl;
    return std::nullopt;
  }
}

} // namespace utils
} // namespace rz