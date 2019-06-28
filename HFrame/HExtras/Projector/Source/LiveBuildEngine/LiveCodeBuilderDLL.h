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


extern "C"
{
    typedef void* LiveCodeBuilder;
    typedef bool (*SendMessageFunction) (void* userInfo, const void* data, size_t dataSize);
    typedef void (*CrashCallbackFunction) (const char* crashDescription);
    typedef void (*QuitCallbackFunction)();
    typedef void (*SetPropertyFunction) (const char* key, const char* value);
    typedef void (*GetPropertyFunction) (const char* key, char* value, size_t size);

    // We've used an X macro to define the DLL functions rather than just declaring them, so that
    // we can load the DLL and its functions dynamically and cope with it not being there.
    // The CompileEngineDLL class is a wrapper that manages finding/loading the DLL and exposing
    // these as callable functions.
    #define LIVE_DLL_FUNCTIONS(X) \
        X (progetVersion,     int, ()) \
        X (proinitialise,     void, (CrashCallbackFunction, QuitCallbackFunction, SetPropertyFunction, GetPropertyFunction, bool setupSignals)) \
        X (proshutdown,       void, ()) \
        X (procreateBuilder,  LiveCodeBuilder, (SendMessageFunction, void* userInfo, const char* projectID, const char* cacheFolder)) \
        X (prosendMessage,    void, (LiveCodeBuilder, const void* messageData, size_t messageDataSize)) \
        X (prodeleteBuilder,  void, (LiveCodeBuilder))

}
