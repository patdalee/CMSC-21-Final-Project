cmake_minimum_required(VERSION 3.16)
project(SlitherAndShift CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# SFML dependency
#   Option A (recommended): install SFML system-wide and rely on find_package.
#     • Ubuntu/Debian: sudo apt-get install libsfml-dev
#     • macOS:         brew install sfml
#     • Windows:       download from https://www.sfml-dev.org and set
#                      SFML_DIR to the cmake/ folder in your SFML installation.
#   Option B: set the SFML_DIR cmake variable manually:
#     cmake -DSFML_DIR=/path/to/SFML/lib/cmake/SFML ..
# ---------------------------------------------------------------------------
find_package(SFML REQUIRED COMPONENTS graphics window system)

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
set(SOURCES
    src/main.cpp
    src/Renderer.cpp
    src/Dice.cpp
    src/Snake.cpp
    src/Ladder.cpp
    src/Board.cpp
    src/Game.cpp
)

add_executable(SlitherAndShift ${SOURCES})

target_include_directories(SlitherAndShift PRIVATE include)

target_link_libraries(SlitherAndShift PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network)

# ---------------------------------------------------------------------------
# Compiler warnings
# ---------------------------------------------------------------------------
if(MSVC)
    target_compile_options(SlitherAndShift PRIVATE /W4)
else()
    target_compile_options(SlitherAndShift PRIVATE
        -Wall -Wextra -Wpedantic -Wshadow
    )
endif()

# ---------------------------------------------------------------------------
# Copy font alongside the executable (if available in the project root)
# ---------------------------------------------------------------------------
if(EXISTS "${CMAKE_SOURCE_DIR}/arial.ttf")
    configure_file("${CMAKE_SOURCE_DIR}/arial.ttf"
                   "${CMAKE_BINARY_DIR}/arial.ttf" COPYONLY)
endif()
