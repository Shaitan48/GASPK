# ============================================
# Project: GASPK
# File: CMakeLists.txt (agent)
# Description: CMake configuration for the agent module.
# Author: GASPK Team
# Last Updated: 2025-01-08
# ============================================

# Find the Qt6 package
find_package(Qt6 REQUIRED COMPONENTS Core Sql Network)

set(AGENT_SRC
    Agent.cpp
    main.cpp
)

set(AGENT_HEADERS
    Agent.h
)

add_executable(agent ${AGENT_SRC} ${AGENT_HEADERS})

target_include_directories(agent PRIVATE ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(agent
    Qt6::Core
    Qt6::Sql
    Qt6::Network
#    common
)

# Copy config.json to output directory
add_custom_command(TARGET agent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/src/agent/config.json
        ${CMAKE_BINARY_DIR}/config.json
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Copy config.json to agent output directory"
)
