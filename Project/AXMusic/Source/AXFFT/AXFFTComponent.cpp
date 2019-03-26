#include "AXFFTComponent.h"

AXFFTComponent::AXFFTComponent()
    : renderThread ("FFT Render Thread"),
      spectroscope (11),
      sonogram (11)
{
    pitchDetector.setSampleRate (44100.0);
    
    addAndMakeVisible (&audioOscilloscope);
    addAndMakeVisible (&spectroscope);
    addAndMakeVisible (&pitchDetector);
    addAndMakeVisible (&sonogram);
    
    renderThread.addTimeSliceClient (&spectroscope);
    renderThread.addTimeSliceClient (&sonogram);
    renderThread.startThread (3);
    
    addAndMakeVisible (&logSpectroscopeButton);
    addAndMakeVisible (&logSonogramButton);
    
    logSpectroscopeButton.setButtonText ("Log Frequency Scale");
    logSonogramButton.setButtonText ("Log Frequency Scale");

    logSpectroscopeButton.setClickingTogglesState (true);
    logSonogramButton.setClickingTogglesState (true);
    
    logSpectroscopeButton.addListener (this);
    logSonogramButton.addListener (this);
    
    addAndMakeVisible (&sonogramSpeedSlider);
    sonogramSpeedSlider.setRange (1.0, 10.0, 1.0);
    sonogramSpeedSlider.setValue (sonogram.getBlockWidth());
    sonogramSpeedSlider.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
    sonogramSpeedSlider.addListener (this);
}

AXFFTComponent::~AXFFTComponent()
{
    renderThread.removeTimeSliceClient (&spectroscope);
    renderThread.removeTimeSliceClient (&sonogram);
    renderThread.stopThread (500);

    logSpectroscopeButton.removeListener (this);
    logSonogramButton.removeListener (this);
    sonogramSpeedSlider.removeListener (this);
}

void AXFFTComponent::paint (Graphics& /*g*/)
{
}

void AXFFTComponent::resized()
{
    const int h = getHeight();
    const int w = getWidth();
    const int m = 5;
    const int ch = (h - (5 * m)) / 4;
    
    //audioOscilloscope.setBounds (m, m, w - (2 * m), ch);
    spectroscope.setBounds (m, ch + (2 * m), w - (2 * m), ch);
    //pitchDetector.setBounds (spectroscope.getBounds());
    //sonogram.setBounds (m, (2 * ch) + (3 * m), w - (2 * m), (2 * ch) + m);
    
    //logSpectroscopeButton.setBounds (spectroscope.getX(), spectroscope.getY(), 150, 18);
    //logSonogramButton.setBounds     (sonogram.getX(), sonogram.getY(), 150, 18);
    
    //sonogramSpeedSlider.setBounds   (logSonogramButton.getRight() + m, logSonogramButton.getY(),
     //                                100, 18);
}

void AXFFTComponent::buttonClicked (Button* button)
{
    if (button == &logSpectroscopeButton)
    {
        spectroscope.setLogFrequencyDisplay (logSpectroscopeButton.getToggleState());
        pitchDetector.setLogFrequencyDisplay (logSpectroscopeButton.getToggleState());
    }
    else if (button == &logSonogramButton)
    {
        sonogram.setLogFrequencyDisplay (logSonogramButton.getToggleState());
    }
}

void AXFFTComponent::sliderValueChanged (Slider* slider)
{
    if (slider == &sonogramSpeedSlider)
    {
        sonogram.setBlockWidth ((int) sonogramSpeedSlider.getValue());
    }
}

//==============================================================================
void AXFFTComponent::setSampleRate (double sampleRate)
{
    pitchDetector.setSampleRate (sampleRate);
}

void AXFFTComponent::processBlock (const float* inputChannelData, int numSamples)
{
    if (inputChannelData != nullptr)
    {
        audioOscilloscope.processBlock (inputChannelData, numSamples);
        pitchDetector.processBlock (inputChannelData, numSamples);
        spectroscope.copySamples (inputChannelData, numSamples);
        sonogram.copySamples (inputChannelData, numSamples);
    }
}
