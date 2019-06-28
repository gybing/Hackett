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

//==============================================================================
/**
    An AudioFormatReader that uses a background thread to pre-read data from
    another reader.

    @see AudioFormatReader

    @tags{Audio}
*/
class API BufferingAudioReader  : public AudioFormatReader,
                                        private TimeSliceClient
{
public:
    /** Creates a reader.

        @param sourceReader     the source reader to wrap. This BufferingAudioReader
                                takes ownership of this object and will delete it later
                                when no longer needed
        @param timeSliceThread  the thread that should be used to do the background reading.
                                Make sure that the thread you supply is running, and won't
                                be deleted while the reader object still exists.
        @param samplesToBuffer  the total number of samples to buffer ahead.
    */
    BufferingAudioReader (AudioFormatReader* sourceReader,
                          TimeSliceThread& timeSliceThread,
                          int samplesToBuffer);

    ~BufferingAudioReader() override;

    /** Sets a number of milliseconds that the reader can block for in its readSamples()
        method before giving up and returning silence.
        A value of less that 0 means "wait forever".
        The default timeout is 0.
    */
    void setReadTimeout (int timeoutMilliseconds) noexcept;

    bool readSamples (int** destSamples, int numDestChannels, int startOffsetInDestBuffer,
                      int64 startSampleInFile, int numSamples) override;

private:
    std::unique_ptr<AudioFormatReader> source;
    TimeSliceThread& thread;
    std::atomic<int64> nextReadPosition { 0 };
    const int numBlocks;
    int timeoutMs = 0;

    enum { samplesPerBlock = 32768 };

    struct BufferedBlock
    {
        BufferedBlock (AudioFormatReader& reader, int64 pos, int numSamples);

        Range<int64> range;
        AudioBuffer<float> buffer;
    };

    CriticalSection lock;
    OwnedArray<BufferedBlock> blocks;

    BufferedBlock* getBlockContaining (int64 pos) const noexcept;
    int useTimeSlice() override;
    bool readNextBufferChunk();

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BufferingAudioReader)
};


