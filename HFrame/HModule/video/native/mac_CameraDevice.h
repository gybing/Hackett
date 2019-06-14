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

struct CameraDevice::Pimpl
{
    Pimpl (CameraDevice& ownerToUse, const String&, int /*index*/, int /*minWidth*/, int /*minHeight*/,
           int /*maxWidth*/, int /*maxHeight*/, bool useHighQuality)
        : owner (ownerToUse)
    {
        HAUTORELEASEPOOL
        {
            captureView = [[AVCaptureView alloc] init];
            session = captureView.session;

            session.sessionPreset = useHighQuality ? AVCaptureSessionPresetHigh
                                                   : AVCaptureSessionPresetMedium;

            refreshConnections();

            static DelegateClass cls;
            callbackDelegate = (id<AVCaptureFileOutputRecordingDelegate>) [cls.createInstance() init];
            DelegateClass::setOwner (callbackDelegate, this);

            SEL runtimeErrorSel = NSSelectorFromString (nsStringLiteral ("captureSessionRuntimeError:"));

            [[NSNotificationCenter defaultCenter] addObserver: callbackDelegate
                                                     selector: runtimeErrorSel
                                                         name: AVCaptureSessionRuntimeErrorNotification
                                                       object: session];
        }
    }

    ~Pimpl()
    {
        [[NSNotificationCenter defaultCenter] removeObserver: callbackDelegate];

        [session stopRunning];
        removeImageCapture();
        removeMovieCapture();
        [session release];
        [callbackDelegate release];
    }

    bool openedOk() const noexcept       { return openingError.empty(); }

    void addImageCapture()
    {
        if (imageOutput == nil)
        {
            imageOutput = [[AVCaptureStillImageOutput alloc] init];
            auto imageSettings = [[NSDictionary alloc] initWithObjectsAndKeys: AVVideoCodecJPEG, AVVideoCodecKey, nil];
            [imageOutput setOutputSettings: imageSettings];
            [imageSettings release];
            [session addOutput: imageOutput];
        }
    }

    void addMovieCapture()
    {
        if (fileOutput == nil)
        {
            fileOutput = [[AVCaptureMovieFileOutput alloc] init];
            [session addOutput: fileOutput];
        }
    }

    void removeImageCapture()
    {
        if (imageOutput != nil)
        {
            [session removeOutput: imageOutput];
            [imageOutput release];
            imageOutput = nil;
        }
    }

    void removeMovieCapture()
    {
        if (fileOutput != nil)
        {
            [session removeOutput: fileOutput];
            [fileOutput release];
            fileOutput = nil;
        }
    }

    void refreshConnections()
    {
        [session beginConfiguration];
        removeImageCapture();
        removeMovieCapture();
        addImageCapture();
        addMovieCapture();
        [session commitConfiguration];
    }

    void refreshIfNeeded()
    {
        if (getVideoConnection() == nullptr)
            refreshConnections();
    }

    void takeStillPicture (std::function<void(const Image&)> pictureTakenCallbackToUse)
    {
        if (pictureTakenCallbackToUse == nullptr)
        {
            HAssertfalse;
            return;
        }

        pictureTakenCallback = std::move (pictureTakenCallbackToUse);

        triggerImageCapture();
    }

    void startRecordingToFile (const File& file, int /*quality*/)
    {
        stopRecording();
        refreshIfNeeded();
        firstPresentationTime = Time::getCurrentTime();
        file.deleteFile();

        isRecording = true;
        [fileOutput startRecordingToOutputFileURL: createNSURLFromFile (file)
                                recordingDelegate: callbackDelegate];
    }

    void stopRecording()
    {
        if (isRecording)
        {
            [fileOutput stopRecording];
            isRecording = false;
        }
    }

    Time getTimeOfFirstRecordedFrame() const
    {
        return firstPresentationTime;
    }

    AVCaptureConnection* getVideoConnection() const
    {
        if (imageOutput != nil)
            for (AVCaptureConnection* connection in imageOutput.connections)
                if ([connection isActive] && [connection isEnabled])
                    for (AVCaptureInputPort* port in [connection inputPorts])
                        if ([[port mediaType] isEqual: AVMediaTypeVideo])
                            return connection;

        return nil;
    }

    void handleImageCapture (const Image& image)
    {
        const ScopedLock sl (listenerLock);
        listeners.call ([=] (Listener& l) { l.imageReceived (image); });

        if (! listeners.empty())
            triggerImageCapture();
    }

