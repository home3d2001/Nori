Wendy library
=============

Introduction
------------

Wendy is a C++ library intended to be useful for the production of small games.

Wendy (for now) resides in a public GitHub repository:

  https://github.com/elmindreda/Wendy


License and copyright
---------------------

Wendy is copyright (c) 2004 Camilla Berglund <elmindreda@elmindreda.org>

Wendy is licensed under the zlib/libpng license.  See `COPYING.txt` for license
details.  The license is also included at the top of each source file.

Wendy comes bundled with a number of dependencies, each with its own license.
See the respective `libs/*/COPYING.txt` file for license details.


Building Wendy
--------------

Wendy uses the [CMake](http://www.cmake.org/) build system.

Wendy comes with internal versions of the following libraries:

*   [Bullet] (http://bulletphysics.org/) for dynamics and collision
*   [ENet](http://enet.bespin.org/) for UDP networking
*   [GLEW](http://glew.sourceforge.net/) for managing OpenGL extensions
*   [GLFW](http://www.glfw.org/) for OpenGL context, input and window
*   [GLM](http://glm.g-truc.net/) for vector math and explicitly sized types
*   [libogg](http://www.xiph.org/ogg/) for reading Ogg files
*   [libpng](http://www.libpng.org/) for reading and writing PNG files
*   [libvorbis](http://xiph.org/vorbis/) for decoding Vorbis audio data
*   [PCRE](http://www.pcre.org/) for regular expression support
*   [pugixml](http://pugixml.org/) for reading and writing XML files
*   [Squirrel](http://squirrel-lang.org/) for embedded scripting
*   [zlib](http://zlib.net/) for compressed data streams

Wendy depends on the availability of OpenGL 3.2 and OpenAL 1.1.

On Debian GNU/Linux based systems, where GLFW also needs Xlib, the following
packages provide the necessary support:

*   `xorg-dev`
*   `libopenal-dev`
*   `libgl1-mesa-dev`

On other Linux systems and other Unix-like systems, there will most likely be
similarly named packages available.

On Mac OS X, OpenGL and OpenAL is already built-in.

On Windows, OpenGL headers and link libraries should be provided by your
compiler.  For OpenAL, you will need to install the OpenAL SDK.


Using Wendy
-----------

Wendy is documented using the Doxygen suite.  A suitable Doxyfile will be
generated by CMake in the docs/ subdirectory.

The render system requires OpenGL 3.2.

The following OpenGL extensions are used where available:

*   `GL_ARB_texture_float`
*   `GL_ARB_half_float_pixel`
*   `GL_ARB_debug_output`
*   `GL_EXT_texture_filter_anisotropic`


Hacking Wendy
-------------

Sorry, you're on your own for now.


Bugs, feedback and patches
--------------------------

Please send bug reports, feedback, patches and cookies to:

  Camilla Berglund <elmindreda@elmindreda.org>


