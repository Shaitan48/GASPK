cmake_minimum_required(VERSION 3.16)

project(client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON) # Enables automatic moc generation

find_package(Qt6 COMPONENTS Core Sql Network Widgets REQUIRED)

set(CLIENT_SOURCES
    main.cpp
    Client.cpp
)

add_executable(client ${CLIENT_SOURCES})

target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(client
    Qt6::Core
    Qt6::Sql
    Qt6::Network
    Qt6::Widgets
    common
)

# Copy config.json to output directory
add_custom_command(TARGET client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/src/client/config.json
        ${CMAKE_BINARY_DIR}/config.json
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Copy config.json to client output directory"
)
