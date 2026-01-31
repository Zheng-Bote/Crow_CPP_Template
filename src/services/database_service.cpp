/**
 * SPDX-FileComment: Database Service Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file database_service.cpp
 * @brief Implementation of DatabaseService.
 * @version 0.1.0
 * @date 2026-01-31
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#include "services/database_service.hpp"
#include "utils/app_config.hpp"
#include <filesystem>
#include <print>
#include <spdlog/spdlog.h>

namespace rz::services {

DatabaseService &DatabaseService::getInstance() {
  static DatabaseService instance;
  return instance;
}

DatabaseService::~DatabaseService() {
  if (m_db) {
    sqlite3_close(m_db);
    m_db = nullptr;
  }
}

std::expected<void, std::string> DatabaseService::init() {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_initialized)
    return {};

  auto &config = rz::utils::AppConfig::getInstance();
  std::string db_path =
      config.getString("DB_DIR", "./data/db/cppappserver.sqlite");

  // Ensure directory exists
  std::filesystem::path path(db_path);
  if (path.has_parent_path() && !std::filesystem::exists(path.parent_path())) {
    std::filesystem::create_directories(path.parent_path());
  }

  int rc = sqlite3_open(db_path.c_str(), &m_db);
  if (rc) {
    std::string err =
        "Can't open database: " + std::string(sqlite3_errmsg(m_db));
    spdlog::error(err);
    return std::unexpected(err);
  }

  // Create Tables
  const char *sql_users = "CREATE TABLE IF NOT EXISTS users ("
                          "uuid TEXT PRIMARY KEY,"
                          "name TEXT NOT NULL,"
                          "email TEXT NOT NULL UNIQUE"
                          ");";

  const char *sql_config = "CREATE TABLE IF NOT EXISTS config_notification ("
                           "user_uuid TEXT PRIMARY KEY,"
                           "email_enabled INTEGER DEFAULT 1,"
                           "html_email INTEGER DEFAULT 1,"
                           "push_enabled INTEGER DEFAULT 0,"
                           "language TEXT DEFAULT 'en',"
                           "FOREIGN KEY(user_uuid) REFERENCES users(uuid)"
                           ");";

  if (auto res = executeQuery(sql_users); !res)
    return res;
  if (auto res = executeQuery(sql_config); !res)
    return res;

  m_initialized = true;
  spdlog::info("Database initialized at {}", db_path);
  return {};
}

std::expected<void, std::string>
DatabaseService::executeQuery(const std::string &query) {
  char *zErrMsg = 0;
  int rc = sqlite3_exec(m_db, query.c_str(), 0, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    std::string err = "SQL error: " + std::string(zErrMsg);
    sqlite3_free(zErrMsg);
    spdlog::error(err);
    return std::unexpected(err);
  }
  return {};
}

std::expected<User, std::string>
DatabaseService::getUser(const std::string &uuid) {
  std::string sql = "SELECT uuid, name, email FROM users WHERE uuid = ?;";
  sqlite3_stmt *stmt;

  if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK) {
    return std::unexpected(sqlite3_errmsg(m_db));
  }

  sqlite3_bind_text(stmt, 1, uuid.c_str(), -1, SQLITE_STATIC);

  User user;
  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW) {
    user.uuid = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    user.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    user.email = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    sqlite3_finalize(stmt);
    return user;
  }

  sqlite3_finalize(stmt);
  return std::unexpected("User not found");
}

std::expected<NotificationConfig, std::string>
DatabaseService::getNotificationConfig(const std::string &user_uuid) {
  std::string sql = "SELECT email_enabled, html_email, push_enabled, language "
                    "FROM config_notification WHERE user_uuid = ?;";
  sqlite3_stmt *stmt;

  if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK) {
    return std::unexpected(sqlite3_errmsg(m_db));
  }

  sqlite3_bind_text(stmt, 1, user_uuid.c_str(), -1, SQLITE_STATIC);

  NotificationConfig conf;
  conf.user_uuid = user_uuid;

  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW) {
    conf.email_enabled = sqlite3_column_int(stmt, 0) != 0;
    conf.html_email = sqlite3_column_int(stmt, 1) != 0;
    conf.push_enabled = sqlite3_column_int(stmt, 2) != 0;
    const char *lang =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    conf.language = lang ? lang : "en";
    sqlite3_finalize(stmt);
    return conf;
  }

  sqlite3_finalize(stmt);
  // Default config if not found
  return NotificationConfig{user_uuid, true, true, false, "en"};
}

std::expected<void, std::string>
DatabaseService::createOrUpdateUser(const User &user,
                                    const NotificationConfig &config) {
  // Transaction
  executeQuery("BEGIN TRANSACTION;");

  std::string sql_user =
      "INSERT OR REPLACE INTO users (uuid, name, email) VALUES (?, ?, ?);";
  sqlite3_stmt *stmt1;
  sqlite3_prepare_v2(m_db, sql_user.c_str(), -1, &stmt1, 0);
  sqlite3_bind_text(stmt1, 1, user.uuid.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt1, 2, user.name.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt1, 3, user.email.c_str(), -1, SQLITE_STATIC);
  if (sqlite3_step(stmt1) != SQLITE_DONE) {
    sqlite3_finalize(stmt1);
    executeQuery("ROLLBACK;");
    return std::unexpected("Failed to upsert user");
  }
  sqlite3_finalize(stmt1);

  std::string sql_conf =
      "INSERT OR REPLACE INTO config_notification (user_uuid, email_enabled, "
      "html_email, push_enabled, language) VALUES (?, ?, ?, ?, ?);";
  sqlite3_stmt *stmt2;
  sqlite3_prepare_v2(m_db, sql_conf.c_str(), -1, &stmt2, 0);
  sqlite3_bind_text(stmt2, 1, user.uuid.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt2, 2, config.email_enabled ? 1 : 0);
  sqlite3_bind_int(stmt2, 3, config.html_email ? 1 : 0);
  sqlite3_bind_int(stmt2, 4, config.push_enabled ? 1 : 0);
  sqlite3_bind_text(stmt2, 5, config.language.c_str(), -1, SQLITE_STATIC);
  if (sqlite3_step(stmt2) != SQLITE_DONE) {
    sqlite3_finalize(stmt2);
    executeQuery("ROLLBACK;");
    return std::unexpected("Failed to upsert config");
  }
  sqlite3_finalize(stmt2);

  executeQuery("COMMIT;");
  return {};
}

} // namespace rz::services
