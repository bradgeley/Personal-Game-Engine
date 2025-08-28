# =========================
# Louganis Game
# =========================

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Gather all game sources
file(GLOB_RECURSE GAME_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Create executable
add_executable(Louganis ${GAME_SOURCES})

# Include Engine headers
target_include_directories(Louganis PUBLIC
    "C:/dev/Personal-Game-Engine/Engine/Code/Engine"
    "C:/dev/Personal-Game-Engine/Engine/Code"
)

# Link Engine static library
target_link_libraries(Louganis PRIVATE Engine)

# Emscripten settings
if(EMSCRIPTEN)
    set_target_properties(Louganis PROPERTIES LINK_FLAGS "-s USE_SDL=2 -s ALLOW_MEMORY_GROWTH=1")
endif()