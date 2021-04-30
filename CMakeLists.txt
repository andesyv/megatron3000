cmake_minimum_required(VERSION 3.14.0)

project(megatron3000 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Gui Concurrent REQUIRED)

add_executable(megatron3000)
add_compile_definitions(CMAKE)




option(EMBEDDED_SHADERS "Embed shaders into executable" OFF)
if (EMBEDDED_SHADERS)
    add_compile_definitions(EMBEDDED_SHADERS)
endif()


# Add project sources
target_include_directories(megatron3000 PRIVATE lib/mINI/src)
add_subdirectory(src)

find_package(Qt5Charts)
target_link_libraries(megatron3000 PRIVATE Qt::Core Qt::Widgets Qt::Gui Qt::Charts Qt::Concurrent)

add_compile_definitions(SHADERPATH="${CMAKE_SOURCE_DIR}/src/shaders")