/*

    IMPORTANT! This file is auto-generated each time you save your
    project - if you alter its contents, your changes may be overwritten!

    This is the header file that your files should include in order to get all the
    H library headers. You should avoid including the H headers directly in
    your own source files, because that wouldn't pick up the correct configuration
    options for your app.

*/

#pragma once

#include "AppConfig.h"

#include <core/core.h>


#if ! DONT_SET_USING_HNAMESPACE
 // If your code uses a lot of H classes, then this will obviously save you
 // a lot of typing, but can be disabled by setting DONT_SET_USING_HNAMESPACE.
 using namespace H;
#endif

#if ! HDONT_DECLARE_PROJECTINFO
namespace ProjectInfo
{
    const char* const  projectName    = "BinaryBuilder";
    const char* const  companyName    = "ROLI Ltd.";
    const char* const  versionString  = "1.0.0";
    const int          versionNumber  = 0x10000;
}
#endif
