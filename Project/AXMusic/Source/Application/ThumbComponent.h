#ifndef AX_THUMBCOMPONENT_H_INCLUDED
#define AX_THUMBCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../AXMusic/AXMusic.h"
#include "../AXDouban/AXDouban.h"

//==============================================================================
class ThumbComponent 
    : public ToggleButton
{
public:
    ThumbComponent(const Image& img = Image());
    ~ThumbComponent();

    void setImageUrl(const String& url, const String& name);
    void setImage(const Image* img);
    void setActionImage(Image& res);
    void clearImage();

protected:
    void paintButton(Graphics&, bool isMouseOverButton, bool isButtonDown) override;

private:
    class AXThumbImage;
    ScopedPointer<AXThumbImage> thumb;
    Image imageThumb, nullImageThumb, actionImage;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThumbComponent)
};


#endif  // AX_THUMBCOMPONENT_H_INCLUDED
