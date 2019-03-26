#pragma once
#include "AltQDefine.h"
#include "AltQBase.h"
#include "AltQModuleRadioDefine.h"
using namespace juce;

//==============================================================================
class AltQRadioPlayer
    : public AudioIODeviceCallback
{
public:
    AltQRadioPlayer();
    ~AltQRadioPlayer();

    void play(InputStream& stream);
    void pause();
    void resume();
    void stop();

protected:
    // AudioIODeviceCallback
    virtual void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels, float** outputChannelData, int numOutputChannels, int numSamples) override;
    virtual void audioDeviceAboutToStart(AudioIODevice* device) override;
    virtual void audioDeviceStopped() override;
    virtual void audioDeviceError(const String& errorMessage) override;

public:
    TimeSliceThread timeSliceThread;
    ScopedPointer<AudioFormatReaderSource> lastReaderSource;
    AudioTransportSource audioTransportSource;
    AudioSourcePlayer audioSourcePlayer;
    AudioDeviceManager audioDeviceManager;
    AudioFormatManager audioFormatManager;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AltQRadioPlayer);
};
