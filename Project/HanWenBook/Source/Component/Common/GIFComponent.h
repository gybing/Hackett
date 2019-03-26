#pragma once

class GIFImage
    : public Timer
{
public:
    void start();
    void stop();

    class Listener
    {
    public: 
        virtual void animateGIFCallback(Image) = 0;
    };

    GIFImage(GIFImage::Listener* l, const String& file = String::empty);
    virtual ~GIFImage();
    bool loadImageFile(const String& file);

private:
    virtual void GIFImage::timerCallback() override;
    GIFImage::Listener* listener;
    int frameDimensionsCount, currentFrame;
    Array<int> frameDelay;
    Array<Image> frameDimensions;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GIFImage)
};


class GIFComponent
    : public Component
    , public GIFImage::Listener
{
public:
    GIFComponent(const String& file = String::empty);
    ~GIFComponent();

protected:
    virtual void paint(Graphics& g) override;
    virtual void visibilityChanged() override;
    virtual void animateGIFCallback(Image) override;

private:
    ScopedPointer<GIFImage> animateImage;
    Image imageToDraw;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GIFComponent)
};

