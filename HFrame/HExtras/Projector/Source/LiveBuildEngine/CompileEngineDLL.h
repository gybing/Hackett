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

#include "LiveCodeBuilderDLL.h"

//==============================================================================
struct CompileEngineDLL  : private DeletedAtShutdown
{
    CompileEngineDLL()
    {
        tryLoadDll();
    }

    ~CompileEngineDLL()
    {
        shutdown();
        clearSingletonInstance();
    }

    bool tryLoadDll()
    {
        // never load the dynamic lib multiple times
        if (! isLoaded())
        {
            auto f = findDLLFile();

            if (f != File() && dll.open (f.getLinkedTarget().getFullPathName()))
            {
               #define INIT_LIVE_DLL_FN(name, returnType, params)    name = (name##_type) dll.getFunction (#name);
                LIVE_DLL_FUNCTIONS (INIT_LIVE_DLL_FN);
               #undef INIT_LIVE_DLL_FN

                return true;
            }

            return false;
        }

        return true;
    }

    void initialise (CrashCallbackFunction crashFn, QuitCallbackFunction quitFn, bool setupSignals)
    {
        if (isLoaded())
            proinitialise (crashFn, quitFn, setPropertyCallback, getPropertyCallback, setupSignals);
    }

    void shutdown()
    {
        if (isLoaded())
            proshutdown();
    }

    bool isLoaded() const
    {
        #define CHECK_LIVE_DLL_FN(name, returnType, params)    if (name == nullptr) return false;
        LIVE_DLL_FUNCTIONS (CHECK_LIVE_DLL_FN);
        #undef CHECK_LIVE_DLL_FN

        return progetVersion() == requiredVersion;
    }

    #define DECLARE_LIVE_DLL_FN(name, returnType, params) \
        typedef returnType (*name##_type) params; \
        name##_type name = nullptr;

    LIVE_DLL_FUNCTIONS (DECLARE_LIVE_DLL_FN)

    #undef DECLARE_LIVE_DLL_FN

    static String getDLLName()
    {
       #if HMAC
        return "HCompileEngine.dylib";
       #elif HLINUX
        return "HCompileEngine.so";
       #elif HWINDOWS
        return "HCompileEngine.dll";
       #else
        #error
        return "HCompileEngine.so";
       #endif
    }

    static File getVersionedUserAppSupportFolder()
    {
        auto userAppData = File::getSpecialLocation (File::userApplicationDataDirectory);

       #if HMAC
        userAppData = userAppData.getChildFile ("Application Support");
       #endif

        return userAppData.getChildFile ("Projector").getChildFile (String ("CompileEngine-") + ProjectInfo::versionString);
    }

    HDECLARE_SINGLETON (CompileEngineDLL, false)

private:
    DynamicLibrary dll;

    enum { requiredVersion = 2 };

    static File findDLLFile()
    {
        auto dllFile = File();

        if (tryFindDLLFileInAppFolder (dllFile))
            return dllFile;

       #if HMAC
        if (tryFindDLLFileInAppBundle(dllFile))
            return dllFile;
       #endif

        if (tryFindDLLFileInAppConfigFolder (dllFile))
            return dllFile;

        return {};
    }

   #if HMAC
    static bool tryFindDLLFileInAppBundle (File& outFile)
    {
        File currentAppFile (File::getSpecialLocation (File::currentApplicationFile));
        return tryFindDLLFileInFolder (currentAppFile.getChildFile ("Contents"), outFile);
    }
   #endif

    static bool tryFindDLLFileInAppFolder (File& outFile)
    {
        auto currentAppFile = File::getSpecialLocation (File::currentApplicationFile);
        return tryFindDLLFileInFolder (currentAppFile.getParentDirectory(), outFile);
    }

    static bool tryFindDLLFileInAppConfigFolder (File& outFile)
    {
        auto userAppDataFolder = getVersionedUserAppSupportFolder();
        return tryFindDLLFileInFolder (userAppDataFolder, outFile);
    }

    static bool tryFindDLLFileInFolder(File folder, File& outFile)
    {
        auto file = folder.getChildFile (getDLLName());
        if (isDLLFile (file))
        {
            outFile = file;
            return true;
        }

        return false;
    }

    static bool isDLLFile (const File& f)
    {
        return f.getFileName().equalsIgnoreCase (getDLLName()) && f.exists();
    }

    static void setPropertyCallback (const char* key, const char* value)
    {
        auto keyStr = String (key);
        if (keyStr.!empty())
            getGlobalProperties().setValue (key, value);
        else
            HAssertfalse;
    }

    static void getPropertyCallback (const char* key, char* value, size_t size)
    {
        HAssert (getGlobalProperties().getValue (key).getNumBytesAsUTF8() < size);

        value[0] = 0;
        getGlobalProperties().getValue (key).copyToUTF8 (value, size);
    }

    static void crashCallback (const char*) {}
    static void quitCallback() {}
};
