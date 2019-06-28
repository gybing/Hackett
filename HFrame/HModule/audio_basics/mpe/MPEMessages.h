

//==============================================================================
/**
    This helper class contains the necessary helper functions to generate
    MIDI messages that are exclusive to MPE, such as defining the upper and lower
    MPE zones and setting per-note and master pitchbend ranges.
    You can then send them to your MPE device using MidiOutput::sendBlockOfMessagesNow.

    All other MPE messages like per-note pitchbend, pressure, and third
    dimension, are ordinary MIDI messages that should be created using the MidiMessage
    class instead. You just need to take care to send them to the appropriate
    per-note MIDI channel.

    Note: If you are working with an MPEZoneLayout object inside your app,
    you should not use the message sequences provided here. Instead, you should
    change the zone layout programmatically with the member functions provided in the
    MPEZoneLayout class itself. You should also make sure that the Expressive
    MIDI zone layout of your C++ code and of the MPE device are kept in sync.

    @see MidiMessage, MPEZoneLayout

    @tags{Audio}
*/
class API MPEMessages
{
public:
    /** Returns the sequence of MIDI messages that, if sent to an Expressive
        MIDI device, will set the lower MPE zone.
    */
    static MidiBuffer setLowerZone (int numMemberChannels = 0,
                                    int perNotePitchbendRange = 48,
                                    int masterPitchbendRange = 2);

    /** Returns the sequence of MIDI messages that, if sent to an Expressive
        MIDI device, will set the upper MPE zone.
    */
    static MidiBuffer setUpperZone (int numMemberChannels = 0,
                                    int perNotePitchbendRange = 48,
                                    int masterPitchbendRange = 2);

    /** Returns the sequence of MIDI messages that, if sent to an Expressive
        MIDI device, will set the per-note pitchbend range of the lower MPE zone.
    */
    static MidiBuffer setLowerZonePerNotePitchbendRange (int perNotePitchbendRange = 48);

    /** Returns the sequence of MIDI messages that, if sent to an Expressive
        MIDI device, will set the per-note pitchbend range of the upper MPE zone.
    */
    static MidiBuffer setUpperZonePerNotePitchbendRange (int perNotePitchbendRange = 48);

    /** Returns the sequence of MIDI messages that, if sent to an Expressive
        MIDI device, will set the master pitchbend range of the lower MPE zone.
    */
    static MidiBuffer setLowerZoneMasterPitchbendRange (int masterPitchbendRange = 2);

    /** Returns the sequence of MIDI messages that, if sent to an Expressive
        MIDI device, will set the master pitchbend range of the upper MPE zone.
    */
    static MidiBuffer setUpperZoneMasterPitchbendRange (int masterPitchbendRange = 2);

    /** Returns the sequence of MIDI messages that, if sent to an Expressive
        MIDI device, will clear the lower zone.
    */
    static MidiBuffer clearLowerZone();

    /** Returns the sequence of MIDI messages that, if sent to an Expressive
        MIDI device, will clear the upper zone.
    */
    static MidiBuffer clearUpperZone();

    /** Returns the sequence of MIDI messages that, if sent to an Expressive
        MIDI device, will clear the lower and upper zones.
    */
    static MidiBuffer clearAllZones();

    /** Returns the sequence of MIDI messages that, if sent to an Expressive
        MIDI device, will reset the whole MPE zone layout of the
        device to the laoyut passed in. This will first clear the current lower and upper
        zones, then then set the zones contained in the passed-in zone layout, and set their
        per-note and master pitchbend ranges to their current values.
    */
    static MidiBuffer setZoneLayout (MPEZoneLayout layout);

    /** The RPN number used for MPE zone layout messages.

        Pitchbend range messages (both per-note and master) are instead sent
        on RPN 0 as in standard MIDI 1.0.
    */
    static const int zoneLayoutMessagesRpnNumber = 6;
};


