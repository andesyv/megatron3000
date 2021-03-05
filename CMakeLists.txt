cmake_minimum_required(VERSION 3.14.0)

project(megatron3000 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Gui REQUIRED)

add_subdirectory(src)

target_link_libraries(megatron3000 PRIVATE Qt::Widgets Qt::Gui)