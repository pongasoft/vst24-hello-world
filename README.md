Introduction
============

This project contains a "Hello World" style application for building a VST 2.4 plugin: the code is heavily commented to explain what is going on. Also check out the VST2.4 section below for more details on VST.

This plugin has no UI nor any way to configure it. It simply lowers the input signal by 3dB. The point is to have the minimum amount of code to have a fully working plugin while demonstrating what is going on and best practices (I know the code could be made even smaller without the C++ wrapper class, but the point is to write realistic code that can be used as a starting point for more advanced plugins).

Motivations
===========

TLDR; Lack of such project

After building a few [Rack Extensions](https://pongasoft.com/) for Propellerhead's Reason DAW, I wanted to build a VST plugin which I could also deploy in other DAWs. The documentation for building a VST 2.4 plugin turned out to be quite hard to find since this version is old and whatever I could find was not easy to follow and understand. There are a few higher level frameworks (like [Juce](https://juce.com/) or [wdl-old](https://github.com/olilarkin/wdl-ol)) that may be better to build a more complex application, but they have their own learning curves and hide (for better or for worse) how the underlying infrastructure actually work.

For the same reason that I strongly believe every single developer should write their own compiler once, even if in the end you will end up using the standard ones (like gcc, clang, etc...), I believe it is important to understand how a VST plugin is built at its lowest level. This will give you a solid foundation and better understanding of the trade offs that higher level frameworks have to make.

In the end, the core concepts of VST 2.4 are not very complicated. I only think they are a bit hard to understand due to the lack of documentation and simple examples.

This project is meant to bridge this gap.
 

VST 2.4
=======

See [A Beginner's Guide to VST 2.4](./A_Beginners_Guide_To_VST2.4.md).

Build
=====

Before building the project for the first time, please refer to the README file under vst2.x.

The main code (`M3dB/plugin.cpp`) for the VST plugin is cross platform compatible. Unfortunately the packaging is not. As a result, this project has only been designed to be built and work on a Mac (OS X) since it is my current development platform. I would definitely be open to contributions for a CMakefile that could build on all platforms.

For the time being, simply open the XCode project (either with XCode itself or AppCode by Jetbrains). Simply build the plugin.

Installation
============

Copy the built bundle (`M3dB.vst` folder) under `$HOME/Library/Audio/Plugin-Ins/VST` and use your favorite VST compliant favorite DAW to load.

Binary
======
If you want to install this plugin without building it (on macOS only), [download](./releases/download/v1.0.0/M3dB.vst.zip) the file, unzip and copy `M3dB.vst` under `$HOME/Library/Audio/Plug-Ins/VST`

Integrity check `md5 M3dB.vst.zip => c4e81b73a7e848bc9f9362f14aff4983`.



License
=======

Apache 2.0