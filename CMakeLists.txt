cmake_minimum_required(VERSION 3.15)
project(LouganisEngine)

set(CMAKE_CXX_STANDARD 17)

# Add engine
add_subdirectory(Engine/Code/Engine)

# Add game
add_subdirectory(Louganis/Code/Game)