#ifndef AX_MUSIC_H_INCLUDED
#define AX_MUSIC_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "../AXFFT/AXFFTComponent.h"

//==============================================================================
class AXMusicPlayer
    : public AudioIODeviceCallback
{
public:
    AXMusicPlayer();
    ~AXMusicPlayer();

    void playSound(const File& file);
    void playSound(InputStream& stream);

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AXMusicPlayer);
};


#endif // AX_MUSIC_H_INCLUDED