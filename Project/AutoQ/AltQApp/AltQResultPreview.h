#pragma once
#include "AltQHeader.h"

//==============================================================================
class AltQResultPreview : public Component
{
public:
    //==============================================================================
    AltQResultPreview();
    ~AltQResultPreview();

    void setPreviewContent(const AltQResultPtr& ptr, const IAltQModuleResultPtr& parsePtr);

private:
    ScopedPointer<Component> customComponent;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AltQResultPreview)
};
