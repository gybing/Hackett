#pragma once
#include "AltQBase.h"
using namespace juce;
// Used for windows to set windows area.
// 

class AltQNative
{
public:
    static Image GetExcuteFileImage(const File& path);
    
};

#define AltQProperties AltQNative::GetAltQProperties()