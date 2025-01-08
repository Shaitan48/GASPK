cmake_minimum_required(VERSION 3.16)

project(server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON) # Enables automatic moc generation

find_package(Qt6 COMPONENTS Core Sql Network REQUIRED)

set(SERVER_SOURCES
    main.cpp
    Server.cpp
)

add_executable(server ${SERVER_SOURCES})

target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(server
    Qt6::Core
    Qt6::Sql
    Qt6::Network
    common
)

# Copy config.json to output directory
add_custom_command(TARGET server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/src/server/config.json
        ${CMAKE_BINARY_DIR}/config.json
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Copy config.json to server output directory"
)
