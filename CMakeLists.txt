cmake_minimum_required(VERSION 3.16)

project(
  TapeRot
  VERSION 0.1.0
  LANGUAGES CXX
)

add_library(taperot_core
  src/cli/Options.cpp
  src/core/Frame.cpp
  src/core/Random.cpp
  src/effects/ChromaDegrade.cpp
  src/effects/Dropout.cpp
  src/effects/EffectPipeline.cpp
  src/effects/HeadSwitchNoise.cpp
  src/effects/LineJitter.cpp
)

target_compile_features(taperot_core PUBLIC cxx_std_17)
target_include_directories(taperot_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(taperot
  src/main.cpp
)

target_link_libraries(taperot PRIVATE taperot_core)

add_executable(taperot_tests
  tests/test_main.cpp
)

target_link_libraries(taperot_tests PRIVATE taperot_core)

enable_testing()
add_test(NAME taperot_tests COMMAND taperot_tests)

if(MSVC)
  target_compile_options(taperot_core PRIVATE /W4)
  target_compile_options(taperot PRIVATE /W4)
  target_compile_options(taperot_tests PRIVATE /W4)
else()
  target_compile_options(taperot_core PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(taperot PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(taperot_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()
