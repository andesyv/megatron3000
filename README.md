<a href="https://github.com/andesyv/megatron3000"><h1>Megatron 3000</h1></a>

[![Build](https://github.com/andesyv/megatron3000/actions/workflows/build.yml/badge.svg)](https://github.com/andesyv/megatron3000/actions/workflows/build.yml)

**Megatron 3000** is a volume renderer for visualization of medical CT scan data. This project was the
course project for the [INF252 - Visualization course at the University of Bergen](https://vis.uib.no/courses/inf252/).

# Table of Contents
- [Table of Contents](#table-of-contents)
- [Design](#design)
- [Overview](#overview)
  - [Transfer Function View](#transfer-function-view)
  - [2D View](#2d-view)
  - [3D View](#3d-view)
  - [Histogram View](#histogram-view)
  - [Other stuff](#other-stuff)
    - [Multithreading](#multithreading)
- [Task Completion](#task-completion)
- [Shortcuts](#shortcuts)
  - [Global shortcuts](#global-shortcuts)
  - [View shortcuts](#view-shortcuts)
    - [3D View](#3d-view-1)
- [Dependencies](#dependencies)
- [Building with CMake](#building-with-cmake)
- [Development setup](#development-setup)
  - [QMake](#qmake)
  - [CMake](#cmake)
    - [Visual Studio](#visual-studio)
  - [Deployment](#deployment)
    - [Windows](#windows)
- [Licence](#licence)

# Design
The project was designed using the [Five Design Sheet](http://fds.design) methodology. The final sheet (5) is supplied in the [design.pdf](design.pdf) file.
# Overview
![Demo](demo.png)

**Megatron3000** features different tools to view and manipulate volumetric data. The program is focused around the use of smaller windows called *Views*. The program features 4 different views that can be moved around, popped out into a separate windows, rearranged, added and closed. All views can manipulate
/ display the same volumetric data, or can individually be linked against other data. Views displaying
/ modifying the same data are linked such that changes in representation are reflected in other views.

## Transfer Function View
The *transfer function view* represents a visual representation of the underlying transfer function
of the volume. A transfer function describes how the different densities of the volume is displayed.
In the representation, the horizontal axis represents the different densities and the vertical axis
represents the rendered opacity of the corresponding density. The color of the transfer function is
decided by the spherical *nodes*, which is both visualized in the 2D and the 3D view.

You can double click in the view to add nodes, which are connected by splines to create the transfer 
function. Nodes can be selected by leftclicking on them. A selected node can have its color changed 
by clicking the **color** property and selecting a color with the pop-up GUI. The color of the node
is displayed in the transfer function view, and reflected in the generated transfer function. Nodes
can be removed by right clicking them.

## 2D View
The *2D View* display slices of the data in grayscale, faithfull to how the data was originally
represented, where a solid color is the highest density and no color is the lowest density. The
representation of the view can be modified by the transfer function view however, unlike the 3D view,
the representation is unaffected by the opacity of the transfer function.

The 2D view can show slices across any of the major axis (x, y and z), any arbitrary axis, or an axis
aligned with the slicing plane in the 3D view. The current axis can be changed in the view's ***View***
menu. When aligned with the major axis, movement is limited to that axis. When aligned with the slicing
plane axis, movements in the 2D view will be reflected on the slicing plane. This makes adjusting the
slicing plane via the 2D view most likely the preferred method of moving the slicing plane.

## 3D View
The *3D View* displays a volumetric rendering of the volume, implemented using raymarching. The visual
representation of the rendering is heavily affected by the *transfer function view*.

The 3D View has an optional slicing plane that can enabled to "cut" the volume in half. This can be
useful for displaying the inner parts of the volume. The slicing plane can be enabled and disabled under
the ***Slicing*** menu. To rotate and move the slicing plane, the slicing plane can be linked and
unlinked to the camera such that movements to the camera will move be reflected on the slicing plane.
You can also move the slicing plane by using the slicing plane in a *2D View*.

There's a small ball glyph right below the axis glyph that can be used dragged with left click in order to
change the light direction of the rendering.

Shortcuts for 3D View:
 - <kbd>S</kbd> Enable / disable slicing plane
 - <kbd>L</kbd>/<kbd>Right Mouse Button</kbd> Enable / disable linked camera for slicing plane

## Histogram View
The *Histogram View* generates a histogram of the data. The histogram displays the count of the
different densities in the volume, where the horizontal axis represents the different density
*"bins"*, and the vertical axis shows the relative amount in relation to the whole volume.

The *bin count*, which is the count of different values you would like to measure, can be changed from
the ***View*** menu. In the same menu you can also map the histogram to the transfer function, which
will multiply the histogram by the transfer function. This is usefull to filter the histogram such that
the representation corresponds to the histogram.

The histogram is generated in the background using separate worker threads and will reflect any changes
in the connected data. If the histogram is mapped to the transfer function, any changes in the transfer
function will regenerate the histogram. Any changes to the bin count will also regenerate the histogram.

## Other stuff
### Multithreading
All expensive tasks the program has to do are running on separate threads from the main program,
making it possible to still use the program while stuff happens in the background. In practice this
means data loading and histogram generation. The worker threads are also cancellable.
For data loading this means that if the user decides to load a different volume mid-loading or cancel
the loading process, they can just click on another file or cancel the data window, respectively. For
histograms this means that if the user changes the data mid histogram-generation, the generation will
cancel and start a new generation as soon as it's convenient.

# Task Completion
In the task description of the course, the product was made for a fictional client, Dr. Strangevis.
According to the various wishes of the fictional client, the following wishes should have been
fullfilled by our application:

Id | Task description | Completion
-- | ---------------- | ----------
**T01** | I want to se a histogram of all the data values | ✅
**T02** | I want to be able to scroll through cross-sectional images along each of the main axes |✅
**T03** | I want to interactively control how colors are assigned to different structures | ✅
**T04** | I want to see the data values along a selected line of sight (ray) through the volume |❌
**T05** | I want to see an image that looks like a standard x-ray |✅ *(The grayscale slices looks like x-rays.)*
**T06** | I want to see which structures in the volume contain the highest data values |❌ *(Using the transfer function you can find the parts of the volume with the highest data values, but indirectly.)*
**T07** | I want to see the 3d surfaces associated with different data values | ✅
**T08** | I want to simultaneously see internal and external structures | ✅
**T09** | I want to see regions where the data values change a lot | ✅
**T10** | I want to be able to control a virtual light source | ✅
**T11** | I want to be able to cut open the volume to see its interior | ✅
**T12** | I want to select a range of data values and see the resulting 3D structures in relation to the whole volume | ❌
**T13** | I want to select a position in a cross-sectional view and see where this point is located in 3D | ❌
**T14** | I want to select a poition in a 3D view and see this point in a cross-sectional view | ❌
**T15** | I want to see the gradient direction and magnitude for a selected position within the volume | ❌
**T16** | I want to see a magnified view of a specified region of the volume | ❌
**T17** | I want to see multiple different views of the same dataset side-by-side | ✅
**T18** | I want to compare two different datasets side-by-side | ✅
**T19** | I want to compare two different datasets in the same view | ❌
**T20** | I want to measure distances to images in real-world coordinates | ❌
**T21** | I want to switch between orthographic and perspective projection in 3D views | ❌
**T22** | I want to assign textures to use different rendering styles for different structures in the volume | ❌
**T23** | I want to be able to generate realistic images with shadows | ❌
**T24** | I want to interactively cut out parts of the volume and examine them in detail | ❌
**T25** | I want to see an overview of the whole dataset | ✅ *(The histogram)*
**T26** | I want to be able to rotate cross-sectional planes | ✅
**T27** | I want to be able to zoom in on regions without losing context | ✅
**T28** | I want to be able to select and control the appearance of salient structures | ❌

# Shortcuts
The application has two sets of shortcuts: *Global shortcuts* and *view shortcuts*. Global shortcuts
operate on the whole program, while view shortcuts affect the currently *focused* view. The view in
focus is indicated by having a cyanish title bar.

## Global shortcuts
 - <kbd>Tab</kbd> Cycles the view focus. You can also left click in a view to change the focus to that view.
 - <kbd>Ctrl+O</kbd> Opens a pop-up menu for loading a volume.
 - <kbd>Ctrl+Shift+O</kbd> Opens the last opened volume.
   - When you load a volume the application will create a file called `lastopened`, which will
   contain a path to the file last opened. <kbd>Ctrl+Shift+O</kbd> loads the volume from this
   file, and reverts to normal file opening if the file isn't found.
 - <kbd>Ctrl+1</kbd> Create a new *2D View*
 - <kbd>Ctrl+2</kbd> Create a new *3D View*
 - <kbd>Ctrl+3</kbd> Create a new *Tranfer Function View*
 - <kbd>Ctrl+4</kbd> Create a new *Histogram View*
 - <kbd>F5</kbd> Reload shaders (***only enabled when using a debug build***)

## View shortcuts
 - <kbd>Ctrl+W</kbd> Close the current view.

### 3D View
 - <kbd>S</kbd> Enable / disable slicing plane
 - <kbd>L</kbd>/<kbd>Right Mouse Button</kbd> Enable / disable linked camera for slicing plane

# Dependencies
The project mostly uses Qt 5.15.6 which is supplied via [vcpkg](https://github.com/microsoft/vcpkg). The project also uses [pulzed/mINI](https://github.com/pulzed/mINI)
to load INI files, which is supplied as a git submodule. To load all submodules you can use the following command:
```
git submodule update --init --recursive
```

# Building with CMake
*Note*: The CMake project automatically fetches the required libraries, but they are huge so it's highly recommended installing the qt libraries in [vcpkg.json](vcpkg.json) locally with vcpkg before configuring the project.

First run CMake to configure the project:
```
cmake -B build .
```
And then build (*Debug* or *Release*):
```
cmake --build build --config Release
```

# Development setup
The project uses two build systems that you can choose from when building: QMake and CMake.

## QMake
Definitively the easiest setup. Using either a Qt extension or Qt creator itself, configure the project using the [megatron3000.pro](megatron3000.pro) file.

## CMake
The project is additionally setup using CMake as an optional approach. External dependencies are automatically setup by vcpkg and CMake, however it is highly recommended to locally install the qt packages manually using vcpkg before configuring the project as they are very large.

If you already have the Qt toolsets installed on your local machine you can also directly link to them instead of downloading via vcpkg. To do so:
  1. First, remove the qt library references in the [vcpkg.json](vcpkg.json) file.
  2. Link to the Qt installation toolset root folder while configuring CMake by specifying the prefix path. E.g.: `cmake -G "Visual Studio 16 2019" -DCMAKE_PREFIX_PATH="D:\Qt\5.15.2\msvc2019_64"`

### Visual Studio
There are two ways of setting up a development environment in Visual Studio: using the CMake CLI or using the build-in CMake support in Visual Studio, the latter being the easiest.

**CLI**:

Using the CLI you can generate a solution file using something like this:
```
cmake -G "Visual Studio 16 2019"
```
Replace `Visual Studio 16 2019` with whatever version of Visual Studio you're using. Building the solution as normal in Visual Studio will then build the project, but you will have to manually set up a debugging target and deployment on your own.

**CMake Extension**:

Visual Studio 2017 and later has build-in support for CMake. To use this you open up the editor, go to File -> Open -> CMake and then navigate to and select the root `CMakeLists.txt` in this project.

## Deployment
### Windows
If using Windows you might need to dynamically link to shared libraries. To do this you need to have all the dynamically linked libraries loaded in a path the executable have access to, like the working directory or system32. Fortunately Qt comes with a program to setup dynamically linked libraries for us called `windeployqt`. Simply run this program, located in the `bin` folder for your Qt installation, with the built executable as an argument and it will set up all shared libraries in the current working directory.

# Licence
Starting with version 1.1 this software and all source code is open source!, and licensed under [LICENSE](LICENSE).

The version before is copyrighted © 2021 by Sigurd Aleksander Sagstad and Anders Syvertsen.