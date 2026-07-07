cmake_minimum_required(VERSION 3.16)

project(
  TapeRot
  VERSION 0.1.0
  LANGUAGES CXX
)

add_executable(taperot
  src/main.cpp
  src/cli/Options.cpp
  src/core/Frame.cpp
  src/core/Random.cpp
  src/effects/ChromaDegrade.cpp
  src/effects/Dropout.cpp
  src/effects/HeadSwitchNoise.cpp
  src/effects/LineJitter.cpp
)

target_compile_features(taperot PRIVATE cxx_std_17)
target_include_directories(taperot PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

if(MSVC)
  target_compile_options(taperot PRIVATE /W4)
else()
  target_compile_options(taperot PRIVATE -Wall -Wextra -Wpedantic)
endif()
