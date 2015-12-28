Lightman
=========

# Introduction
Lightman is a simple library for lighting simulation and HDR image post
production.

![](https://github.com/skygao93/Lightman/blob/master/src/doc/sample.png?raw=true)

# Dependencies
Lightman uses OpenEXR as its main image input format and also supports creating
per-light renderings from ass (Arnold Scene Source) files for post-render
re-lighting using Solid Angle's Arnold renderer. Lightman workspaces are saved
as json and uses Jansson for creating and loading workspaces. All Dependencies
for Lightman are listed below:

* GLEW
* GLFW3
* OpenEXR
* Arnold (Optional)

# Build Instructions
Lightman uses cmake (version > 3.0) for its build system. To install, first
make sure you have all dependencies installed. Then in the Lightman root
directory:

* mkdir build
* cd build
* cmake ..
* make install

# Documentation
Lightman is documented using Doxygen. Uncomment `BUILD_DOCUMENTATION` in the cmake configuration file to generate documentation (doxygen required).

# Using Lightman
There are several ways to build applications using Lightman. The LightmanCore
library offers only the core functionalities of Lightman. To use only the core
features, include `LightmanCore.h` in your code and link with the built
library. If you wish to use Lightman's simple application for display and
basic user interaction, you may simply create an LightmanApplication instance
for which you will also need to include `LightmanApplication.h`. See the
documentation for more details. A `findLIGHTMAN.cmake` module is also provided
to use with cmake.
