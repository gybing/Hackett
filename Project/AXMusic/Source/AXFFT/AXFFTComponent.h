#ifndef __DROWAUDIODEMO_FFTDEMO_H__
#define __DROWAUDIODEMO_FFTDEMO_H__

#include "../JuceLibraryCode/JuceHeader.h"
#include "PitchDetectorComponent.h"

//==============================================================================
class AXFFTComponent : public Component,
                public Button::Listener,
                public Slider::Listener
{
public:
 	//==============================================================================
    AXFFTComponent();
    
    ~AXFFTComponent();
    
    void paint (Graphics& g);
    
    void resized();
    
    void buttonClicked (Button* button);
    
    void sliderValueChanged (Slider* slider);
    
	//==============================================================================
    void setSampleRate (double sampleRate);
    
    void processBlock (const float* inputChannelData, int numSamples);
    
private:
	//==============================================================================
    TimeSliceThread renderThread;
    PitchDetectorComponent pitchDetector;

	//==============================================================================
    ToggleButton logSpectroscopeButton, logSonogramButton;
    Slider sonogramSpeedSlider;
    
	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AXFFTComponent);
};

#endif //__DROWAUDIODEMO_FFTDEMO_H__