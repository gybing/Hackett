

//==============================================================================
/**
    An AudioSource which takes some float audio data as an input.

    @tags{Audio}
*/
class API MemoryAudioSource   : public AudioSource
{
public:
    //==============================================================================
    /**  Creates a MemoryAudioSource by providing an audio buffer.

         If copyMemory is true then the buffer will be copied into an internal
         buffer which will be owned by the MemoryAudioSource. If copyMemory is
         false, then you must ensure that the lifetime of the audio buffer is
         at least as long as the MemoryAudioSource.
    */
    MemoryAudioSource (AudioBuffer<float>& audioBuffer, bool copyMemory, bool shouldLoop = false);

    //==============================================================================
    /** Implementation of the AudioSource method. */
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;

    /** Implementation of the AudioSource method. */
    void releaseResources() override;

    /** Implementation of the AudioSource method. */
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;

private:
    //==============================================================================
    AudioBuffer<float> buffer;
    int position = 0;
    bool isLooping;

    //==============================================================================
    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MemoryAudioSource)
};


