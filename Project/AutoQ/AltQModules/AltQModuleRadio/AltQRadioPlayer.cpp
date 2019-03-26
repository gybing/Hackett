#include "AltQRadioPlayer.h"

//==============================================================================
AltQRadioPlayer::AltQRadioPlayer()
    : timeSliceThread("slice_thread")
{
    audioFormatManager.registerBasicFormats();
    audioSourcePlayer.setSource(&audioTransportSource);
    audioDeviceManager.initialise(0, 2, nullptr, true);
    audioDeviceManager.addAudioCallback(this);
    timeSliceThread.startThread();
}

AltQRadioPlayer::~AltQRadioPlayer()
{
}

void AltQRadioPlayer::audioDeviceIOCallback(const float** inputChannelData, int numInputChannels, float** outputChannelData, int numOutputChannels, int numSamples)
{
    audioSourcePlayer.audioDeviceIOCallback(inputChannelData, numInputChannels, outputChannelData, numOutputChannels, numSamples);
}

void AltQRadioPlayer::audioDeviceAboutToStart(AudioIODevice* device)
{
    audioSourcePlayer.audioDeviceAboutToStart(device);
}

void AltQRadioPlayer::audioDeviceStopped()
{
    audioSourcePlayer.audioDeviceStopped();
}

void AltQRadioPlayer::audioDeviceError(const String& errorMessage)
{
}

void AltQRadioPlayer::play(InputStream& stream)
{
    if (AudioFormatReader* formatReader = audioFormatManager.createReaderFor(&stream))
    {
        AudioFormatReaderSource* formatReaderSource = new AudioFormatReaderSource(formatReader, true);
        if (formatReaderSource)
        {
            audioTransportSource.setSource(formatReaderSource, 65535 * 4, &timeSliceThread, formatReader->sampleRate);
            audioTransportSource.start();
        }
        lastReaderSource = formatReaderSource;
    }
 }

void AltQRadioPlayer::pause()
{
    if (audioTransportSource.isPlaying())
    {
        audioTransportSource.stop();
    }
}

void AltQRadioPlayer::resume()
{
    if (audioTransportSource.isPlaying())
    {
        audioTransportSource.start();
    }
}

void AltQRadioPlayer::stop()
{
    pause();
    audioTransportSource.setSource(nullptr);
}
