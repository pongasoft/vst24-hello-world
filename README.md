Introduction
============

This project contains a "Hello World" style application for building a VST 2.4 plugin: the code is heavily commented to explain what is going on. Also check out the VST2.4 section below for more details on VST.

This plugin has no UI nor any way to configure it. It simply lowers the input signal by 3dB. The point is to have the minimum amount of code to have a fully functionning plugin while demonstrating what is going on and best practices (I know the code could be made even smaller without the C++ wrapper class, but the point is to write realistic code that can be used as a starting point for more advanced plugins).

Motivations
===========

TLDR; Lack of one

Motivations coming soon...

VST 2.4
=======

Coming soon :)

Build
=====

Before building the project for the first time, please refer to the README file under vst2.x.

The main code (`M3dB/plugin.cpp`) for the VST plugin is cross platform compatible. Unfortunately the packaging is not. As a result, this project has only been designed to be built and work on a Mac (OS X) since it is my current development platform. I would definitely be open to contributions for a CMakefile that could build on all platforms.

For the time being, simply open the XCode project (either with XCode itself or AppCode by Jetbrains). Simply build the plugin.

Installation
============

Copy the built bundle (`M3dB.vst` folder) under `$HOME/Audio/Plugin-Ins/VST` and use your favorite VST compliant favorite DAW to load.

License
=======

Apache 2.0