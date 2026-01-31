cmake_minimum_required(VERSION 3.28)

# cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
# cmake --build build -j$(nproc)

# use GNU Compiler, not Clang (due to SPDLOG)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")


project(CPPAppServer
    VERSION 0.1.0
    DESCRIPTION "Example CPP Application Server"
    HOMEPAGE_URL "https://github.com/Zheng-Bote/web_req_be"
    LANGUAGES C CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(EXECUTABLE_NAME "${PROJECT_NAME}")
set(PROG_LONGNAME "Example CPP Application Server")
set(PROG_CREATED "2026")
set(PROG_AUTHOR "ZHENG Robert")
set(PROG_ORGANIZATION_NAME "ZHENG Robert")
set(PROG_ORGANIZATION_DOMAIN "net.hase-zheng")
set(PROG_LICENSE "MIT")

# Compiler Flags
if(MSVC)
    add_compile_options(/W4)
else()
    # add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion)
endif()

# Include subdirectories
add_subdirectory(configure)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(SQLite3 REQUIRED)

include(FetchContent)
find_program(PATCH_COMMAND patch)

# --- Dependencies ---

# --- NLOHMANN JSON ---
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# --- ASIO ---
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG        asio-1-30-2
)
FetchContent_MakeAvailable(asio)
set(ASIO_INCLUDE_DIR ${asio_SOURCE_DIR}/asio/include CACHE PATH "Path to asio include")

# --- DOTENV ---
FetchContent_Declare(
    dotenv-cpp
    GIT_REPOSITORY https://github.com/laserpants/dotenv-cpp.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(dotenv-cpp)

# --- ARGON2 ---
FetchContent_Declare(
    Argon2
    GIT_REPOSITORY https://github.com/P-H-C/phc-winner-argon2.git
    GIT_TAG        20190702
)
FetchContent_MakeAvailable(Argon2)

set(ARGON2_SRC
    ${argon2_SOURCE_DIR}/src/argon2.c
    ${argon2_SOURCE_DIR}/src/core.c
    ${argon2_SOURCE_DIR}/src/blake2/blake2b.c
    ${argon2_SOURCE_DIR}/src/thread.c
    ${argon2_SOURCE_DIR}/src/encoding.c
    ${argon2_SOURCE_DIR}/src/opt.c
)
add_library(argon2_lib STATIC ${ARGON2_SRC})
target_include_directories(argon2_lib PUBLIC ${argon2_SOURCE_DIR}/include)
target_link_libraries(argon2_lib PRIVATE Threads::Threads)

# --- JWT-CPP ---
FetchContent_Declare(
    jwt-cpp
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(jwt-cpp)

# --- CROW ---
FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG master
)
FetchContent_MakeAvailable(Crow)

# --- INJA ---
FetchContent_Declare(
    inja
    GIT_REPOSITORY https://github.com/pantor/inja.git
    GIT_TAG        v3.4.0
)
set(INJA_USE_EMBEDDED_JSON OFF CACHE BOOL "" FORCE)
set(INJA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_BENCHMARK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(inja)

# --- MAILIO ---
FetchContent_Declare(
    mailio
    GIT_REPOSITORY https://github.com/karastojko/mailio.git
    GIT_TAG        0.25.3
)
FetchContent_MakeAvailable(mailio)

# --- SPDLOG ---
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.12.0
)
FetchContent_MakeAvailable(spdlog)

set(HEADERS
    include/rz_config.hpp
)
set(SOURCES
    src/main.cpp
    src/utils/app_config.cpp
    src/controllers/home_controller.cpp
    src/controllers/system_controller.cpp
    src/services/smtp_service.cpp
    src/services/database_service.cpp
    src/services/notification_service.cpp
)
# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_BINARY_DIR}/include"
    "${asio_SOURCE_DIR}/asio/include"
    "${simplemail_SOURCE_DIR}/src"
)

# Enable C++23 features
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    nlohmann_json::nlohmann_json
    dotenv
    Crow::Crow
    argon2_lib
    OpenSSL::SSL
    OpenSSL::Crypto
    jwt-cpp::jwt-cpp
    mailio::mailio
    inja
    SQLite::SQLite3
    spdlog::spdlog
)

if(NOT CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -O3)
endif()