    void triggerImageCapture()
    {
        refreshIfNeeded();

        if (auto* videoConnection = getVideoConnection())
        {
            [imageOutput captureStillImageAsynchronouslyFromConnection: videoConnection
                                                     completionHandler: ^(CMSampleBufferRef sampleBuffer, NSError* error)
            {
                if (error != nil)
                {
                    HCAMERA_LOG ("Still picture capture failed, error: " + nsStringToJuce (error.localizedDescription));
                    HAssertfalse;
                    return;
                }

                NSData* imageData = [AVCaptureStillImageOutput jpegStillImageNSDataRepresentation: sampleBuffer];

                auto image = ImageFileFormat::loadFrom (imageData.bytes, (size_t) imageData.length);

                handleImageCapture (image);

                WeakReference<Pimpl> weakRef (this);
                MessageManager::callAsync ([weakRef, image]() mutable
                {
                    if (weakRef != nullptr && weakRef->pictureTakenCallback != nullptr)
                        weakRef->pictureTakenCallback (image);
                });
            }];
        }
    }

    void addListener (CameraDevice::Listener* listenerToAdd)
    {
        const ScopedLock sl (listenerLock);
        listeners.add (listenerToAdd);

        if (listeners.size() == 1)
            triggerImageCapture();
    }

    void removeListener (CameraDevice::Listener* listenerToRemove)
    {
        const ScopedLock sl (listenerLock);
        listeners.remove (listenerToRemove);
    }

    static StringArray getAvailableDevices()
    {
        StringArray results;
        results.add ("default");
        return results;
    }

    void cameraSessionRuntimeError (const String& error)
    {
        HCAMERA_LOG ("cameraSessionRuntimeError(), error = " + error);

        if (owner.onErrorOccurred != nullptr)
            owner.onErrorOccurred (error);
    }

    CameraDevice& owner;
    AVCaptureView* captureView = nil;
    AVCaptureSession* session = nil;
    AVCaptureMovieFileOutput* fileOutput = nil;
    AVCaptureStillImageOutput* imageOutput = nil;

    id<AVCaptureFileOutputRecordingDelegate> callbackDelegate = nil;
    String openingError;
    Time firstPresentationTime;
    bool isRecording = false;

    CriticalSection listenerLock;
    ListenerList<Listener> listeners;

    std::function<void(const Image&)> pictureTakenCallback;

    HDECLARE_WEAK_REFERENCEABLE (Pimpl)

private:
    //==============================================================================
    struct DelegateClass  : public ObjCClass<NSObject>
    {
        DelegateClass()  : ObjCClass<NSObject> ("HCameraDelegate_")
        {
            addIvar<Pimpl*> ("owner");
            addProtocol (@protocol (AVCaptureFileOutputRecordingDelegate));

            addMethod (@selector (captureOutput:didStartRecordingToOutputFileAtURL:  fromConnections:),       didStartRecordingToOutputFileAtURL,   "v@:@@@");
            addMethod (@selector (captureOutput:didPauseRecordingToOutputFileAtURL:  fromConnections:),       didPauseRecordingToOutputFileAtURL,   "v@:@@@");
            addMethod (@selector (captureOutput:didResumeRecordingToOutputFileAtURL: fromConnections:),       didResumeRecordingToOutputFileAtURL,  "v@:@@@");
            addMethod (@selector (captureOutput:willFinishRecordingToOutputFileAtURL:fromConnections:error:), willFinishRecordingToOutputFileAtURL, "v@:@@@@");

            SEL runtimeErrorSel = NSSelectorFromString (nsStringLiteral ("captureSessionRuntimeError:"));
            addMethod (runtimeErrorSel, sessionRuntimeError, "v@:@");

            registerClass();
        }

        static void setOwner (id self, Pimpl* owner)   { object_setInstanceVariable (self, "owner", owner); }
        static Pimpl& getOwner (id self)               { return *getIvar<Pimpl*> (self, "owner"); }

    private:
        static void didStartRecordingToOutputFileAtURL (id, SEL, AVCaptureFileOutput*, NSURL*, NSArray*) {}
        static void didPauseRecordingToOutputFileAtURL (id, SEL, AVCaptureFileOutput*, NSURL*, NSArray*) {}
        static void didResumeRecordingToOutputFileAtURL (id, SEL, AVCaptureFileOutput*, NSURL*, NSArray*) {}
        static void willFinishRecordingToOutputFileAtURL (id, SEL, AVCaptureFileOutput*, NSURL*, NSArray*, NSError*) {}

        static void sessionRuntimeError (id self, SEL, NSNotification* notification)
        {
            HCAMERA_LOG (nsStringToJuce ([notification description]));

            NSError* error = notification.userInfo[AVCaptureSessionErrorKey];
            auto errorString = error != nil ? nsStringToJuce (error.localizedDescription) : String();
            getOwner (self).cameraSessionRuntimeError (errorString);
        }
    };

    HDECLARE_NON_COPYABLE (Pimpl)
};

struct CameraDevice::ViewerComponent  : public NSViewComponent
{
    ViewerComponent (CameraDevice& d)
    {
        HAUTORELEASEPOOL
        {
            setSize (640, 480);
            setView (d.pimpl->captureView);
        }
    }

    ~ViewerComponent()
    {
        setView (nil);
    }

    HDECLARE_NON_COPYABLE (ViewerComponent)
};

String CameraDevice::getFileExtension()
{
    return ".mov";
}
