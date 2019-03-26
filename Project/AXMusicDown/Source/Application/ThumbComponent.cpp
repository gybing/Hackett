#include "ThumbComponent.h"

//==============================================================================
class ThumbComponent::AXThumbImage : public Thread
{
public:
    AXThumbImage(std::function<void(const Image*)> f)
        : callback(f), Thread("ThubmThread")
    {
        startThread();
    }

    ~AXThumbImage()
    {
        stopThread(-1);
    }

    void setImageURL(const String& url, const String& name)
    {
        if (name.isNotEmpty())
        {
            Image img = ImageFileFormat::loadFrom(name);
            if (!img.isNull())
            {
                callback(&img);
                return;
            }
        }

        if (url.isNotEmpty())
        {
            imageFileName = name;
            imageUrl = url;
            notify();
        }
    }

private:
    static void* OnThumbImageCallback(void* userData)
    {
        if (AXThumbImage* thumb = static_cast<AXThumbImage*>(userData))
        {
            thumb->callback(&thumb->thumbImage);
        }
        return nullptr;
    }

    virtual void run() override
    {
        String strUrl, strFileName;
        while (!threadShouldExit())
        {
            strUrl = imageUrl;
            strFileName = imageFileName;

            imageUrl.clear();
            imageFileName.clear();

            if (strUrl.isEmpty())
            {
                wait(-1);
            }
            else
            {
                URL url(strUrl);
                ScopedPointer<InputStream> stream = url.createInputStream(false);
                if (stream)
                {
                    if (strFileName.isNotEmpty())
                    {
                        thumbImage = ImageFileFormat::loadFrom(*stream);
                        juce::MessageManager::getInstance()->callFunctionOnMessageThread(&AXThumbImage::OnThumbImageCallback, this);

                        // File picFile(strFileName);
                        // String ext = picFile.getFileExtension();
                        // if (ext == ".png" || ext == ".PNG")
                        // {
                        //     FileOutputStream fs(picFile);
                        //     PNGImageFormat format;
                        //     format.writeImageToStream(thumbImage, fs);
                        // }
                        // else if (ext == ".jpg" || ext == ".JPG")
                        // {
                        //     FileOutputStream fs(picFile);
                        //     JPEGImageFormat format;
                        //     format.writeImageToStream(thumbImage, fs);
                        // }
                        // else if(ext == ".gif" || ext == ".GIF")
                        // {
                        //     FileOutputStream fs(picFile);
                        //     GIFImageFormat format;
                        //     format.writeImageToStream(thumbImage, fs);
                        // }
                    }
                    else
                    {
                        thumbImage = ImageFileFormat::loadFrom(*stream);
                        juce::MessageManager::getInstance()->callFunctionOnMessageThread(&AXThumbImage::OnThumbImageCallback, this);
                    }
                }
                else
                {
                    thumbImage = Image();
                    juce::MessageManager::getInstance()->callFunctionOnMessageThread(&AXThumbImage::OnThumbImageCallback, this);
                }
            }
        }
    }

    Image thumbImage;
    std::function<void(const Image*)> callback;
    String imageUrl, imageFileName;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AXThumbImage)
};


//==============================================================================
ThumbComponent::ThumbComponent(const Image& img /*= Image()*/)
{
    nullImageThumb = img;

    thumb = new AXThumbImage([=](const Image* img){
        setImage(img);
        repaint();
    });
}

ThumbComponent::~ThumbComponent()
{
}

void ThumbComponent::setImageUrl(const String& url, const String& name)
{
    thumb->setImageURL(url, name);
}

void ThumbComponent::setImage(const Image* img)
{
    imageThumb = img ? *img : Image();
}

void ThumbComponent::clearImage()
{
    imageThumb = Image();
}

void ThumbComponent::paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown)
{
    Image* imageToDraw = nullptr;
    imageToDraw = imageThumb.isNull() ? (nullImageThumb.isNull() ? nullptr : &nullImageThumb) : &imageThumb; 

    if (imageToDraw != nullptr)
    {
        g.drawImage(*imageToDraw, 0, 0, getWidth(), getHeight(), 0, 0, imageToDraw->getWidth(), imageToDraw->getHeight());
    }

    if (isButtonDown || getToggleState())
        g.fillAll(Colour(0x5F000000));
    else if (isMouseOverButton)
        g.fillAll(Colour(0x50000000));
    
    if (getToggleState() && !actionImage.isNull())
        g.drawImage(actionImage, 0.25f * getWidth(), 0.25f * getHeight(), 0.5f * getWidth(), 0.5f * getHeight(),
            0, 0, actionImage.getWidth(), actionImage.getHeight());
}

void ThumbComponent::setActionImage(Image& res)
{
    actionImage = res;
}
