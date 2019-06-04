

MemoryAudioSource::MemoryAudioSource (AudioBuffer<float>& bufferToUse, bool copyMemory, bool shouldLoop)
    : isLooping (shouldLoop)
{
    if (copyMemory)
        buffer.makeCopyOf (bufferToUse);
    else
        buffer.setDataToReferTo (bufferToUse.getArrayOfWritePointers(),
                                 bufferToUse.getNumChannels(),
                                 bufferToUse.getNumSamples());
}

//==============================================================================
void MemoryAudioSource::prepareToPlay (int /*samplesPerBlockExpected*/, double /*sampleRate*/)
{
    position = 0;
}

void MemoryAudioSource::releaseResources()   {}

void MemoryAudioSource::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    auto& dst = *bufferToFill.buffer;
    auto channels = jmin (dst.getNumChannels(), buffer.getNumChannels());
    auto max = 0, pos = 0;
    auto n = buffer.getNumSamples(), m = bufferToFill.numSamples;

    int i;
    for (i = position; (i < n || isLooping) && (pos < m); i += max)
    {
        max = jmin (m - pos, n - (i % n));

        int ch = 0;
        for (; ch < channels; ++ch)
            dst.copyFrom (ch, bufferToFill.startSample + pos, buffer, ch, i % n, max);

        for (; ch < dst.getNumChannels(); ++ch)
            dst.clear (ch, bufferToFill.startSample + pos, max);

        pos += max;
    }

    if (pos < m)
        dst.clear (bufferToFill.startSample + pos, m - pos);

    position = (i % n);
}


