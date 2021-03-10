cmake_minimum_required(VERSION 3.14.0)

project(megatron3000 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Gui REQUIRED)

add_executable(megatron3000)





option(EMBEDDED_SHADERS "Embed shaders into executable" OFF)
if (EMBEDDED_SHADERS)
    add_compile_definitions(EMBEDDED_SHADERS)
    
    
    # Shader bundling:
    # Annoyingly this has to be done in the top CMakeLists.txt for some stupid reason
    # It forwards the shader source files to the output folder and links them together
    # with the executable via a hack.
    # Ref: https://hamelot.io/visualization/opengl-glsl-shader-as-a-string/
    set(SHADER_INPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src/shaders)
    set(SHADER_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
    add_custom_command(
        OUTPUT  ${SHADER_OUTPUT_DIRECTORY}/default.vs.h
        COMMAND embedfile DEFAULT_VS ${SHADER_INPUT_DIRECTORY}/default.vs ${SHADER_OUTPUT_DIRECTORY}/default.vs.h
        DEPENDS ${SHADER_INPUT_DIRECTORY}/default.vs
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    add_custom_command(
        OUTPUT  ${SHADER_OUTPUT_DIRECTORY}/default.fs.h
        COMMAND embedfile DEFAULT_FS ${SHADER_INPUT_DIRECTORY}/default.fs ${SHADER_OUTPUT_DIRECTORY}/default.fs.h
        DEPENDS ${SHADER_INPUT_DIRECTORY}/default.fs
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    target_sources(megatron3000 PRIVATE
        ${CMAKE_BINARY_DIR}/shaders/default.vs.h
        ${CMAKE_BINARY_DIR}/shaders/default.fs.h
    )
    include_directories(${SHADER_OUTPUT_DIRECTORY})
endif()


# Add project sources
add_subdirectory(src)
target_link_libraries(megatron3000 PRIVATE Qt::Widgets Qt::Gui)

add_compile_definitions(SHADERPATH="${CMAKE_SOURCE_DIR}/src/shaders")