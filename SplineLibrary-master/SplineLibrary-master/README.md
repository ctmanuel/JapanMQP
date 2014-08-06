SplineLibrary
=============
A C++ library to collect many useful spline functions into one place.

A spline is a formula for smoothly transitioning from one data point to the next in a data set. For example, you could create a spline containing ten colors (each stored as R, G, and B values) to create a color gradient that smoothly transitions from one color to the next.

Features
-------------
* Interpolation of standard catmull-rom splines
    * Include `spline_library/cubic_hermite/cr_spline.h`, create a `CRSpline` object, and call its `getPosition` method.
    * Several more spline types. See [Spline Types](docs/SplineTypes.md) for the full list
* Looping Splines
    * To make a looping catmull-rom spline, include `spline_library/cubic_hermite/looping_cr_spline.h` and create a `LoopingCRSpline` object instead.
    * Every spline type has both looping and non-looping variants
* Compute the inverse of a spline
    * Given a data point (not necessarily on the spline, or even close to it), what T value brings the spline closest to that data point?
    * Create a SplineInverter object and call its findClosestT method
* Interpolation of the first, second, and third derivatives of the spline
    * The first derivative is called the "tangent" - this is how quickly and in what direction the interpolated position is changing, per T
    * The second derivative is called the "curvature" - this is how quickly and in what direction the interpolated tangent is changing, per T
    * The third derivative is called the "wiggle" - this is how quickly and in what direction the interpolated curvature is changing, per T
    

Documentation
-------------
[Glossary](docs/Glossary.md) - Glossary of important terms for understanding splines.

[Spline class API](docs/SplineAPI.md) - API documentation of the `Spline` base class.

[Spline Types](docs/SplineTypes.md) - Complete list of all supported spline formulas

[Spline Utilities](docs/SplineUtilities.md) - Documentation of some utility classes for splines, including `SplineInverter` and `SplineLengthCalculator`.

Project Layout
-------------
The root of the repository is a Qt Creator project that demonstrates some uses of the library. The source for the spline code itself is in the "spline_library" directory, and the code to set up the demo is in the "demo" directory.

Requirements
-------------

Both the demo and the spline_library code require a fully compliant C++11 compiler. Visual Studio 2012 currently compiles it correctly, but this is not guaranteed to be the case in the future: VS2012 isn't fully compliant, missing useful features like std::initializer_list.

The spline_library code has no third-party dependencies, so it's safe to drop that folder directly in the source folder of your own project.

The demo project requires Qt 5. To build it, either run qmake with the .pro file to generate a makefile, or open the .pro file in qt Creator.

License
-------------
This code is available under the terms of the [Mozilla Public License v2.0](http://mozilla.org/MPL/2.0/)

This project includes the [nanoflann](https://code.google.com/p/nanoflann/) library for fast nearest-neighbor queries, which is available under the [Simplified BSD License](http://opensource.org/licenses/BSD-2-Clause)

To-Do
-------------
* Implement a Generic B-Sline type that supports arbitrary powers, in addition to the cubic-only version. The generic one would almost certainly have worse performance and numerical stability than the cubic-only version, so it's worth keeping both.
* Add support for arbitrary T value differences in the Cubic B-Spline
* Implement "composite splines", ie a spline that is made by combining two or more splines. Current ideas include a "sum spline" where the output for a given T is a sum of all the child spline results at T, and a "concatenation spline" formed simply by starting one spline where the previous leaves off.
