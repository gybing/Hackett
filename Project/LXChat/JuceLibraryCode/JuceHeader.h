/*

    IMPORTANT! This file is auto-generated each time you save your
    project - if you alter its contents, your changes may be overwritten!

    This is the header file that your files should include in order to get all the
    JUCE library headers. You should avoid including the JUCE headers directly in
    your own source files, because that wouldn't pick up the correct configuration
    options for your app.

*/

#ifndef __APPHEADERFILE_G0IEF1__
#define __APPHEADERFILE_G0IEF1__

#include "AppConfig.h"
#include "modules/juce_core/juce_core.h"
#include "modules/juce_cryptography/juce_cryptography.h"
#include "modules/juce_data_structures/juce_data_structures.h"
#include "modules/juce_events/juce_events.h"
#include "modules/juce_graphics/juce_graphics.h"
#include "modules/juce_gui_basics/juce_gui_basics.h"
#include "modules/juce_gui_extra/juce_gui_extra.h"
#include "modules/juce_opengl/juce_opengl.h"
#include "modules/juce_video/juce_video.h"

#if ! DONT_SET_USING_JUCE_NAMESPACE
 // If your code uses a lot of JUCE classes, then this will obviously save you
 // a lot of typing, but can be disabled by setting DONT_SET_USING_JUCE_NAMESPACE.
 using namespace juce;
#endif

#if ! JUCE_DONT_DECLARE_PROJECTINFO
namespace ProjectInfo
{
    const wchar_t* const  projectName    = L"Î¢ÐÅ";
    const char* const  versionString  = "1.0.0";
    const int          versionNumber  = 0x10000;

    const Colour colorLightBlack(0xFF3E3A39);
    const Colour colorLightWhite(0xFFF3F3F0);
    const Colour colorLightRed(0xffF56863);
    const Colour colorLightBlue(0xff0594c1);
    const Colour colorLightGrey(0xff9fa0a0);
    const Colour colorLightBrown(0xff6b5147); // ×ØÉ«

    const Colour colorBlueBlack(0xFF59B2BA); // ±³¾°£¬±ß¿ò
    const Colour colorGoldYello(0xFFF08E00); // ÆÀ·Ö
    const Colour colorDarkGray(0xFF404040);	// ÔÄ¶Á×ÀÃæ±³¾°
    const Colour colorDarkLightGray(0xFF585858);	// ÔÄ¶Á×ÀÃæ±³¾°
    const Colour colorEyeGreen(0xFFC7EDCC);

    const Colour colorBlueButton(0xFF507299); //µ­À¶É«°´Å¥

    //////////////////////////////////////////////////////////////////////////
    inline Colour getRandomColour(float brightness)
    {
        return Colour::fromHSV(Random::getSystemRandom().nextFloat(), 0.5f, brightness, 1.0f);
    }

    inline Colour getRandomBrightColour()   { return getRandomColour(0.8f); }
    inline Colour getRandomDarkColour()     { return getRandomColour(0.3f); }
}
#endif

#define LXLoadImage(n) ImageFileFormat::loadFrom(juce::File(juce::File::getCurrentWorkingDirectory().getFullPathName() + L"\\Resources\\" + n))
#define LXLoadCacheImage(n) juce::ImageCache::getFromFile (juce::File(juce::File::getCurrentWorkingDirectory().getFullPathName() + L"\\Resources\\" + n))

#include "..\Source\Common\LXComponentHelper.h"
#include "..\Source\Common\LXProtocol.h"
#include "..\Source\Common\LXComponentAnimator.h"
#include "..\Source\Common\LXGridView.h"

#endif   // __APPHEADERFILE_G0IEF1__
