if(EMBEDDED_SHADERS)
    if(CMAKE_MAJOR_VERSION GREATER_EQUAL 3 AND CMAKE_MINOR_VERSION GREATER_EQUAL 20)
        file(GLOB SHADERFILES "*.glsl" "*.vs" "*.gs" "*.fs")
        foreach(shaderfile IN LISTS SHADERFILES)
            # Need CMake 3.20 for cmake_path
            cmake_path(GET shaderfile FILENAME varName)
            file(READ ${shaderfile} ${varName})
        endforeach()

        configure_file(shadermanager.cpp shadermanager.gen.cpp)
        configure_file(shadermanager.h shadermanager.h COPYONLY)
        target_sources(megatron3000 PRIVATE
            ${CMAKE_CURRENT_BINARY_DIR}/shadermanager.gen.cpp
        )
    else()
        message("Embedding shaders require CMake version 3.20 or higher!")
        set(EMBEDDED_SHADERS OFF)
    endif()
endif()

if (NOT EMBEDDED_SHADERS)
    target_sources(megatron3000 PRIVATE
        shadermanager.cpp
    )
endif()