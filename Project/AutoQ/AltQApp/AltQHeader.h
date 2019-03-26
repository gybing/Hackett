#ifndef __ALTQAPP_HEADER_HH__
#define __ALTQAPP_HEADER_HH__
#include "AltQBase.h"
#include "resources/AltQResources.h"
using namespace juce;

#include "AltQDefine.h"
#include "AltQInterfaceCore.h"
#include "AltQInterfaceModule.h"
#include "AltQInterfaceModuleUI.h"

// Global Shortcuts
#define AQ_HOTKEY       'Q' // globle hotkey
#define IDB_HOTKEY      4007 // globle hotkey
#define QMOD_ALT        0x01

// useage: AltQLoadImage(search_png);
#define AltQLoadImage(src) ImageFileFormat::loadFrom (AltQResources::src, AltQResources::src##Size)

const int AltQConstSearchBarHeight = 58;
const int AltQConstWidth = 680;
const int AltQConstHeight = 430;

const int AltQListItemHeight = 28;
const int AltQListLeftMargin = 20;

static const wchar_t* AltQConstDefaultTipText = L"ËÑË÷";

namespace ProjectInfo
{
    const char* const  projectName    = "AutoQ";
    const char* const  versionString  = "1.0.0";

    const char* const  coreModuleName = "AltQCore.dll";
    const char* const  coreCreateFuncName = "CreateAltQCore";
    const char* const  moduleCreateFuncName = "CreateAltQModule";
    const char* const  moduleUICreateFuncName = "CreateAltQModuleUI";
    const char* const  extentionPathName = "Plugin";
    const char* const  extentionMainName = "main.json";
    const char* const  settingFileName = "settings";
    const int          versionNumber = 0x10000;

}

#endif // __ALTQAPP_HEADER_HH__
