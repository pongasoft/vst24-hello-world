#include "aeffect.h"
#include "aeffectx.h"
#include <stdio.h>

/*

/Volumes/Vault/misc/src/MrsWatson/bin/Mac\ OS\ X/mrswatson64 --plugin MyCocoPlugin

 */


extern "C" {

// Main method exported by the VST plugin
extern AEffect *VSTPluginMain(audioMasterCallback audioMaster);

}

// defining better names than the one in the api...
typedef audioMasterCallback VSTHostCallback;
typedef AEffect VSTPlugin;

const VstInt32 PLUGIN_VERSION = 1000;

class VSTPluginWrapper
{
public:
  VSTPluginWrapper(VSTHostCallback vstHostCallback);

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

  void initPlugin(VstInt32 vendorUniqueID,
                  VstInt32 vendorVersion,
                  VstInt32 numParams,
                  VstInt32 numPrograms,
                  VstInt32 numInputs,
                  VstInt32 numOutputs);

private:
  VSTHostCallback _vstHostCallback;
  VSTPlugin _vstPlugin;
};

VstIntPtr VSTPluginDispatcher(VSTPlugin *vstPlugin, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *ptr, float opt)
{
  printf("called VSTPluginDispatcher(%d)\n", opcode);

  VstIntPtr v = 0;


  VSTPluginWrapper *wrapper = static_cast<VSTPluginWrapper *>(vstPlugin->object);
  switch(opcode)
  {
    case effGetPlugCategory:
      return kPlugCategEffect;

    case effClose:
      delete wrapper;
      break;

    case effGetVendorString:
      strncpy(static_cast<char *>(ptr), "pongasoft", kVstMaxVendorStrLen);
      v = 1;
      break;

    case effGetVendorVersion:
      return PLUGIN_VERSION;

    default:
      printf("Unknown opcode %d [ignored] \n", opcode);
      break;
  }

  return v;
}

void VSTPluginProcessSamplesFloat32(VSTPlugin *vstPlugin, float **inputs, float **outputs, VstInt32 sampleFrames)
{
  VSTPluginWrapper *wrapper = static_cast<VSTPluginWrapper *>(vstPlugin->object);

  for(int i = 0; i < wrapper->getVSTPlugin()->numInputs; i++)
  {
    auto inputSamples = inputs[i];
    auto outputSamples = outputs[i];
    for(int j = 0; j < sampleFrames; j++)
    {
      outputSamples[j] = inputSamples[j] * 0.5f;
    }
  }
}

void VSTPluginProcessSamplesFloat64(VSTPlugin *vstPlugin, double **inputs, double **outputs, VstInt32 sampleFrames)
{
  VSTPluginWrapper *wrapper = static_cast<VSTPluginWrapper *>(vstPlugin->object);

  for(int i = 0; i < wrapper->getVSTPlugin()->numInputs; i++)
  {
    auto inputSamples = inputs[i];
    auto outputSamples = outputs[i];
    for(int j = 0; j < sampleFrames; j++)
    {
      outputSamples[j] = inputSamples[j] * 0.5;
    }
  }
}

void VSTPluginSetParameter(VSTPlugin *vstPlugin, VstInt32 index, float parameter)
{
  printf("called VSTPluginSetParameter(%d, %f)\n", index, parameter);
  VSTPluginWrapper *wrapper = static_cast<VSTPluginWrapper *>(vstPlugin->object);
}

float VSTPluginGetParameter(VSTPlugin *vstPlugin, VstInt32 index)
{
  printf("called VSTPluginGetParameter(%d)\n", index);
  VSTPluginWrapper *wrapper = static_cast<VSTPluginWrapper *>(vstPlugin->object);
  return 0;
}


VSTPluginWrapper::VSTPluginWrapper(audioMasterCallback vstHostCallback) : _vstHostCallback(vstHostCallback)
{
  memset(&_vstPlugin, 0, sizeof(_vstPlugin));

  _vstPlugin.magic = kEffectMagic;
  _vstPlugin.object = this;

  _vstPlugin.flags = effFlagsCanReplacing | effFlagsCanDoubleReplacing;

  // callbacks
  _vstPlugin.dispatcher = VSTPluginDispatcher;
  _vstPlugin.getParameter = VSTPluginGetParameter;
  _vstPlugin.setParameter = VSTPluginSetParameter;
  _vstPlugin.processReplacing = VSTPluginProcessSamplesFloat32;
  _vstPlugin.processDoubleReplacing = VSTPluginProcessSamplesFloat64;

}

VSTPluginWrapper::~VSTPluginWrapper()
{
}

void VSTPluginWrapper::initPlugin(VstInt32 vendorUniqueID,
                                  VstInt32 vendorVersion,
                                  VstInt32 numParams,
                                  VstInt32 numPrograms,
                                  VstInt32 numInputs,
                                  VstInt32 numOutputs)
{
  _vstPlugin.uniqueID = vendorUniqueID;
  _vstPlugin.version = vendorVersion;
  _vstPlugin.numParams = numParams;
  _vstPlugin.numPrograms = numPrograms;
  _vstPlugin.numInputs = numInputs;
  _vstPlugin.numOutputs = numOutputs;
}

AEffect *VSTPluginMain(audioMasterCallback audioMaster)
{
  printf("called VSTPluginMain... \n");
  VSTPluginWrapper *plugin = new VSTPluginWrapper(audioMaster);
  plugin->initPlugin(CCONST('u', 's', 'a', 'n'), // registered with Steinberg (http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm)
                     PLUGIN_VERSION, // version
                     0,    // no params
                     0,    // no programs
                     2,    // 2 inputs
                     2);   // 2 outputs
  return plugin->getVSTPlugin();
}
