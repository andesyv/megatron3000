cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

project(megatron3000 VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


add_executable(megatron3000)
add_compile_definitions(CMAKE)


option(EMBEDDED_SHADERS "Embed shaders into executable" OFF)
if (EMBEDDED_SHADERS)
    add_compile_definitions(EMBEDDED_SHADERS)
endif()


target_include_directories(megatron3000 PRIVATE lib/mINI/src)

find_package(Qt5 COMPONENTS Widgets Gui Concurrent REQUIRED)
find_package(Qt5Charts)
target_link_libraries(megatron3000 PRIVATE Qt::Core Qt::Widgets Qt::Gui Qt::Charts Qt::Concurrent)

add_compile_definitions(SHADERPATH="${CMAKE_SOURCE_DIR}/src/shaders")

# Add project sources
add_subdirectory(src)