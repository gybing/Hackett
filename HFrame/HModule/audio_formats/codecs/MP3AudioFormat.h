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

#if HUSE_MP3AUDIOFORMAT || DOXYGEN

//==============================================================================
/**
    Software-based MP3 decoding format (doesn't currently provide an encoder).

    IMPORTANT DISCLAIMER: By choosing to enable the HUSE_MP3AUDIOFORMAT flag and
    to compile the MP3 code into your software, you do so AT YOUR OWN RISK! By doing so,
    you are agreeing that ROLI Ltd. is in no way responsible for any patent, copyright,
    or other legal issues that you may suffer as a result.

    The code in MP3AudioFormat.cpp is NOT guaranteed to be free from infringements of 3rd-party
    intellectual property. If you wish to use it, please seek your own independent advice about the
    legality of doing so. If you are not willing to accept full responsibility for the consequences
    of using this code, then do not enable the HUSE_MP3AUDIOFORMAT setting.

    @tags{Audio}
*/
class MP3AudioFormat  : public AudioFormat
{
public:
    //==============================================================================
    MP3AudioFormat();
    ~MP3AudioFormat();

    //==============================================================================
    Array<int> getPossibleSampleRates() override;
    Array<int> getPossibleBitDepths() override;
    bool canDoStereo() override;
    bool canDoMono() override;
    bool isCompressed() override;
    StringArray getQualityOptions() override;

    //==============================================================================
    AudioFormatReader* createReaderFor (InputStream*, bool deleteStreamIfOpeningFails) override;

    AudioFormatWriter* createWriterFor (OutputStream*, double sampleRateToUse,
                                        unsigned int numberOfChannels, int bitsPerSample,
                                        const StringPairArray& metadataValues, int qualityOptionIndex) override;
};

#endif


