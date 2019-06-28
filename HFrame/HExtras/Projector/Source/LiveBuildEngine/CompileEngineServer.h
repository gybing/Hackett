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
// These functions are called by our child process on startup, to launch
// the compilation server

String createCommandLineForLaunchingServer (const String& pipeName,
                                            const String& projectUID,
                                            const File& cacheLocation);

void* createClangServer (const String& commandLine);
void destroyClangServer (void*);

// Called if our child process is asked to shutdown by the user, so it can pass
// that shutdown event up to the parent (IDE) process..
void sendQuitMessageToIDE (void*);
