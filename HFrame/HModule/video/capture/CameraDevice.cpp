/*
  ==============================================================================

   This file is part of the H library.
   Copyright (c) 2017 - ROLI Ltd.

   H is an open source library subject to commercial or open-source
   licensing.

   By using H, you agree to the terms of both the H 5 End-User License
   Agreement and H 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.H.com/H-5-licence
   Privacy Policy: www.H.com/H-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   H IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#if HMAC
 #include "../native/mac_CameraDevice.h"
#elif HWINDOWS
 #include "../native/win32_CameraDevice.h"
#elif HIOS
 #if HCLANG
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunguarded-availability-new"
 #endif

 #include "../native/ios_CameraDevice.h"

 #if HCLANG
  #pragma clang diagnostic pop
 #endif
#elif HANDROID
 #include "../native/android_CameraDevice.h"
#endif

#if HANDROID || HIOS
//==============================================================================
class CameraDevice::CameraFactory
{
public:
    static CameraFactory& getInstance()
    {
        static CameraFactory factory;
        return factory;
    }

    void openCamera (int index, OpenCameraResultCallback resultCallback,
                     int minWidth, int minHeight, int maxWidth, int maxHeight, bool useHighQuality)
    {
        auto cameraId = getAvailableDevices()[index];

        if (getCameraIndex (cameraId) != -1)
        {
            // You are trying to open the same camera twice.
            HAssertfalse;
            return;
        }

        std::unique_ptr<CameraDevice> device (new CameraDevice (cameraId, index,
                                                                minWidth, minHeight, maxWidth,
                                                                maxHeight, useHighQuality));

        camerasToOpen.add ({ nextRequestId++,
                             std::unique_ptr<CameraDevice> (device.release()),
                             resultCallback });

        auto& pendingOpen = camerasToOpen.getReference (camerasToOpen.size() - 1);

        pendingOpen.device->pimpl->open ([this](const String& deviceId, const String& error)
                                         {
                                             int cIndex = getCameraIndex (deviceId);

                                             if (cIndex == -1)
                                                 return;

                                             auto& cameraPendingOpen = camerasToOpen.getReference (cIndex);

                                             if (error.isEmpty())
                                                 cameraPendingOpen.resultCallback (cameraPendingOpen.device.release(), error);
                                             else
                                                 cameraPendingOpen.resultCallback (nullptr, error);

                                             int id = cameraPendingOpen.requestId;

                                             MessageManager::callAsync ([this, id]() { removeRequestWithId (id); });
                                         });
    }

private:
    int getCameraIndex (const String& cameraId) const
    {
        for (int i = 0; i < camerasToOpen.size(); ++i)
        {
            auto& pendingOpen = camerasToOpen.getReference (i);

            if (pendingOpen.device->pimpl->getCameraId() == cameraId)
                return i;
        }

        return -1;
    }

    void removeRequestWithId (int id)
    {
        for (int i = camerasToOpen.size(); --i >= 0;)
        {
            if (camerasToOpen.getReference (i).requestId == id)
            {
                camerasToOpen.remove (i);
                return;
            }
        }
    }

    struct PendingCameraOpen
    {
        int requestId;
        std::unique_ptr<CameraDevice> device;
        OpenCameraResultCallback resultCallback;
    };

    Array<PendingCameraOpen> camerasToOpen;
    static int nextRequestId;
};

int CameraDevice::CameraFactory::nextRequestId = 0;

#endif

//==============================================================================
CameraDevice::CameraDevice (const String& nm, int index, int minWidth, int minHeight, int maxWidth, int maxHeight, bool useHighQuality)
   : name (nm), pimpl (new Pimpl (*this, name, index, minWidth, minHeight, maxWidth, maxHeight, useHighQuality))
{
}

CameraDevice::~CameraDevice()
{
    HAssert (MessageManager::getInstance()->currentThreadHasLockedMessageManager());

    stopRecording();
    pimpl.reset();
}

Component* CameraDevice::createViewerComponent()
{
    return new ViewerComponent (*this);
}

void CameraDevice::takeStillPicture (std::function<void(const Image&)> pictureTakenCallback)
{
    pimpl->takeStillPicture (pictureTakenCallback);
}

void CameraDevice::startRecordingToFile (const File& file, int quality)
{
    stopRecording();
    pimpl->startRecordingToFile (file, quality);
}

Time CameraDevice::getTimeOfFirstRecordedFrame() const
{
    return pimpl->getTimeOfFirstRecordedFrame();
}

void CameraDevice::stopRecording()
{
    pimpl->stopRecording();
}

void CameraDevice::addListener (Listener* listenerToAdd)
{
    if (listenerToAdd != nullptr)
        pimpl->addListener (listenerToAdd);
}

void CameraDevice::removeListener (Listener* listenerToRemove)
{
    if (listenerToRemove != nullptr)
        pimpl->removeListener (listenerToRemove);
}

//==============================================================================
StringArray CameraDevice::getAvailableDevices()
{
    HAUTORELEASEPOOL
    {
        return Pimpl::getAvailableDevices();
    }
}

CameraDevice* CameraDevice::openDevice (int index,
                                        int minWidth, int minHeight,
                                        int maxWidth, int maxHeight,
                                        bool useHighQuality)
{
    HAssert (MessageManager::getInstance()->currentThreadHasLockedMessageManager());

   #if ! HANDROID && ! HIOS
    std::unique_ptr<CameraDevice> d (new CameraDevice (getAvailableDevices() [index], index,
                                                       minWidth, minHeight, maxWidth, maxHeight, useHighQuality));
    if (d != nullptr && d->pimpl->openedOk())
        return d.release();
   #else
    ignoreUnused (index, minWidth, minHeight);
    ignoreUnused (maxWidth, maxHeight, useHighQuality);

    // Use openDeviceAsync to open a camera device on iOS or Android.
    HAssertfalse;
   #endif

    return nullptr;
}

void CameraDevice::openDeviceAsync (int index, OpenCameraResultCallback resultCallback,
                                    int minWidth, int minHeight, int maxWidth, int maxHeight, bool useHighQuality)
{
    HAssert (MessageManager::getInstance()->currentThreadHasLockedMessageManager());

    if (resultCallback == nullptr)
    {
        // A valid callback must be passed.
        HAssertfalse;
        return;
    }

   #if HANDROID || HIOS
    CameraFactory::getInstance().openCamera (index, std::move (resultCallback),
                                             minWidth, minHeight, maxWidth, maxHeight, useHighQuality);
   #else
    auto* device = openDevice (index, minWidth, minHeight, maxWidth, maxHeight, useHighQuality);

    resultCallback (device, device != nullptr ? String() : "Could not open camera device");
   #endif
}


