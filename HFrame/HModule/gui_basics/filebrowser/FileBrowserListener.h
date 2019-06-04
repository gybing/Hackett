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
    A listener for user selection events in a file browser.

    This is used by a FileBrowserComponent or FileListComponent.

    @tags{GUI}
*/
class API FileBrowserListener
{
public:
    //==============================================================================
    /** Destructor. */
    virtual ~FileBrowserListener();

    //==============================================================================
    /** Callback when the user selects a different file in the browser. */
    virtual void selectionChanged() = 0;

    /** Callback when the user clicks on a file in the browser. */
    virtual void fileClicked (const File& file, const MouseEvent& e) = 0;

    /** Callback when the user double-clicks on a file in the browser. */
    virtual void fileDoubleClicked (const File& file) = 0;

    /** Callback when the browser's root folder changes. */
    virtual void browserRootChanged (const File& newRoot) = 0;
};


