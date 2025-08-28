# =========================
# Engine library
# =========================

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Third-party absolute paths
set(TINYXML2_CPP "C:/dev/Personal-Game-Engine/Engine/Code/ThirdParty/tinyxml2/tinyxml2.cpp")

# Gather all Engine sources
file(GLOB_RECURSE ENGINE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Add tinyxml2 source only
list(APPEND ENGINE_SOURCES
    ${TINYXML2_CPP}
)

# Create the static library
add_library(Engine STATIC ${ENGINE_SOURCES})

# Include directories
target_include_directories(Engine PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"                    # Engine/Code/Engine
    "${CMAKE_CURRENT_SOURCE_DIR}/.."                 # Engine/Code
    "${CMAKE_SOURCE_DIR}/Louganis/Code"             # Game headers (EngineBuildPreferences.h)
    "C:/dev/Personal-Game-Engine/Engine/Code/ThirdParty/tinyxml2"
    "C:/dev/Personal-Game-Engine/Engine/Code/ThirdParty/miniaudio"
    "C:/dev/Personal-Game-Engine/Engine/Code/ThirdParty/stb"
)

# Enable MSVC extensions for __declspec when compiling with Emscripten
if(EMSCRIPTEN)
    add_compile_options(-fms-extensions)
endif()

# Desktop vs Web setup
if(NOT EMSCRIPTEN)
    message(STATUS "Building Engine for Native Desktop")
    find_package(SDL2 CONFIG REQUIRED)
    target_include_directories(Engine PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(Engine PRIVATE SDL2::SDL2 SDL2::SDL2main)
else()
    message(STATUS "Building Engine for Emscripten (WASM)")
    # SDL2 flag handled at configure time via -DCMAKE_EXE_LINKER_FLAGS="-s USE_SDL=2"
endif()