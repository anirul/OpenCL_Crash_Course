# OpenCL Crash Course

Simple crash course for OpenCL I wrote for IDIAP. This contain samples and slides.

## Slides

Where made using keynotes but there is a pdf available [here]().

## Examples

### Dependencies

I use some dependencies:
- CMake
- Boost (date time, program options)
- GL / GLU / GLUT
- OpenCV
- OpenCL 1.1

### Simple (C)

Simple example of how to use OpenCL with C/C++.

### Floyd Warshall (C++)

Implementing a fast version of [Floyd Warshall](http://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm). This demonstrate how to recurse and iterate on arrays. Also multiple call to the same kernel.

### Histogram (C++ - OpenCV)

Local memory working group and advance structures in OpenCL. Basic notion of dispatching computation and reducing it in another kernel (2 kernel operation).

### Video (C++ - OpenCV - OpenGL)

Get video from OpenCV make modification with OpenCL and send it back to OpenGL to draw it on screen. It come from my own example [OpenCL Video](http://github.com/anirul/OpenCL_Video). It shows some interaction between OpenCV and OpenCL.