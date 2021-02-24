cmake_minimum_required(VERSION 3.16.0)

project(megatron3000 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

#  Add the src/CMakeLists.txt for source files
add_subdirectory(src)

target_link_libraries(megatron3000 PRIVATE Qt6::Widgets)