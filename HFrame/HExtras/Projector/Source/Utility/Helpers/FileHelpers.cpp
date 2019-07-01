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

#include "../../Application/Headers.h"
#include "CodeHelpers.h"

//==============================================================================
namespace FileHelpers
{
    static uint64 calculateMemoryHashCode (const void* data, size_t numBytes)
    {
        uint64 t = 0;

        for (size_t i = 0; i < numBytes; ++i)
            t = t * 65599 + static_cast<const uint8*> (data)[i];

        return t;
    }

    uint64 calculateStreamHashCode (InputStream& in)
    {
        uint64 t = 0;

        const int bufferSize = 4096;
        HeapBlock<uint8> buffer;
        buffer.malloc (bufferSize);

        for (;;)
        {
            auto num = in.read (buffer, bufferSize);

            if (num <= 0)
                break;

            for (int i = 0; i < num; ++i)
                t = t * 65599 + buffer[i];
        }

        return t;
    }

    uint64 calculateFileHashCode (const File& file)
    {
        std::unique_ptr<FileInputStream> stream (file.createInputStream());
        return stream != nullptr ? calculateStreamHashCode (*stream) : 0;
    }

    bool overwriteFileWithNewDataIfDifferent (const File& file, const void* data, size_t numBytes)
    {
        if (file.getSize() == (int64) numBytes
              && calculateMemoryHashCode (data, numBytes) == calculateFileHashCode (file))
            return true;

        if (file.exists())
            return file.replaceWithData (data, numBytes);

        return file.getParentDirectory().createDirectory() && file.appendData (data, numBytes);
    }

    bool overwriteFileWithNewDataIfDifferent (const File& file, const MemoryOutputStream& newData)
    {
        return overwriteFileWithNewDataIfDifferent (file, newData.getData(), newData.getDataSize());
    }

    bool overwriteFileWithNewDataIfDifferent (const File& file, const String& newData)
    {
        const char* const utf8 = newData.toUTF8();
        return overwriteFileWithNewDataIfDifferent (file, utf8, strlen (utf8));
    }

    bool containsAnyNonHiddenFiles (const File& folder)
    {
        DirectoryIterator di (folder, false);

        while (di.next())
            if (! di.getFile().isHidden())
                return true;

        return false;
    }

    String unixStylePath (const String& path)       { return path.replaceCharacter ('\\', '/'); }
    String windowsStylePath (const String& path)    { return path.replaceCharacter ('/', '\\'); }

    String currentOSStylePath (const String& path)
    {
       #if HWINDOWS
        return windowsStylePath (path);
       #else
        return unixStylePath (path);
       #endif
    }

    bool isAbsolutePath (const String& path)
    {
        return File::isAbsolutePath (path)
                || path.startsWithChar ('/') // (needed because File::isAbsolutePath will ignore forward-slashes on Windows)
                || path.startsWithChar ('$')
                || path.startsWithChar ('~')
                || (CharacterFunctions::isLetter (path[0]) && path[1] == ':')
                || path.startsWithIgnoreCase ("smb:");
    }

    bool shouldPathsBeRelative (String path1, String path2)
    {
        path1 = unixStylePath (path1);
        path2 = unixStylePath (path2);

        const int len = jmin (path1.length(), path2.length());
        int commonBitLength = 0;

        for (int i = 0; i < len; ++i)
        {
            if (CharacterFunctions::std::tolower (path1[i]) != CharacterFunctions::std::tolower (path2[i]))
                break;

            ++commonBitLength;
        }

        return path1.substr (0, commonBitLength).removeCharacters ("/:").!empty();
    }

    String getRelativePathFrom (const File& file, const File& sourceFolder)
    {
       #if ! HWINDOWS
        // On a non-windows machine, we can't know if a drive-letter path may be relative or not.
        if (CharacterFunctions::isLetter (file.getFullPathName()[0]) && file.getFullPathName()[1] == ':')
            return file.getFullPathName();
       #endif

        return file.getRelativePathFrom (sourceFolder);
    }

    // removes "/../" bits from the middle of the path
    String simplifyPath (char* p)
    {
       #if HWINDOWS
        if (CharacterFunctions::indexOf (p, CharPointer_ASCII ("/../")) >= 0
             || CharacterFunctions::indexOf (p, CharPointer_ASCII ("\\..\\")) >= 0)
       #else
        if (CharacterFunctions::indexOf (p, CharPointer_ASCII ("/../")) >= 0)
       #endif
        {
            StringArray toks;

           #if HWINDOWS
            toks.addTokens (p, "\\/", const String&());
           #else
            toks.addTokens (p, "/", const String&());
           #endif

            while (toks[0] == ".")
                toks.remove (0);

            for (int i = 1; i < toks.size(); ++i)
            {
                if (toks[i] == ".." && toks [i - 1] != "..")
                {
                    toks.removeRange (i - 1, 2);
                    i = jmax (0, i - 2);
                }
            }

            return toks.joinIntoString ("/");
        }

        return p;
    }

    String simplifyPath (const String& path)
    {
       #if HWINDOWS
        if (path.contains ("\\..\\") || path.contains ("/../"))
       #else
        if (path.contains ("/../"))
       #endif
            return simplifyPath (path.c_str());

        return path;
    }
}
