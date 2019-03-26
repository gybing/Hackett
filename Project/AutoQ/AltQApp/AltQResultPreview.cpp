#include "AltQResultPreview.h"
#include "native/AltQNativeHelper.h"
#include "AltQExtentions.h"

//==============================================================================
AltQResultPreview::AltQResultPreview()
{
}

AltQResultPreview::~AltQResultPreview()
{
}

void AltQResultPreview::setPreviewContent(const AltQResultPtr& ptr, const IAltQModuleResultPtr& parsePtr)
{
    customComponent = AltQExtentions::getInstance()->RefreshResultDetailComponent(ptr, parsePtr, customComponent.release());
    if (customComponent != nullptr)
    {
        addAndMakeVisible(customComponent);
        customComponent->setBounds(getLocalBounds());
    }
}
