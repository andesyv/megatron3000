target_sources(megatron3000 PRIVATE
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    histogramwidget.cpp
    histogramwidget.ui
    transferfunctionwidget.cpp
    viewport2d.cpp
    viewport3d.cpp
    renderer.cpp
    volume.cpp
    datawidget.cpp
    menuinterface.cpp
    renderer2d.cpp
    renderer3d.cpp
    renderutils.cpp
    materialnode.cpp
)

# Add target for creating embedfile helper program
add_executable(embedfile embedfile.cpp)

add_subdirectory(shaders)