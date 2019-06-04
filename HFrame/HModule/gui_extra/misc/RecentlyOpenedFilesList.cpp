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

RecentlyOpenedFilesList::RecentlyOpenedFilesList()
    : maxNumberOfItems (10)
{
}

RecentlyOpenedFilesList::~RecentlyOpenedFilesList()
{
}

//==============================================================================
void RecentlyOpenedFilesList::setMaxNumberOfItems (const int newMaxNumber)
{
    maxNumberOfItems = jmax (1, newMaxNumber);

    files.removeRange (maxNumberOfItems, getNumFiles());
}

int RecentlyOpenedFilesList::getNumFiles() const
{
    return files.size();
}

File RecentlyOpenedFilesList::getFile (const int index) const
{
    return File (files [index]);
}

void RecentlyOpenedFilesList::clear()
{
    files.clear();
}

void RecentlyOpenedFilesList::addFile (const File& file)
{
    removeFile (file);
    files.insert (0, file.getFullPathName());

    setMaxNumberOfItems (maxNumberOfItems);
}

void RecentlyOpenedFilesList::removeFile (const File& file)
{
    files.removeString (file.getFullPathName());
}

void RecentlyOpenedFilesList::removeNonExistentFiles()
{
    for (int i = getNumFiles(); --i >= 0;)
        if (! getFile(i).exists())
            files.remove (i);
}

//==============================================================================
int RecentlyOpenedFilesList::createPopupMenuItems (PopupMenu& menuToAddTo,
                                                   const int baseItemId,
                                                   const bool showFullPaths,
                                                   const bool dontAddNonExistentFiles,
                                                   const File** filesToAvoid)
{
    int num = 0;

    for (int i = 0; i < getNumFiles(); ++i)
    {
        const File f (getFile(i));

        if ((! dontAddNonExistentFiles) || f.exists())
        {
            bool needsAvoiding = false;

            if (filesToAvoid != nullptr)
            {
                for (const File** avoid = filesToAvoid; *avoid != nullptr; ++avoid)
                {
                    if (f == **avoid)
                    {
                        needsAvoiding = true;
                        break;
                    }
                }
            }

            if (! needsAvoiding)
            {
                menuToAddTo.addItem (baseItemId + i,
                                     showFullPaths ? f.getFullPathName()
                                                   : f.getFileName());
                ++num;
            }
        }
    }

    return num;
}

//==============================================================================
String RecentlyOpenedFilesList::toString() const
{
    return files.joinIntoString ("\n");
}

void RecentlyOpenedFilesList::restoreFromString (const String& stringifiedVersion)
{
    clear();
    files.addLines (stringifiedVersion);

    setMaxNumberOfItems (maxNumberOfItems);
}


//==============================================================================
void RecentlyOpenedFilesList::registerRecentFileNatively (const File& file)
{
   #if HMAC
    HAUTORELEASEPOOL
    {
        [[NSDocumentController sharedDocumentController] noteNewRecentDocumentURL: createNSURLFromFile (file)];
    }
   #else
    ignoreUnused (file);
   #endif
}

void RecentlyOpenedFilesList::forgetRecentFileNatively (const File& file)
{
   #if HMAC
    HAUTORELEASEPOOL
    {
        // for some reason, OSX doesn't provide a method to just remove a single file
        // from the recent list, so we clear them all and add them back excluding
        // the specified file

        auto sharedDocController = [NSDocumentController sharedDocumentController];
        auto recentDocumentURLs  = [sharedDocController recentDocumentURLs];

        [sharedDocController clearRecentDocuments: nil];

        auto* nsFile = createNSURLFromFile (file);

        auto reverseEnumerator = [recentDocumentURLs reverseObjectEnumerator];

        for (NSURL* url : reverseEnumerator)
            if (! [url isEqual:nsFile])
                [sharedDocController noteNewRecentDocumentURL:url];
    }
   #else
    ignoreUnused (file);
   #endif
}

void RecentlyOpenedFilesList::clearRecentFilesNatively()
{
   #if HMAC
    HAUTORELEASEPOOL
    {
        [[NSDocumentController sharedDocumentController] clearRecentDocuments: nil];
    }
   #endif
}


