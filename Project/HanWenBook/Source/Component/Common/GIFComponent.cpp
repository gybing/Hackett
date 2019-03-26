#include "JuceHeader.h"
#include "ControlComponent.h"

namespace juce
{
    namespace GIFImageHelper
    {
#ifdef JUCE_WIN32

#include <GdiPlus.h>
#pragma comment(lib, "GdiPlus.lib")

        struct GDIInitHelper
        {
            ULONG_PTR m_token;  
            GDIInitHelper() noexcept { Gdiplus::GdiplusStartupInput s; GdiplusStartup(&m_token, &s, nullptr); }
            ~GDIInitHelper() noexcept { Gdiplus::GdiplusShutdown(m_token); }
        };

        static bool loadGIFImageFromFile(int& frameCount, Array<int>& arrDelay, Array<Image>& arrFrames, const String& file)
        {
            static GDIInitHelper gdiIniter;

            if (file.isEmpty())
                return false;

            Gdiplus::Image* image = new Gdiplus::Image(file.toWideCharPointer());
            if (image == nullptr)
                return false;

            unsigned int nCount = image->GetFrameDimensionsCount();
            if (nCount <= 0)
                return false;

            GUID* dimensionIDs = new GUID[nCount];
            if (dimensionIDs != NULL)
            {
                image->GetFrameDimensionsList(dimensionIDs, nCount);
                frameCount = image->GetFrameCount(&dimensionIDs[0]);
                delete[] dimensionIDs;
            }

            unsigned int nSize = image->GetPropertyItemSize(PropertyTagFrameDelay);
            if (nSize <= 0)
                return false;

            Gdiplus::PropertyItem * pPropertyItem = (Gdiplus::PropertyItem *)malloc(nSize);
            if (pPropertyItem != NULL)
            {
                image->GetPropertyItem(PropertyTagFrameDelay, nSize, pPropertyItem);
                for(int i=0; i<frameCount; ++i)
                {
                    arrDelay.add(10 * ((long*)pPropertyItem->value)[i]);
                }
                free(pPropertyItem);
            }

            for(int i=0; i<frameCount; ++i)
            {
                image->SelectActiveFrame(&Gdiplus::FrameDimensionTime,i);
                Gdiplus::Bitmap bmp(image->GetWidth(), image->GetHeight(), PixelFormat32bppPARGB);
                Gdiplus::Graphics g(&bmp);
                g.DrawImage(image, 0, 0);
                Gdiplus::Rect rc(0, 0, image->GetWidth(), image->GetHeight());
                Gdiplus::BitmapData data;
                bmp.LockBits(&rc, 0, PixelFormat32bppPARGB, &data);
                arrFrames.add(CXOption::loadImageFromMemory(data.Scan0, data.Width, data.Height, true));
                bmp.UnlockBits(&data);
            }

            return frameCount > 0;
        }

#else

        static bool loadGIFImageFromFile(int& frameCount, Array<int>& arrDelay, Array<Image>& arrFrames, const String& file) {}

#endif // JUCE_WIN32
    }

    //////////////////////////////////////////////////////////////////////////
    //
    GIFImage::GIFImage(GIFImage::Listener* l, const String& file /*= String::empty*/)
        : listener(l)
        , frameDimensionsCount(0)
        , currentFrame(0)

    {
        if (file.isNotEmpty())
            loadImageFile(file);
    }

    GIFImage::~GIFImage()
    {
    }

    void GIFImage::start()
    {
        if (frameDimensionsCount > 0)
        {
            if (isTimerRunning())
                stopTimer();

            currentFrame = 0;
            startTimer(1);
        }
    }

    void GIFImage::stop()
    {
        if (isTimerRunning())
            stopTimer();
    }

    void GIFImage::timerCallback()
    {
        stopTimer();
        if (listener && currentFrame >= 0 && currentFrame < frameDimensions.size())
        {
            const Image image = frameDimensions.getUnchecked(currentFrame);
            if (!image.isNull())
            {
                listener->animateGIFCallback(image);
            }
        }

        ++currentFrame;

        if (currentFrame >= frameDimensionsCount)
            currentFrame = 0;

        int delay = 200;

        if (currentFrame > 0 && currentFrame < frameDelay.size())
            delay = frameDelay.getUnchecked(currentFrame);

        startTimer(delay > 0 ? delay : 200);
    }

    bool GIFImage::loadImageFile(const String& file)
    {
        return GIFImageHelper::loadGIFImageFromFile(frameDimensionsCount, frameDelay, frameDimensions, file);
    }

    //////////////////////////////////////////////////////////////////////////
    //
    GIFComponent::GIFComponent(const String& file /*= String::empty*/)
        : animateImage(new GIFImage(this, file))
    {
    }

    GIFComponent::~GIFComponent()
    {
    }

    void GIFComponent::paint(Graphics& g)
    {
        if (!imageToDraw.isNull())
        {
            g.drawImage(imageToDraw, 0, 0, getWidth(), getHeight(), 0, 0, imageToDraw.getWidth(), imageToDraw.getHeight());
        }
    }

    void GIFComponent::visibilityChanged()
    {
        if (animateImage)
        {
            if (isVisible())
                animateImage->start();
            else
                animateImage->stop();
        }
    }

    void GIFComponent::animateGIFCallback(Image image)
    {
        imageToDraw = image;
        repaint();
    }

}