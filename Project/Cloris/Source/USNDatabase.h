#ifndef USNDATABASE_H_INCLUDED
#define USNDATABASE_H_INCLUDED

#include "JuceHeader.h"

//==============================================================================
class USNDatabase
{
public:
    juce_DeclareSingleton(USNDatabase, false);
    USNDatabase();
    ~USNDatabase();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(USNDatabase)
};

#endif  // USNSEARCHER_H_INCLUDED
