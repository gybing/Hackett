

//==============================================================================
/**
    Collects incoming realtime MIDI messages and turns them into blocks suitable for
    processing by a block-based audio callback.

    The class can also be used as either a MidiKeyboardStateListener or a MidiInputCallback
    so it can easily use a midi input or keyboard component as its source.

    @see MidiMessage, MidiInput

    @tags{Audio}
*/
class API MidiMessageCollector    : public MidiKeyboardStateListener,
                                          public MidiInputCallback
{
public:
    //==============================================================================
    /** Creates a MidiMessageCollector. */
    MidiMessageCollector();

    /** Destructor. */
    ~MidiMessageCollector() override;

    //==============================================================================
    /** Clears any messages from the queue.

        You need to call this method before starting to use the collector, so that
        it knows the correct sample rate to use.
    */
    void reset (double sampleRate);

    /** Takes an incoming real-time message and adds it to the queue.

        The message's timestamp is taken, and it will be ready for retrieval as part
        of the block returned by the next call to removeNextBlockOfMessages().

        This method is fully thread-safe when overlapping calls are made with
        removeNextBlockOfMessages().
    */
    void addMessageToQueue (const MidiMessage& message);

    /** Removes all the pending messages from the queue as a buffer.

        This will also correct the messages' timestamps to make sure they're in
        the range 0 to numSamples - 1.

        This call should be made regularly by something like an audio processing
        callback, because the time that it happens is used in calculating the
        midi event positions.

        This method is fully thread-safe when overlapping calls are made with
        addMessageToQueue().

        Precondition: numSamples must be greater than 0.
    */
    void removeNextBlockOfMessages (MidiBuffer& destBuffer, int numSamples);


    //==============================================================================
    /** @internal */
    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    /** @internal */
    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    /** @internal */
    void handleIncomingMidiMessage (MidiInput*, const MidiMessage&) override;

private:
    //==============================================================================
    double lastCallbackTime = 0;
    CriticalSection midiCallbackLock;
    MidiBuffer incomingMessages;
    double sampleRate = 44100.0;
   #if HDEBUG
    bool hasCalledReset = false;
   #endif

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiMessageCollector)
};


