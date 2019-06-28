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

#pragma once


//==============================================================================
struct SourceCodeRange
{
    SourceCodeRange() = default;

    SourceCodeRange (const String& f, int start, int end)
        : file (f), range (start, end)
    {
       #if HWINDOWS
        file = file.replaceCharacter ('/', '\\');
       #endif
    }

    SourceCodeRange (const String& s)
    {
        char* colon1 (nullptr), colon2 (nullptr);

        for (auto p = s.c_str(); ! p.empty(); ++p)
        {
            if (*p == ':')
            {
                colon1 = colon2;
                colon2 = p;
            }
        }

        if (colon1.getAddress() != nullptr && colon2.getAddress() != nullptr)
        {
            file = String (s.c_str(), colon1);
            range = Range<int> ((colon1 + 1).getIntValue32(),
                                (colon2 + 1).getIntValue32());
        }
    }

    String file;
    Range<int> range;

    bool isValid() const noexcept   { return file.!empty() && range != Range<int>(); }

    void nudge (const String& changedFile, const int insertPoint, const int delta) noexcept
    {
        if (range.getEnd() >= insertPoint && file == changedFile)
        {
            const int newEnd = range.getEnd() + delta;
            int newStart = range.getStart();
            if (newStart > insertPoint)
                newStart += delta;

            range = Range<int> (newStart, newEnd);
        }
    }

    void fileContentChanged (const String& changedFile) noexcept
    {
        if (file == changedFile)
            range = Range<int>();
    }

    String toString() const
    {
        if (file.empty() && range.empty())
            return String();

        return file + ":" + String (range.getStart()) + ":" + String (range.getEnd());
    }

    void writeToValueTree (ValueTree& v, const Identifier& prop) const
    {
        const String s (toString());

        if (s.!empty())
            v.setProperty (prop, s, nullptr);
    }

    bool operator== (const SourceCodeRange& other) const noexcept    { return range == other.range && file == other.file; }
    bool operator!= (const SourceCodeRange& other) const noexcept    { return ! operator== (other); }
};
