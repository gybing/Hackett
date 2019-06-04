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

#if HUSE_LAME_AUDIO_FORMAT || defined (DOXYGEN)

//==============================================================================
/**
    An AudioFormat class which can use an installed version of the LAME mp3
    encoder to encode a file.

    This format can't read MP3s, it just writes them. Internally, the
    AudioFormatWriter object that is returned writes the incoming audio data
    to a temporary WAV file, and then when the writer is deleted, it invokes
    the LAME executable to convert the data to an MP3, whose data is then
    piped into the original OutputStream that was used when first creating
    the writer.

    @see AudioFormat

    @tags{Audio}
*/
class API LAMEEncoderAudioFormat    : public AudioFormat
{
public:
    /** Creates a LAMEEncoderAudioFormat that expects to find a working LAME
        executable at the location given.
    */
    LAMEEncoderAudioFormat (const File& lameExecutableToUse);
    ~LAMEEncoderAudioFormat();

    bool canHandleFile (const File&);
    Array<int> getPossibleSampleRates();
    Array<int> getPossibleBitDepths();
    bool canDoStereo();
    bool canDoMono();
    bool isCompressed();
    StringArray getQualityOptions();

    AudioFormatReader* createReaderFor (InputStream*, bool deleteStreamIfOpeningFails);

    AudioFormatWriter* createWriterFor (OutputStream*, double sampleRateToUse,
                                        unsigned int numberOfChannels, int bitsPerSample,
                                        const StringPairArray& metadataValues, int qualityOptionIndex);

private:
    File lameApp;
    class Writer;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LAMEEncoderAudioFormat)
};

#endif


