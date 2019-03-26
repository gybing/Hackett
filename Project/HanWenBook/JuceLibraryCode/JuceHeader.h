#ifndef __APPHEADERFILE_PGPDF1__
#define __APPHEADERFILE_PGPDF1__

#include "AppConfig.h"
#include "modules/juce_core/juce_core.h"
#include "modules/juce_cryptography/juce_cryptography.h"
#include "modules/juce_data_structures/juce_data_structures.h"
#include "modules/juce_events/juce_events.h"
#include "modules/juce_graphics/juce_graphics.h"
#include "modules/juce_gui_basics/juce_gui_basics.h"
#include "modules/juce_gui_extra/juce_gui_extra.h"

namespace juce{
#include "Component/Common/ControlComponent.h"
#include "Component/Common/GIFComponent.h"
#include "Component/Common/AttributedImageText.h"
#include "Component/Common/ImageTextLayout.h"
}

 // custom
#include "Component/Common/ListView.h"
#include "Component/Common/ExpandListView.h"
#include "Component/Common/DocumentWindow.h"
#include "Component/Common/DialogWindow.h"
#include "Component/Common/HorListBox.h"
#include "Component/Common/ReadListView.h"
#include "Component/Common/ShelfViewComponent.h"
#include "LookAndFeel/LookAndFeelBase.h"

#if _MSC_VER < 1800
#include "false"
#endif

#include "../Source/Data/HWInclude.h"

#ifdef JUCE_WINDOWS

#include "Component/Common/CEFBrowserControl.h"

#define CXIMAGE_CACHE_LOAD(n) juce::ImageCache::getFromFile (juce::File(juce::File::getCurrentWorkingDirectory().getFullPathName() + L"\\res\\images\\" + n))
#define CXDRAWABLE_LOAD(n) juce::Drawable::createFromImageFile (juce::File(juce::File::getCurrentWorkingDirectory().getFullPathName() + L"\\res\\images\\" + n))
 
#else if JUCE_MAC

#define CXIMAGE_CACHE_LOAD(n) juce::ImageCache::getFromFile (juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentApplicationFile).getChildFile(juce::String(L"Contents/Resources/") + n))
#define CXDRAWABLE_LOAD(n) juce::Drawable::createFromImageFile (juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentApplicationFile).getChildFile(juce::String(L"Contents/Resources/") + n))

#endif // JUCE_WINDOWS


#if ! JUCE_DONT_DECLARE_PROJECTINFO

 namespace ProjectInfo
 {
     static const wchar_t* const  projectTitle   = L"乐学阅读";
     static const wchar_t* const  projectName    = L"HanWenBook";
     static const wchar_t* const  versionString  = L"2.0.0";
     static const int             versionNumber  = 0x10104;
#ifdef _DEBUG
     static const wchar_t*  updaterName = L"HWToolD.exe";
     static const wchar_t*  fileName = L"LXReadD.exe";
#else
     static const wchar_t*  updaterName = L"HWTool.exe";
     static const wchar_t*  fileName = L"LXRead.exe";
#endif

     static const wchar_t* crashRptLogName	= L"CrashRptlog.txt";
     static const wchar_t* crashReportLogName = L"CrashReportLog.txt";

     // 默认书架大小
     const float DefaultBookPanelWidth = 500.0f;
     const float DefaultBookPanelHeight = 735.0f;

     // 默认图书大小
     const float DefaultBookWidth = 96.0f;
     const float DefaultBookHeight = 135.0f;

     // 分辨率变换相关
     const int DefaultFrameMaxWidth  = 1280; // 主界面最小高度
     const int DefaultFrameMaxHeight = 800; // 主界面最小宽度 
     const int DefaultFrameMinWidth  = 1024; // 主界面最小高度
     const int DefaultFrameMinHeight = 720; // 主界面最小宽度 

     // 常用颜色常量
     const juce::Colour colorLightBlack(0xFF3E3A39);
     const juce::Colour colorLightWhite(0xFFF3F3F0);
     const juce::Colour colorLightRed	(0xffF56863);
     const juce::Colour colorLightBlue	(0xff0594c1);
     const juce::Colour colorLightGrey	(0xff9fa0a0);
     const juce::Colour colorLightBrown (0xff6b5147); // 棕色

     const juce::Colour colorBlueBlack (0xFF59B2BA); // 背景，边框
     const juce::Colour colorGoldYello (0xFFF08E00); // 评分
     const juce::Colour colorDarkGray	(0xFF404040);	// 阅读桌面背景
     const juce::Colour colorDarkLightGray	(0xFF585858);	// 阅读桌面背景
     const juce::Colour colorEyeGreen	(0xFFC7EDCC);

     const juce::Colour colorBlueButton (0xFF507299); //淡蓝色按钮

     const wchar_t* const LoginWebCallback = L"id.lejiaolexue.com";
     const wchar_t* const LoginWebUrl = L"http://id.lejiaolexue.com//oauth/login.aspx?callback=http://id.lejiaolexue.com//oauth/login.aspx";

     const int PagenoInvalid = -5; // 无效的PDF页码
     const int PagenoFront   = -1; // 封面
     const int PagenoBack    = -2; // 封底
 }
#endif

#include "../Source/Component/ReadComponent/ReadComponent.h"
#include "../Source/Component/LoginComponent.h"
#include "../Source/Component/PopupComponent/PreviewComponent.h"
#include "../Source/Component/PopupComponent/OptionComponent.h"
#include "../Source/Component/PopupComponent/EmojiComponent.h"

#endif   // __APPHEADERFILE_PGPDF1__
