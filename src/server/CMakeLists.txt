# ============================================
# Project: GASPK
# File: CMakeLists.txt (server)
# Description: CMake configuration for the server module.
# Author: GASPK Team
# Last Updated: 2025-01-08
# ============================================

# Find the Qt6 package
find_package(Qt6 REQUIRED COMPONENTS Core Sql Network)

set(SERVER_SRC
    Server.cpp
    main.cpp
)

set(SERVER_HEADERS
    Server.h
)

add_executable(server ${SERVER_SRC} ${SERVER_HEADERS})

target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(server
    Qt6::Core
    Qt6::Sql
    Qt6::Network
#    common
)

# Copy config.json to output directory
add_custom_command(TARGET server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/src/server/config.json
        ${CMAKE_BINARY_DIR}/config.json
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Copy config.json to server output directory"
)
