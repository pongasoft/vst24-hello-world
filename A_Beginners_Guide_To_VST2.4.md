A Beginner's Guide to VST 2.4
=============================

History
-------
The [steinberg](https://www.steinberg.net/en/home.html) company released the original VST specification back in 1996. The 2.0 branch was released in 1999. The 3.0 branch was released in 2008 and the 2.x branch is no longer supported since the end of 2013.

(source [Wikipedia](https://en.wikipedia.org/wiki/Virtual_Studio_Technology))


Why VST 2.4?
------------
It is fair to wonder why (as of this writing in early 2018), I chose to create a project about VST 2.4. The reason is actually quite simple. When steinberg released 3.0, they decided to make it not backward compatible. 2.4 is the last version released of the 2.x branch. Due to the immense popularity of the format, and the fact that (at least at the time), the 3.x branch was not bringing enough to justify porting all the plugins to the new version, the format has continued to strive. Even despite steinberg officially ending its support at the end of 2013, the format has continued to strive.

In the end, it is a catch 22 problem: every DAW needs to support the format if they want to give access to the huge library of VST 2.4 plugin. Reason, one of the latest DAW to embrace the format (in their version 9.5 released in [spring 2017](https://www.propellerheads.se/blog/when-hell-froze-over-ernst-nathorst-boos-on-vst-in-reason)), only supports this VST version (as of this writing). Because of the fact that this version is so widely supported, there is less incentive to write a plugin which will be supported by less hosts.

Of course, some frameworks (like [Juce](https://juce.com/)) allow to hide all this and even allow to build for various platforms and plugin types (like Apple Audio Unit for example) with the same code.

But for the reasons explained in the motivation section, if you want to build a VST 2.4 plugin the materials can be hard to come by. The rest of this document will only discuss VST 2.4. Some aspects may be valid for other versions of VST (or other plugins) but in general assume it is 2.4 specific.

What is a VST plugin?
---------------------
A plugin, in general, is a piece of software that is (usually) loaded at runtime to _extend_ the capabilities of a given piece of software that is not part of the original software.

A VST plugin is a plugin in the context of a music application software commonly called DAW (Digital Audio Workstation) like Abbleton's Live, Propellerhead's Reason, etc... The purpose being to _extend_ the capability of the DAW to add effects, instruments, etc... not builtin originally. Note that VST plugin can also work in the context of less _traditional_ DAWs like [MrsWatson](http://teragonaudio.com/MrsWatson).

There are various types of audio plugins, like the Apple Audio Unit format, AAX, RTAS, etc... A VST plugin is an audio plugin format developed by steinberg (see introduction).

When you distill it down to its core, a VST plugin is nothing more than:

- a set of APIs in C (structs, enums, typedefs) 
- a set of usage conventions (not enforced by API)
- a packaging convention (a macOS bundle on Mac, a DLL on Windows, ...)

What is a VST Host?
-------------------
A VST plugin is loaded in a music application software commonly called DAW or others (like MrsWhatson) and is commonly referred to as the VST Host since it is _hosting_ the VST plugin.

The VST Host is in charge of implementing the _other_ side of the APIs and conventions (loading, instantiating, communicating, etc...). Note that the VST SDK does not come with any implementation on the host side so each host implements their own version.

VST SDK
-------

#### Obtaining the SDK
For licensing reasons, you need to download the VST SDK from [steinberg](https://www.steinberg.net/en/company/developers.html) (3.6.8 as of 2018/01/01)

You can no longer download the VST 2.4 SDK and instead you have to download the VST 3 SDK, but it contains the 2.4 version. Also, although the VST3 SDK is open source (under a [dual licensing including GPL3](https://github.com/steinbergmedia/vst3sdk)), version 2.4 is explicitly excluded so you need to get it yourself.

#### Anatomy of the SDK
The entire VST 2.4 SDK is comprised of 2 files: `aeffect.h` and `aeffectx.h` located under `VST2_SDK/pluginterfaces/vst2.x` in the downloaded archive. There is no dependency on anything else so in order to build and compile a basic VST 2.4 plugin, you only need to include `aeffectx.h` since it includes the other file.

`aeffectx.h` contains extensions added after version 1.0. For example, in `aeffect.h` the enum `AEffectOpcodes` defines the set of opCodes used by the `AEffectDispatcherProc` function pointer callback and in `aeffectx.h` the enum `AEffectXOpcodes` _extends_ the number of opCodes: 

    effProcessEvents = effSetChunk + 1 // effSetChunk is the last opCode in AEffectOpcodes

The SDK comprises a set of C structs, enums and function pointers definitions which define the way the host and the plugin communicate with each other.

#### Plugin Lifecycle
Based entirely on convention, the host will locate the plugin executable (packaging and location depends on platform). The host will then look for a function with the following signature (by convention):

    AEffect *VSTPluginMain(audioMasterCallback vstHostCallback) 

or if you prefer with more readable/meaningful types:

    typedef audioMasterCallback VSTHostCallback;
    typedef AEffect VSTPlugin;
    VSTPlugin *VSTPluginMain(VSTHostCallback vstHostCallback);

This function, written by the plugin developer acts as a factory of plugins:
- it receives the host callback which is used by the plugin to communicate with the host (for example to get the sample rate (opCode `audioMasterGetSampleRate`))
- it returns an instance of the `AEffect` (aka `VSTPlugin`) structure defined by the API

This structure contains information that the host requires, like the number of inputs and outputs as well as 5 function pointers which defines the callbacks that the host will use to interact with the plugin.

For example, `AEffect.dispatcher` is the main callback that the host will use to communicate with the plugin. As an example, the opCode `effClose` is used when the plugin is closed and as such the plugin code should clean up any resources.

#### Processing samples
The callback `AEffect.processReplacing` (and `AEffect.processDoubleReplacing` for double precision) is the callback that the host will call repeatedly to process samples in single precision (respectively double precision). This is where the main processing of the plugin happens.

#### More details
The file [plugin.cpp](./M3dB/plugin.cpp) serves as a basic example and explain the main concepts and interactions between the host and the plugin (for example how to get a hold of the plugin in the various callbacks, how to implement the `dispatcher` callback, etc...).

#### Packaging & Deployment
The VST plugin packaging and deployment depend entirely on the platform. On macOS, it needs to be a bundle. By convention, on macOS, the host will look into several folders like `$HOME/Library/Audio/Plugin-Ins/VST` or `/Library/Audio/Plugin-Ins/VST` for VST 2.4 plugins.

See [README](./README.md) for more details on how to build/deploy this example (on macOS for the moment).