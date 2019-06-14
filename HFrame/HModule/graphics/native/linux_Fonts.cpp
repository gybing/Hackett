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

static std::unique_ptr<XmlElement> findFontsConfFile()
{
    static const char* pathsToSearch[] = { "/etc/fonts/fonts.conf",
                                           "/usr/share/fonts/fonts.conf" };

    for (auto* path : pathsToSearch)
        if (auto xml = parseXML (File (path)))
            return xml;

    return {};
}

StringArray FTTypefaceList::getDefaultFontDirectories()
{
    StringArray fontDirs;

    fontDirs.addTokens (String (CharPointer_UTF8 (getenv ("HFONT_PATH"))), ";,", "");
    fontDirs.removeEmptyStrings (true);

    if (fontDirs.empty())
    {
        if (auto fontsInfo = findFontsConfFile())
        {
            forEachXmlChildElementWithTagName (*fontsInfo, e, "dir")
            {
                auto fontPath = e->getAllSubText().trim();

                if (fontPath.!empty())
                {
                    if (e->getStringAttribute ("prefix") == "xdg")
                    {
                        auto xdgDataHome = SystemStats::getEnvironmentVariable ("XDG_DATA_HOME", {});

                        if (xdgDataHome.trimStart().empty())
                            xdgDataHome = "~/.local/share";

                        fontPath = File (xdgDataHome).getChildFile (fontPath).getFullPathName();
                    }

                    fontDirs.add (fontPath);
                }
            }
        }
    }

    if (fontDirs.empty())
        fontDirs.add ("/usr/X11R6/lib/X11/fonts");

    fontDirs.removeDuplicates (false);
    return fontDirs;
}

Typeface::Ptr Typeface::createSystemTypefaceFor (const Font& font)
{
    return new FreeTypeTypeface (font);
}

Typeface::Ptr Typeface::createSystemTypefaceFor (const void* data, size_t dataSize)
{
    return new FreeTypeTypeface (data, dataSize);
}

void Typeface::scanFolderForFonts (const File& folder)
{
    FTTypefaceList::getInstance()->scanFontPaths (StringArray (folder.getFullPathName()));
}

StringArray Font::findAllTypefaceNames()
{
    return FTTypefaceList::getInstance()->findAllFamilyNames();
}

StringArray Font::findAllTypefaceStyles (const String& family)
{
    return FTTypefaceList::getInstance()->findAllTypefaceStyles (family);
}

bool TextLayout::createNativeLayout (const AttributedString&)
{
    return false;
}

//==============================================================================
struct DefaultFontNames
{
    DefaultFontNames()
        : defaultSans  (getDefaultSansSerifFontName()),
          defaultSerif (getDefaultSerifFontName()),
          defaultFixed (getDefaultMonospacedFontName())
    {
    }

    String getRealFontName (const String& faceName) const
    {
        if (faceName == Font::getDefaultSansSerifFontName())    return defaultSans;
        if (faceName == Font::getDefaultSerifFontName())        return defaultSerif;
        if (faceName == Font::getDefaultMonospacedFontName())   return defaultFixed;

        return faceName;
    }

    String defaultSans, defaultSerif, defaultFixed;

private:
    static String pickBestFont (const StringArray& names, const char* const* choicesArray)
    {
        const StringArray choices (choicesArray);

        for (auto& choice : choices)
            if (names.contains (choice, true))
                return choice;

        for (auto& choice : choices)
            for (auto& name : names)
                if (name.startsWithIgnoreCase (choice))
                    return name;

        for (auto& choice : choices)
            for (auto& name : names)
                if (name.containsIgnoreCase (choice))
                    return name;

        return names[0];
    }

    static String getDefaultSansSerifFontName()
    {
        StringArray allFonts;
        FTTypefaceList::getInstance()->getSansSerifNames (allFonts);

        static const char* targets[] = { "Verdana", "Bitstream Vera Sans", "Luxi Sans",
                                         "Liberation Sans", "DejaVu Sans", "Sans", nullptr };
        return pickBestFont (allFonts, targets);
    }

    static String getDefaultSerifFontName()
    {
        StringArray allFonts;
        FTTypefaceList::getInstance()->getSerifNames (allFonts);

        static const char* targets[] = { "Bitstream Vera Serif", "Times", "Nimbus Roman",
                                         "Liberation Serif", "DejaVu Serif", "Serif", nullptr };
        return pickBestFont (allFonts, targets);
    }

    static String getDefaultMonospacedFontName()
    {
        StringArray allFonts;
        FTTypefaceList::getInstance()->getMonospacedNames (allFonts);

        static const char* targets[] = { "DejaVu Sans Mono", "Bitstream Vera Sans Mono", "Sans Mono",
                                         "Liberation Mono", "Courier", "DejaVu Mono", "Mono", nullptr };
        return pickBestFont (allFonts, targets);
    }

    HDECLARE_NON_COPYABLE (DefaultFontNames)
};

Typeface::Ptr Font::getDefaultTypefaceForFont (const Font& font)
{
    static DefaultFontNames defaultNames;

    Font f (font);
    f.setTypefaceName (defaultNames.getRealFontName (font.getTypefaceName()));
    return Typeface::createSystemTypefaceFor (f);
}


