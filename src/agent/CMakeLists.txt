
cmake_minimum_required(VERSION 3.16)

project(agent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON) # Enables automatic moc generation

find_package(Qt6 COMPONENTS Core Sql Network REQUIRED)

set(AGENT_SOURCES
    main.cpp
    Agent.cpp
)

add_executable(agent ${AGENT_SOURCES})

target_include_directories(agent PRIVATE ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(agent
    Qt6::Core
    Qt6::Sql
    Qt6::Network
    common
)

# Copy config.json to output directory
add_custom_command(TARGET agent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/config.json
        ${CMAKE_BINARY_DIR}/config.json
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Copy config.json to agent output directory"
)
