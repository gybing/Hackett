/*
  ==============================================================================

   This file is part of the H library.
   Copyright (c) 2017 - ROLI Ltd.

   H is an open source library subject to commercial or open-source
   licensing.

   By using H, you agree to the terms of both the H 5 End-User License
   Agreement and H 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.H.com/H-5-licence
   Privacy Policy: www.H.com/H-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   H IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#if HUSE_CDREADER || DOXYGEN


//==============================================================================
/**
    A type of AudioFormatReader that reads from an audio CD.

    One of these can be used to read a CD as if it's one big audio stream. Use the
    getPositionOfTrackStart() method to find where the individual tracks are
    within the stream.

    @see AudioFormatReader

    @tags{Audio}
*/
class API AudioCDReader  : public AudioFormatReader
{
public:
    //==============================================================================
    /** Returns a list of names of Audio CDs currently available for reading.

        If there's a CD drive but no CD in it, this might return an empty list, or
        possibly a device that can be opened but which has no tracks, depending
        on the platform.

        @see createReaderForCD
    */
    static StringArray getAvailableCDNames();

    /** Tries to create an AudioFormatReader that can read from an Audio CD.

        @param index    the index of one of the available CDs - use getAvailableCDNames()
                        to find out how many there are.
        @returns        a new AudioCDReader object, or nullptr if it couldn't be created. The
                        caller will be responsible for deleting the object returned.
    */
    static AudioCDReader* createReaderForCD (const int index);

    //==============================================================================
    /** Destructor. */
    ~AudioCDReader();

    /** Implementation of the AudioFormatReader method. */
    bool readSamples (int** destSamples, int numDestChannels, int startOffsetInDestBuffer,
                      int64 startSampleInFile, int numSamples) override;

    /** Checks whether the CD has been removed from the drive. */
    bool isCDStillPresent() const;

    /** Returns the total number of tracks (audio + data). */
    int getNumTracks() const;

    /** Finds the sample offset of the start of a track.
        @param trackNum     the track number, where trackNum = 0 is the first track
                            and trackNum = getNumTracks() means the end of the CD.
    */
    int getPositionOfTrackStart (int trackNum) const;

    /** Returns true if a given track is an audio track.
        @param trackNum     the track number, where 0 is the first track.
    */
    bool isTrackAudio (int trackNum) const;

    /** Returns an array of sample offsets for the start of each track, followed by
        the sample position of the end of the CD.
    */
    const Array<int>& getTrackOffsets() const;

    /** Refreshes the object's table of contents.

        If the disc has been ejected and a different one put in since this
        object was created, this will cause it to update its idea of how many tracks
        there are, etc.
    */
    void refreshTrackLengths();

    /** Enables scanning for indexes within tracks.
        @see getLastIndex
    */
    void enableIndexScanning (bool enabled);

    /** Returns the index number found during the last read() call.

        Index scanning is turned off by default - turn it on with enableIndexScanning().

        Then when the read() method is called, if it comes across an index within that
        block, the index number is stored and returned by this method.

        Some devices might not support indexes, of course.

        (If you don't know what CD indexes are, it's unlikely you'll ever need them).

        @see enableIndexScanning
    */
    int getLastIndex() const;

    /** Scans a track to find the position of any indexes within it.
        @param trackNumber  the track to look in, where 0 is the first track on the disc
        @returns    an array of sample positions of any index points found (not including
                    the index that marks the start of the track)
    */
    Array<int> findIndexesInTrack (const int trackNumber);

    /** Returns the CDDB id number for the CD.
        It's not a great way of identifying a disc, but it's traditional.
    */
    int getCDDBId();

    /** Tries to eject the disk.
        Ejecting the disk might not actually be possible, e.g. if some other process is using it.
    */
    void ejectDisk();

    //==============================================================================
    enum
    {
        framesPerSecond = 75,
        samplesPerFrame = 44100 / framesPerSecond
    };

private:
    //==============================================================================
    Array<int> trackStartSamples;

   #if HMAC
    File volumeDir;
    Array<File> tracks;
    int currentReaderTrack;
    std::unique_ptr<AudioFormatReader> reader;
    AudioCDReader (const File& volume);

   #elif HWINDOWS
    bool audioTracks [100];
    void* handle;
    MemoryBlock buffer;
    bool indexingEnabled;
    int lastIndex, firstFrameInBuffer, samplesInBuffer;
    AudioCDReader (void* handle);
    int getIndexAt (int samplePos);

   #elif HLINUX
    AudioCDReader();
   #endif

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioCDReader)
};

#endif


