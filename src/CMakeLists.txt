target_sources(megatron3000 PRIVATE
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    histogramwidget.cpp
    histogramwidget.ui
    transferfunctionwidget.cpp
    transferfunctionwidget.h
    viewport2d.cpp
    viewport3d.cpp
    renderer.cpp
    volume.cpp
    datawidget.cpp
    menuinterface.cpp
    renderer3d.cpp
)

# Add target for creating embedfile helper program
add_executable(embedfile embedfile.cpp)

add_subdirectory(shaders)