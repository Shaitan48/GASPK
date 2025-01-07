
cmake_minimum_required(VERSION 3.16)

project(GASPK)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON) # Enables automatic moc generation

find_package(Qt6 COMPONENTS Core Sql Network REQUIRED)

# Common files
set(COMMON_SOURCES
)
set(COMMON_HEADERS
)

add_library(common ${COMMON_SOURCES})
target_link_libraries(common Qt6::Core Qt6::Network)

# Server
add_subdirectory(src/server)

# Agent
add_subdirectory(src/agent)

# Client
add_subdirectory(src/client)
