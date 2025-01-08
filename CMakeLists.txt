# ============================================
# Project: GASPK
# File: CMakeLists.txt (root)
# Description: Root CMake configuration for the GASPK project.
# Author: GASPK Team
# Last Updated: 2025-01-08
# ============================================

cmake_minimum_required(VERSION 3.10)
project(GASPK)

# Add subdirectories
add_subdirectory(src/server)
add_subdirectory(src/client)
add_subdirectory(src/agent)

# Include header files
include_directories(src/server)
include_directories(src/client)
include_directories(src/agent)
