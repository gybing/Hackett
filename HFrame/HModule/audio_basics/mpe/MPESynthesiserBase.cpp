

MPESynthesiserBase::MPESynthesiserBase()
    : instrument (new MPEInstrument)
{
    instrument->addListener (this);
}

MPESynthesiserBase::MPESynthesiserBase (MPEInstrument* inst)
    : instrument (inst)
{
    HAssert (instrument != nullptr);
    instrument->addListener (this);
}

//==============================================================================
MPEZoneLayout MPESynthesiserBase::getZoneLayout() const noexcept
{
    return instrument->getZoneLayout();
}

void MPESynthesiserBase::setZoneLayout (MPEZoneLayout newLayout)
{
    instrument->setZoneLayout (newLayout);
}

//==============================================================================
void MPESynthesiserBase::enableLegacyMode (int pitchbendRange, Range<int> channelRange)
{
    instrument->enableLegacyMode (pitchbendRange, channelRange);
}

bool MPESynthesiserBase::isLegacyModeEnabled() const noexcept
{
    return instrument->isLegacyModeEnabled();
}

Range<int> MPESynthesiserBase::getLegacyModeChannelRange() const noexcept
{
    return instrument->getLegacyModeChannelRange();
}

void MPESynthesiserBase::setLegacyModeChannelRange (Range<int> channelRange)
{
    instrument->setLegacyModeChannelRange (channelRange);
}

int MPESynthesiserBase::getLegacyModePitchbendRange() const noexcept
{
    return instrument->getLegacyModePitchbendRange();
}

void MPESynthesiserBase::setLegacyModePitchbendRange (int pitchbendRange)
{
    instrument->setLegacyModePitchbendRange (pitchbendRange);
}

//==============================================================================
void MPESynthesiserBase::setPressureTrackingMode (TrackingMode modeToUse)
{
    instrument->setPressureTrackingMode (modeToUse);
}

void MPESynthesiserBase::setPitchbendTrackingMode (TrackingMode modeToUse)
{
    instrument->setPitchbendTrackingMode (modeToUse);
}

void MPESynthesiserBase::setTimbreTrackingMode (TrackingMode modeToUse)
{
    instrument->setTimbreTrackingMode (modeToUse);
}

//==============================================================================
void MPESynthesiserBase::handleMidiEvent (const MidiMessage& m)
{
    instrument->processNextMidiEvent (m);
}

//==============================================================================
template <typename floatType>
void MPESynthesiserBase::renderNextBlock (AudioBuffer<floatType>& outputAudio,
                                          const MidiBuffer& inputMidi,
                                          int startSample,
                                          int numSamples)
{
    // you must set the sample rate before using this!
    HAssert (sampleRate != 0);

    MidiBuffer::Iterator midiIterator (inputMidi);
    midiIterator.setNextSamplePosition (startSample);

    bool firstEvent = true;
    int midiEventPos;
    MidiMessage m;

    const ScopedLock sl (noteStateLock);

    while (numSamples > 0)
    {
        if (! midiIterator.getNextEvent (m, midiEventPos))
        {
            renderNextSubBlock (outputAudio, startSample, numSamples);
            return;
        }

        auto samplesToNextMidiMessage = midiEventPos - startSample;

        if (samplesToNextMidiMessage >= numSamples)
        {
            renderNextSubBlock (outputAudio, startSample, numSamples);
            handleMidiEvent (m);
            break;
        }

        if (samplesToNextMidiMessage < ((firstEvent && ! subBlockSubdivisionIsStrict) ? 1 : minimumSubBlockSize))
        {
            handleMidiEvent (m);
            continue;
        }

        firstEvent = false;

        renderNextSubBlock (outputAudio, startSample, samplesToNextMidiMessage);
        handleMidiEvent (m);
        startSample += samplesToNextMidiMessage;
        numSamples  -= samplesToNextMidiMessage;
    }

    while (midiIterator.getNextEvent (m, midiEventPos))
        handleMidiEvent (m);
}

// explicit instantiation for supported float types:
template void MPESynthesiserBase::renderNextBlock<float> (AudioBuffer<float>&, const MidiBuffer&, int, int);
template void MPESynthesiserBase::renderNextBlock<double> (AudioBuffer<double>&, const MidiBuffer&, int, int);

//==============================================================================
void MPESynthesiserBase::setCurrentPlaybackSampleRate (const double newRate)
{
    if (sampleRate != newRate)
    {
        const ScopedLock sl (noteStateLock);
        instrument->releaseAllNotes();
        sampleRate = newRate;
    }
}

//==============================================================================
void MPESynthesiserBase::setMinimumRenderingSubdivisionSize (int numSamples, bool shouldBeStrict) noexcept
{
    HAssert (numSamples > 0); // it wouldn't make much sense for this to be less than 1
    minimumSubBlockSize = numSamples;
    subBlockSubdivisionIsStrict = shouldBeStrict;
}


