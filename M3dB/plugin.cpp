#include "aeffect.h"
#include "aeffectx.h"
#include <stdio.h>

/*
 * I find the naming a bit confusing so I decided to use more meaningful names instead.
 */

/**
 * The VSTHostCallback is a function pointer so that the plugin can communicate with the host (not used in this small example)
 */
typedef audioMasterCallback VSTHostCallback;

/**
 * The VSTPlugin structure (AEffect) contains information about the plugin (like version, number of inputs, ...) and
 * callbacks so that the host can call the plugin to do its work. The primary callback will be `processReplacing` for
 * single precision (float) sample processing (or `processDoubleReplacing` for double precision (double)).
 */
typedef AEffect VSTPlugin;


// Since the host is expecting a very specific API we need to make sure it has C linkage (not C++)
extern "C" {

/*
 * This is the main entry point to the VST plugin.
 *
 * The host (DAW like Maschine, Ableton Live, Reason, ...) will look for this function with this exact API.
 *
 * It is the equivalent to `int main(int argc, char *argv[])` for a C executable.
 *
 * @param vstHostCallback is a callback so that the plugin can communicate with the host (not used in this small example)
 * @return a pointer to the AEffect structure
 */
extern VSTPlugin *VSTPluginMain(VSTHostCallback vstHostCallback);

}

/*
 * Constant for the version of the plugin. For example 1100 for version 1.1.0.0
 */
const VstInt32 PLUGIN_VERSION = 1000;


/**
 * Encapsulates the plugin as a C++ class. It will keep both the host callback and the structure required by the
 * host (VSTPlugin). This class will be stored in the `VSTPlugin.object` field (circular reference) so that it can
 * be accessed when the host calls the plugin back (for example in `processDoubleReplacing`).
 */
class VSTPluginWrapper
{
public:
  VSTPluginWrapper(VSTHostCallback vstHostCallback,
                   VstInt32 vendorUniqueID,
                   VstInt32 vendorVersion,
                   VstInt32 numParams,
                   VstInt32 numPrograms,
                   VstInt32 numInputs,
                   VstInt32 numOutputs);

  ~VSTPluginWrapper();

  inline VSTPlugin *getVSTPlugin()
  {
    return &_vstPlugin;
  }

  inline VstInt32 getNumInputs() const
  {
    return _vstPlugin.numInputs;
  }

  inline VstInt32 getNumOutputs() const
  {
    return _vstPlugin.numOutputs;
  }

private:
  // the host callback (a function pointer)
  VSTHostCallback _vstHostCallback;

  // the actual structure required by the host
  VSTPlugin _vstPlugin;
};

/*******************************************
 * Callbacks: Host -> Plugin
 *
 * Defined here because they are used in the rest of the code later
 */

/**
 * This is the callback that will be called to process the samples in the case of single precision. This is where the
 * meat of the logic happens!
 *
 * @param vstPlugin the object returned by VSTPluginMain
 * @param inputs an array of array of input samples. You read from it. First dimension is for inputs, second dimension is for samples: inputs[numInputs][sampleFrames]
 * @param outputs an array of array of output samples. You write to it. First dimension is for outputs, second dimension is for samples: outputs[numOuputs][sampleFrames]
 * @param sampleFrames the number of samples (second dimension in both arrays)
 */
void VSTPluginProcessSamplesFloat32(VSTPlugin *vstPlugin, float **inputs, float **outputs, VstInt32 sampleFrames)
{
  // we can get retrieve a hold to our C++ class since we stored it in the `object` field (see constructor)
  VSTPluginWrapper *wrapper = static_cast<VSTPluginWrapper *>(vstPlugin->object);

  // code speaks for itself: for each input (2 when stereo input), iterating over every sample and writing the
  // result in the outputs array after multiplying by 0.5 (which result in a 3dB attenuation of the sound)
  for(int i = 0; i < wrapper->getNumInputs(); i++)
  {
    auto inputSamples = inputs[i];
    auto outputSamples = outputs[i];
    for(int j = 0; j < sampleFrames; j++)
    {
      outputSamples[j] = inputSamples[j] * 0.5f;
    }
  }
}

/**
 * This is the callback that will be called to process the samples in the case of double precision. This is where the
 * meat of the logic happens!
 *
 * @param vstPlugin the object returned by VSTPluginMain
 * @param inputs an array of array of input samples. You read from it. First dimension is for inputs, second dimension is for samples: inputs[numInputs][sampleFrames]
 * @param outputs an array of array of output samples. You write to it. First dimension is for outputs, second dimension is for samples: outputs[numOuputs][sampleFrames]
 * @param sampleFrames the number of samples (second dimension in both arrays)
 */
void VSTPluginProcessSamplesFloat64(VSTPlugin *vstPlugin, double **inputs, double **outputs, VstInt32 sampleFrames)
{
  // we can get retrieve a hold to our C++ class since we stored it in the `object` field (see constructor)
  VSTPluginWrapper *wrapper = static_cast<VSTPluginWrapper *>(vstPlugin->object);

  // code speaks for itself: for each input (2 when stereo input), iterating over every sample and writing the
  // result in the outputs array after multiplying by 0.5 (which result in a 3dB attenuation of the sound)
  for(int i = 0; i < wrapper->getNumInputs(); i++)
  {
    auto inputSamples = inputs[i];
    auto outputSamples = outputs[i];
    for(int j = 0; j < sampleFrames; j++)
    {
      outputSamples[j] = inputSamples[j] * 0.5;
    }
  }
}

