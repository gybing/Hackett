/*
  ==============================================================================

   This file is part of the H library.
   Copyright (c) 2017 - ROLI Ltd.

   H is an open source library subject to commercial or open-source
   licensing.

   By using H, you agree to the terms of both the H 5 End-User License
   Agreement and H 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.H.com/H-5-licence
   Privacy Policy: www.H.com/H-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   H IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#pragma once


//==============================================================================
#define UNITY_AUDIO_PLUGIN_API_VERSION 0x010401

#if HWINDOWS
 #define UNITY_INTERFACE_API __stdcall
 #define UNITY_INTERFACE_EXPORT __declspec(dllexport)
#else
 #define UNITY_INTERFACE_API
 #define UNITY_INTERFACE_EXPORT __attribute__ ((visibility("default")))
#endif

//==============================================================================
struct UnityAudioEffectState;

typedef int  (UNITY_INTERFACE_API * createCallback)              (UnityAudioEffectState* state);
typedef int  (UNITY_INTERFACE_API * releaseCallback)             (UnityAudioEffectState* state);
typedef int  (UNITY_INTERFACE_API * resetCallback)               (UnityAudioEffectState* state);

typedef int  (UNITY_INTERFACE_API * processCallback)             (UnityAudioEffectState* state, float* inBuffer, float* outBuffer, unsigned int bufferSize,
                                                                  int numInChannels, int numOutChannels);

typedef int  (UNITY_INTERFACE_API * setPositionCallback)         (UnityAudioEffectState* state, unsigned int pos);

typedef int  (UNITY_INTERFACE_API * setFloatParameterCallback)   (UnityAudioEffectState* state, int index, float value);
typedef int  (UNITY_INTERFACE_API * getFloatParameterCallback)   (UnityAudioEffectState* state, int index, float* value, char* valuestr);
typedef int  (UNITY_INTERFACE_API * getFloatBufferCallback)      (UnityAudioEffectState* state, const char* name, float* buffer, int numsamples);

typedef int  (UNITY_INTERFACE_API * distanceAttenuationCallback) (UnityAudioEffectState* state, float distanceIn, float attenuationIn, float* attenuationOut);

typedef void (UNITY_INTERFACE_API * renderCallback)              (int eventId);

//==============================================================================
enum UnityAudioEffectDefinitionFlags
{
    isSideChainTarget = 1,
    isSpatializer = 2,
    isAmbisonicDecoder = 4,
    appliesDistanceAttenuation = 8
};

enum UnityAudioEffectStateFlags
{
    stateIsPlaying = 1,
    stateIsPaused = 2,
    stateIsMuted = 8,
    statIsSideChainTarget = 16
};

enum UnityEventModifiers
{
    shift = 1,
    control = 2,
    alt = 4,
    command = 8,
    numeric = 16,
    capsLock = 32,
    functionKey = 64
};

//==============================================================================
struct UnityAudioSpatializerData
{
    float                          listenerMatrix[16];
    float                          sourceMatrix[16];
    float                          spatialBlend;
    float                          reverbZoneMix;
    float                          spread;
    float                          stereoPan;
    distanceAttenuationCallback    attenuationCallback;
    float                          minDistance;
    float                          maxDistance;
};

struct UnityAudioAmbisonicData
{
    float                          listenerMatrix[16];
    float                          sourceMatrix[16];
    float                          spatialBlend;
    float                          reverbZoneMix;
    float                          spread;
    float                          stereoPan;
    distanceAttenuationCallback    attenuationCallback;
    int                            ambisonicOutChannels;
    float                          volume;
};

struct UnityAudioEffectState
{
    uint32               structSize;
    uint32               sampleRate;
    uint64               dspCurrentTick;
    uint64               dspPreviousTick;
    float*                     sidechainBuffer;
    void*                      effectData;
    uint32               flags;
    void*                      internal;

    UnityAudioSpatializerData* spatializerData;
    uint32               dspBufferSize;
    uint32               hostAPIVersion;

    UnityAudioAmbisonicData*   ambisonicData;

    template<typename T>
    inline T* getEffectData() const
    {
        HAssert (effectData != nullptr);
        HAssert (internal != nullptr);

        return (T*) effectData;
    }
};

struct UnityAudioParameterDefinition
{
    char        name[16];
    char        unit[16];
    const char* description;
    float       min;
    float       max;
    float       defaultVal;
    float       displayScale;
    float       displayExponent;
};

struct UnityAudioEffectDefinition
{
    uint32                   structSize;
    uint32                   parameterStructSize;
    uint32                   apiVersion;
    uint32                   pluginVersion;
    uint32                   channels;
    uint32                   numParameters;
    uint64                   flags;
    char                           name[32];
    createCallback                 create;
    releaseCallback                release;
    resetCallback                  reset;
    processCallback                process;
    setPositionCallback            setPosition;
    UnityAudioParameterDefinition* parameterDefintions;
    setFloatParameterCallback      setFloatParameter;
    getFloatParameterCallback      getFloatParameter;
    getFloatBufferCallback         getFloatBuffer;
};

//==============================================================================
// Unity callback
extern "C" UNITY_INTERFACE_EXPORT int  UNITY_INTERFACE_API UnityGetAudioEffectDefinitions (UnityAudioEffectDefinition*** definitionsPtr);

// GUI script callbacks
extern "C" UNITY_INTERFACE_EXPORT renderCallback UNITY_INTERFACE_API getRenderCallback();

extern "C" UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API unityInitialiseTexture (int id, void* textureHandle, int w, int h);

extern "C" UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API unityMouseDown (int id, float x, float y, UnityEventModifiers mods, int button);
extern "C" UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API unityMouseDrag (int id, float x, float y, UnityEventModifiers mods, int button);
extern "C" UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API unityMouseUp   (int id, float x, float y, UnityEventModifiers mods);

extern "C" UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API unityKeyEvent (int id, int code, UnityEventModifiers mods, const char* name);

extern "C" UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API unitySetScreenBounds (int id, float x, float y, float w, float h);
