cmake_minimum_required(VERSION 3.16)
project(arch_installer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(DRY_RUN "Run in dry-run mode" ON)
add_compile_definitions($<$<BOOL:${DRY_RUN}>:ARCHI_DRY_RUN>)

set(PROJ_ROOT ${CMAKE_SOURCE_DIR}/arch-installer)

include_directories(${PROJ_ROOT}/include)

file(GLOB_RECURSE ARCH_SOURCES CONFIGURE_DEPENDS
  "${PROJ_ROOT}/src/*.cpp"
)
message(STATUS "Sources found: ${ARCH_SOURCES}")
if(NOT ARCH_SOURCES)
  message(FATAL_ERROR "Nenhum .cpp encontrado em ${PROJ_ROOT}/src")
endif()

add_executable(arch-installer ${ARCH_SOURCES})

find_package(yaml-cpp REQUIRED CONFIG
  HINTS /opt/homebrew/opt/yaml-cpp /opt/homebrew/opt/yaml-cpp/lib/cmake/yaml-cpp
)
target_link_libraries(arch-installer PRIVATE yaml-cpp)
