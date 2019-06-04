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

#if (HPLUGINHOST_VST3 && (HMAC || HWINDOWS)) || DOXYGEN

/**
    Implements a plugin format for VST3s.

    @tags{Audio}
*/
class API VST3PluginFormat   : public AudioPluginFormat
{
public:
    /** Constructor */
    VST3PluginFormat();

    /** Destructor */
    ~VST3PluginFormat() override;

    //==============================================================================
    /** Attempts to reload a VST3 plugin's state from some preset file data.

        @see VSTPluginFormat::loadFromFXBFile
    */
    static bool setStateFromVSTPresetFile (AudioPluginInstance*, const MemoryBlock&);

    //==============================================================================
    String getName() const override             { return "VST3"; }
    void findAllTypesForFile (OwnedArray<PluginDescription>&, const String& fileOrIdentifier) override;
    bool fileMightContainThisPluginType (const String& fileOrIdentifier) override;
    String getNameOfPluginFromIdentifier (const String& fileOrIdentifier) override;
    bool pluginNeedsRescanning (const PluginDescription&) override;
    StringArray searchPathsForPlugins (const FileSearchPath&, bool recursive, bool) override;
    bool doesPluginStillExist (const PluginDescription&) override;
    FileSearchPath getDefaultLocationsToSearch() override;
    bool canScanForPlugins() const override     { return true; }

private:
    void createPluginInstance (const PluginDescription&, double initialSampleRate,
                               int initialBufferSize, void* userData, PluginCreationCallback) override;

    bool requiresUnblockedMessageThreadDuringCreation (const PluginDescription&) const noexcept override;

private:
    //==============================================================================
    void recursiveFileSearch (StringArray&, const File&, bool recursive);

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VST3PluginFormat)
};

#endif // HPLUGINHOST_VST3


