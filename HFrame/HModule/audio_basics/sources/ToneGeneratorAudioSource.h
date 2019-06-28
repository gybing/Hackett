

//==============================================================================
/**
    A simple AudioSource that generates a sine wave.


    @tags{Audio}
*/
class API ToneGeneratorAudioSource  : public AudioSource
{
public:
    //==============================================================================
    /** Creates a ToneGeneratorAudioSource. */
    ToneGeneratorAudioSource();

    /** Destructor. */
    ~ToneGeneratorAudioSource() override;

    //==============================================================================
    /** Sets the signal's amplitude. */
    void setAmplitude (float newAmplitude);

    /** Sets the signal's frequency. */
    void setFrequency (double newFrequencyHz);


    //==============================================================================
    /** Implementation of the AudioSource method. */
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;

    /** Implementation of the AudioSource method. */
    void releaseResources() override;

    /** Implementation of the AudioSource method. */
    void getNextAudioBlock (const AudioSourceChannelInfo&) override;


private:
    //==============================================================================
    double frequency, sampleRate;
    double currentPhase, phasePerSample;
    float amplitude;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToneGeneratorAudioSource)
};


