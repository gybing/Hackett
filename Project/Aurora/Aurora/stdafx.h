#pragma once

#define _CRT_NON_CONFORMING_SWPRINTFS
#define WIN32_LEAN_AND_MEAN

#include "AppConfig.h"
#include "modules/juce_core/juce_core.h"
#include "modules/juce_audio_basics/juce_audio_basics.h"
#include "modules/juce_cryptography/juce_cryptography.h"
#include "modules/juce_data_structures/juce_data_structures.h"
#include "modules/juce_events/juce_events.h"
#include "modules/juce_graphics/juce_graphics.h"
#include "modules/juce_gui_basics/juce_gui_basics.h"
#include "modules/juce_gui_extra/juce_gui_extra.h"

using namespace juce;

// custom
#include "Configuration/Configuration.h"
#include "LookAndFeel/LAFBase.h"
#include "Component/DocumentWindow.h"
#include "Component/DialogWindow.h"

// library
#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
typedef int (CALLBACK *PROC)();
typedef int (FAR WINAPI *FARPROC)();

#if ! JUCE_DONT_DECLARE_PROJECTINFO
namespace ProjectInfo
{
    const char* const  projectName    = "Aurora";
    const char* const  versionString  = "1.0.0";
    const int          versionNumber  = 0x10000;
}
#endif