/**
 * This is the plugin called by the host to communicate with the plugin, mainly to request information (like the
 * vendor string, the plugin category...) or communicate state/changes (like open/close, frame rate...)
 *
 * @param vstPlugin the object returned by VSTPluginMain
 * @param opCode defined in aeffect.h/AEffectOpcodes and which continues in aeffectx.h/AEffectXOpcodes for a grand
 *        total of 79 of them! Only a few of them are implemented in this small plugin.
 * @param index depend on the opcode
 * @param value depend on the opcode
 * @param ptr depend on the opcode
 * @param opt depend on the opcode
 * @return depend on the opcode (0 is ok when you don't implement an opcode...)
 */
VstIntPtr VSTPluginDispatcher(VSTPlugin *vstPlugin, VstInt32 opCode, VstInt32 index, VstIntPtr value, void *ptr, float opt)
{
  printf("called VSTPluginDispatcher(%d)\n", opCode);

  VstIntPtr v = 0;

  // we can get retrieve a hold to our C++ class since we stored it in the `object` field (see constructor)
  VSTPluginWrapper *wrapper = static_cast<VSTPluginWrapper *>(vstPlugin->object);
  // see aeffect.h/AEffectOpcodes and aeffectx.h/AEffectXOpcodes for details on all of them
  switch(opCode)
  {
    // request for the category of the plugin: in this case it is an effect since it is modifying the input (as opposed
    // to generating sound)
    case effGetPlugCategory:
      return kPlugCategEffect;

    // called by the host when the plugin was called... time to reclaim memory!
    case effClose:
      delete wrapper;
      break;

    // request for the vendor string (usually used in the UI for plugin grouping)
    case effGetVendorString:
      strncpy(static_cast<char *>(ptr), "pongasoft", kVstMaxVendorStrLen);
      v = 1;
      break;

    // request for the version
    case effGetVendorVersion:
      return PLUGIN_VERSION;

    // ignoring all other opcodes
    default:
      printf("Unknown opCode %d [ignored] \n", opCode);
      break;
  }

  return v;
}

/**
 * Used for parameter setting (not used by this plugin)
 */
void VSTPluginSetParameter(VSTPlugin *vstPlugin, VstInt32 index, float parameter)
{
  printf("called VSTPluginSetParameter(%d, %f)\n", index, parameter);
  // we can get retrieve a hold to our C++ class since we stored it in the `object` field (see constructor)
  VSTPluginWrapper *wrapper = static_cast<VSTPluginWrapper *>(vstPlugin->object);
}

/**
 * Used for parameter (not used by this plugin)
 */
float VSTPluginGetParameter(VSTPlugin *vstPlugin, VstInt32 index)
{
  printf("called VSTPluginGetParameter(%d)\n", index);
  // we can get retrieve a hold to our C++ class since we stored it in the `object` field (see constructor)
  VSTPluginWrapper *wrapper = static_cast<VSTPluginWrapper *>(vstPlugin->object);
  return 0;
}

/**
 * Main constructor for our C++ class
 */
VSTPluginWrapper::VSTPluginWrapper(audioMasterCallback vstHostCallback,
                                   VstInt32 vendorUniqueID,
                                   VstInt32 vendorVersion,
                                   VstInt32 numParams,
                                   VstInt32 numPrograms,
                                   VstInt32 numInputs,
                                   VstInt32 numOutputs) :
  _vstHostCallback(vstHostCallback)
{
  // Make sure that the memory is properly intialized
  memset(&_vstPlugin, 0, sizeof(_vstPlugin));

  // this field must be set with this constant...
  _vstPlugin.magic = kEffectMagic;

  // storing this object into the VSTPlugin so that it can be retrieved when called back (see callbacks for use)
  _vstPlugin.object = this;

  // specifying that we handle both single and double precision (there are other flags see aeffect.h/VstAEffectFlags)
  _vstPlugin.flags = effFlagsCanReplacing | effFlagsCanDoubleReplacing;

  // initializing the plugin with the various values
  _vstPlugin.uniqueID = vendorUniqueID;
  _vstPlugin.version = vendorVersion;
  _vstPlugin.numParams = numParams;
  _vstPlugin.numPrograms = numPrograms;
  _vstPlugin.numInputs = numInputs;
  _vstPlugin.numOutputs = numOutputs;

  // setting the callbacks to the previously defined functions
  _vstPlugin.dispatcher = VSTPluginDispatcher;
  _vstPlugin.getParameter = VSTPluginGetParameter;
  _vstPlugin.setParameter = VSTPluginSetParameter;
  _vstPlugin.processReplacing = VSTPluginProcessSamplesFloat32;
  _vstPlugin.processDoubleReplacing = VSTPluginProcessSamplesFloat64;

}

/**
 * Destructor called when the plugin is closed (see VSTPluginDispatcher with effClose opCode). In this very simply plugin
 * there is nothing to do but in general the memory that gets allocated MUST be freed here otherwise there might be a
 * memory leak which may end up slowing down and/or crashing the host
 */
VSTPluginWrapper::~VSTPluginWrapper()
{
}

/**
 * Implementation of the main entry point of the plugin
 */
VSTPlugin *VSTPluginMain(VSTHostCallback vstHostCallback)
{
  printf("called VSTPluginMain... \n");

  // simply create our plugin C++ class
  VSTPluginWrapper *plugin =
    new VSTPluginWrapper(vstHostCallback,
                         CCONST('u', 's', 'a', 'n'), // registered with Steinberg (http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm)
                         PLUGIN_VERSION, // version
                         0,    // no params
                         0,    // no programs
                         2,    // 2 inputs
                         2);   // 2 outputs

  // return the plugin per the contract of the API
  return plugin->getVSTPlugin();
}
