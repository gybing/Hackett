#pragma once
#include "../AltQHeader.h"

// Used for windows to set windows area.
// 

class AltQNative
{
public:
    static bool SetWindowTransparent(void* handle);
    static bool RegisterHotKey(void* handle, int id, unsigned int fsModifiers, unsigned int vk);
    static bool UnregisterHotKey(void* handle, int id);
    static String GetStoragePath();
    static ApplicationProperties* GetAltQProperties();
    static bool SetStartupBoot(bool s);
    static bool GetStartupBoot();
    static bool elevateToAdminRights(String commandLineArgs);
    

};



#define AltQProperties AltQNative::GetAltQProperties()