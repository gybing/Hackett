

//==============================================================================
/**
    Contains functions to control the system's master volume.

    @tags{Audio}
*/
class API SystemAudioVolume
{
public:
    //==============================================================================
    /** Returns the operating system's current volume level in the range 0 to 1.0 */
    static float CALLTYPE getGain();

    /** Attempts to set the operating system's current volume level.
        @param newGain  the level, between 0 and 1.0
        @returns true if the operation succeeds
    */
    static bool CALLTYPE setGain (float newGain);

    /** Returns true if the system's audio output is currently muted. */
    static bool CALLTYPE isMuted();

    /** Attempts to mute the operating system's audio output.
        @param shouldBeMuted    true if you want it to be muted
        @returns true if the operation succeeds
    */
    static bool CALLTYPE setMuted (bool shouldBeMuted);

private:
    SystemAudioVolume(); // Don't instantiate this class, just call its static fns.
    HDECLARE_NON_COPYABLE (SystemAudioVolume)
};


