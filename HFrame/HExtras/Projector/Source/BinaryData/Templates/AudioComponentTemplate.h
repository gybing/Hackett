/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

%%include_H%%

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class %%content_component_class%%   : public AudioAppComponent
{
public:
    //==============================================================================
    %%content_component_class%%();
    ~%%content_component_class%%();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...


    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (%%content_component_class%%)
};
