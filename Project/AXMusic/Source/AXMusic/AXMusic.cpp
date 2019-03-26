#include "AXMusic.h"

//==============================================================================
AXMusicPlayer::AXMusicPlayer()
    : timeSliceThread("slice_thread")
{
    audioFormatManager.registerBasicFormats();
    audioSourcePlayer.setSource(&audioTransportSource);
    audioDeviceManager.initialise(0, 2, nullptr, true);
    audioDeviceManager.addAudioCallback(this);
    timeSliceThread.startThread();
}

AXMusicPlayer::~AXMusicPlayer()
{
}

void AXMusicPlayer::audioDeviceIOCallback(const float** inputChannelData, int numInputChannels, float** outputChannelData, int numOutputChannels, int numSamples)
{
    audioSourcePlayer.audioDeviceIOCallback(inputChannelData, numInputChannels, outputChannelData, numOutputChannels, numSamples);
}

void AXMusicPlayer::audioDeviceAboutToStart(AudioIODevice* device)
{
    audioSourcePlayer.audioDeviceAboutToStart(device);
}

void AXMusicPlayer::audioDeviceStopped()
{
    audioSourcePlayer.audioDeviceStopped();
}

void AXMusicPlayer::audioDeviceError(const String& errorMessage)
{
}

void AXMusicPlayer::playSound(const File& file)
{

}

void AXMusicPlayer::playSound(InputStream& stream)
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
