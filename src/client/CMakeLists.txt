# ============================================
# Project: GASPK
# File: CMakeLists.txt (client)
# Description: CMake configuration for the client module.
# Author: GASPK Team
# Last Updated: 2025-01-08
# ============================================

# Find the Qt6 package
find_package(Qt6 COMPONENTS Core Network Widgets REQUIRED)

set(CLIENT_SRC
    Client.cpp
    main.cpp
)

set(CLIENT_HEADERS
    Client.h
)

add_executable(client ${CLIENT_SRC} ${CLIENT_HEADERS})

target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(client
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
#    common
)

# Copy config.json to output directory
add_custom_command(TARGET client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/src/client/config.json
        ${CMAKE_BINARY_DIR}/config.json
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Copy config.json to client output directory"
)
