#ifndef __PITCHDETECTORCOMPONENT_H_FCCBFF40__
#define __PITCHDETECTORCOMPONENT_H_FCCBFF40__

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class PitchDetectorComponent :  public Component,
                                public Timer
{
public:
	//==============================================================================
    PitchDetectorComponent();

    ~PitchDetectorComponent();

    void setLogFrequencyDisplay (bool shouldDisplayLogFrequency);
    
    void paint (Graphics&);
    
    void resized();

    void timerCallback();
    
	//==============================================================================
    void setSampleRate (double newSampleRate);
    
    void processBlock (const float* inputChannelData, int numSamples);

private:
	//==============================================================================
    bool displayLogFrequency;
    double sampleRate, pitch;
    AudioSampleBuffer sampleBuffer;
    String pitchString;
    Label pitchLabel;
    
    CriticalSection detectorLock;
    
	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchDetectorComponent)
};


#endif  // __PITCHDETECTORCOMPONENT_H_FCCBFF40__
