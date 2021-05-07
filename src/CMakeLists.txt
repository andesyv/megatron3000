target_sources(megatron3000 PRIVATE
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    histogramwidget.cpp
    transferfunctionrenderer.cpp
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
    spline.cpp
    nodepropertywidget.ui
    math.cpp
)

add_subdirectory(shaders)