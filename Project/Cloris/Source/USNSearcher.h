#ifndef USNSEARCHER_H_INCLUDED
#define USNSEARCHER_H_INCLUDED

#include "JuceHeader.h"
#include "USNVolume.h"

//==============================================================================
class USNSearcher : public Thread
{
public:
    juce_DeclareSingleton(USNSearcher, false);
    USNSearcher();
    ~USNSearcher();

    void openNTFSVolume();

protected:
    virtual void run() override;

private:
    OwnedArray<USNVolume> volumes;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(USNSearcher)
};

#endif  // USNSEARCHER_H_INCLUDED
