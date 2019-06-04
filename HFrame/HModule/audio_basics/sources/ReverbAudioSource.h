

//==============================================================================
/**
    An AudioSource that uses the Reverb class to apply a reverb to another AudioSource.

    @see Reverb

    @tags{Audio}
*/
class API ReverbAudioSource   : public AudioSource
{
public:
    /** Creates a ReverbAudioSource to process a given input source.

        @param inputSource              the input source to read from - this must not be null
        @param deleteInputWhenDeleted   if true, the input source will be deleted when
                                        this object is deleted
    */
    ReverbAudioSource (AudioSource* inputSource,
                       bool deleteInputWhenDeleted);

    /** Destructor. */
    ~ReverbAudioSource() override;

    //==============================================================================
    /** Returns the parameters from the reverb. */
    const Reverb::Parameters& getParameters() const noexcept    { return reverb.getParameters(); }

    /** Changes the reverb's parameters. */
    void setParameters (const Reverb::Parameters& newParams);

    void setBypassed (bool isBypassed) noexcept;
    bool isBypassed() const noexcept                            { return bypass; }

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock (const AudioSourceChannelInfo&) override;

private:
    //==============================================================================
    CriticalSection lock;
    OptionalScopedPointer<AudioSource> input;
    Reverb reverb;
    std::atomic<bool> bypass;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbAudioSource)
};


