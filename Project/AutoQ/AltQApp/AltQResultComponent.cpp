#include "AltQResultComponent.h"

//==============================================================================
AltQResultComponent::AltQResultComponent()
{
    setComponentID("altq_result");
    // preview
    addAndMakeVisible(resultPreview = new AltQResultPreview);

    // list
    addAndMakeVisible(resultListBox = new AltQResultListBox(resultPreview.get()));
}

AltQResultComponent::~AltQResultComponent()
{
}

void AltQResultComponent::paint (Graphics& g)
{
    const float seg = (0.426f) * AltQConstWidth;
    const int resultHeight = AltQConstHeight - AltQConstSearchBarHeight;

    g.setColour(Colour(213, 214, 216));
    g.drawLine(seg, 0, seg + 2, resultHeight);
}

void AltQResultComponent::resized()
{
    const float seg = (0.426f) * AltQConstWidth;
    const int resultHeight = AltQConstHeight - AltQConstSearchBarHeight;

    resultListBox->setBounds(0, 0, seg, resultHeight);
    resultPreview->setBounds(seg, 0, AltQConstWidth - seg, resultHeight);
}
