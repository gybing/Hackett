

//==============================================================================
/**
    A type of FileFilter that works by wildcard pattern matching.

    This filter only allows files that match one of the specified patterns, but
    allows all directories through.

    @see FileFilter, DirectoryContentsList, FileListComponent, FileBrowserComponent

    @tags{Core}
*/
class API WildcardFileFilter  : public FileFilter
{
public:
    //==============================================================================
    /**
        Creates a wildcard filter for one or more patterns.

        The wildcardPatterns parameter is a comma or semicolon-delimited set of
        patterns, e.g. "*.wav;*.aiff" would look for files ending in either .wav
        or .aiff.

        Passing an empty string as a pattern will fail to match anything, so by leaving
        either the file or directory pattern parameter empty means you can control
        whether files or directories are found.

        The description is a name to show the user in a list of possible patterns, so
        for the wav/aiff example, your description might be "audio files".
    */
    WildcardFileFilter (const String& fileWildcardPatterns,
                        const String& directoryWildcardPatterns,
                        const String& description);

    /** Destructor. */
    ~WildcardFileFilter() override;

    //==============================================================================
    /** Returns true if the filename matches one of the patterns specified. */
    bool isFileSuitable (const File& file) const override;

    /** This always returns true. */
    bool isDirectorySuitable (const File& file) const override;

private:
    //==============================================================================
    StringArray fileWildcards, directoryWildcards;

    HLEAK_DETECTOR (WildcardFileFilter)
};

