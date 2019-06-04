

static void parseWildcard (const String& pattern, StringArray& result)
{
    result.addTokens (pattern.toLowerCase(), ";,", "\"'");
    result.trim();
    result.removeEmptyStrings();

    // special case for *.*, because people use it to mean "any file", but it
    // would actually ignore files with no extension.
    for (auto& r : result)
        if (r == "*.*")
            r = "*";
}

static bool matchWildcard (const File& file, const StringArray& wildcards)
{
    auto filename = file.getFileName();

    for (auto& w : wildcards)
        if (filename.matchesWildcard (w, true))
            return true;

    return false;
}

WildcardFileFilter::WildcardFileFilter (const String& fileWildcardPatterns,
                                        const String& directoryWildcardPatterns,
                                        const String& desc)
    : FileFilter (desc.isEmpty() ? fileWildcardPatterns
                                 : (desc + " (" + fileWildcardPatterns + ")"))
{
    parseWildcard (fileWildcardPatterns, fileWildcards);
    parseWildcard (directoryWildcardPatterns, directoryWildcards);
}

WildcardFileFilter::~WildcardFileFilter()
{
}

bool WildcardFileFilter::isFileSuitable (const File& file) const
{
    return matchWildcard (file, fileWildcards);
}

bool WildcardFileFilter::isDirectorySuitable (const File& file) const
{
    return matchWildcard (file, directoryWildcards);
}


