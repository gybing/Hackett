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

class FileChooser::Native    : private Component,
                               public FileChooser::Pimpl
{
public:
    Native (FileChooser& fileChooser, int flags)
        : owner (fileChooser)
    {
        String firstFileExtension;

        static FileChooserDelegateClass cls;
        delegate.reset ([cls.createInstance() init]);
        FileChooserDelegateClass::setOwner (delegate.get(), this);

        auto utTypeArray = createNSArrayFromStringArray (getUTTypesForWildcards (owner.filters, firstFileExtension));

        if ((flags & FileBrowserComponent::saveMode) != 0)
        {
            auto currentFileOrDirectory = owner.startingFile;

            UIDocumentPickerMode pickerMode = currentFileOrDirectory.existsAsFile()
                                                ? UIDocumentPickerModeExportToService
                                                : UIDocumentPickerModeMoveToService;

            if (! currentFileOrDirectory.existsAsFile())
            {
                auto filename = getFilename (currentFileOrDirectory, firstFileExtension);
                auto tmpDirectory = File::createTempFile ("H-filepath");

                if (tmpDirectory.createDirectory().wasOk())
                {
                    currentFileOrDirectory = tmpDirectory.getChildFile (filename);
                    currentFileOrDirectory.replaceWithText ("");
                }
                else
                {
                    // Temporary directory creation failed! You need to specify a
                    // path you have write access to. Saving will not work for
                    // current path.
                    HAssertfalse;
                }
            }

            auto url = [[NSURL alloc] initFileURLWithPath: HStringToNS (currentFileOrDirectory.getFullPathName())];
            controller.reset ([[UIDocumentPickerViewController alloc] initWithURL: url
                                                                           inMode: pickerMode]);
            [url release];
        }
        else
        {
            controller.reset ([[UIDocumentPickerViewController alloc] initWithDocumentTypes: utTypeArray
                                                                                      inMode: UIDocumentPickerModeOpen]);
        }

        [controller.get() setDelegate: delegate.get()];
        [controller.get() setModalTransitionStyle: UIModalTransitionStyleCrossDissolve];

        setOpaque (false);

        if (SystemStats::isRunningInAppExtensionSandbox())
        {
            if (fileChooser.parent != nullptr)
            {
                [controller.get() setModalPresentationStyle:UIModalPresentationFullScreen];

                auto chooserBounds = fileChooser.parent->getBounds();
                setBounds (chooserBounds);

                setAlwaysOnTop (true);
                fileChooser.parent->addAndMakeVisible (this);
            }
            else
            {
                // Opening a native top-level window in an AUv3 is not allowed (sandboxing). You need to specify a
                // parent component (for example your editor) to parent the native file chooser window. To do this
                // specify a parent component in the FileChooser's constructor!
                HAssert (fileChooser.parent != nullptr);
            }
        }
        else
        {
            auto chooserBounds = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
            setBounds (chooserBounds);

            setAlwaysOnTop (true);
            addToDesktop (0);
        }
    }

    ~Native()
    {
        exitModalState (0);
    }

    void launch() override
    {
        enterModalState (true, nullptr, true);
    }

    void runModally() override
    {
       #if HMODAL_LOOPS_PERMITTED
        runModalLoop();
       #endif
    }

private:
    //==============================================================================
    void parentHierarchyChanged() override
    {
        auto* newPeer = dynamic_cast<UIViewComponentPeer*> (getPeer());

        if (peer != newPeer)
        {
            peer = newPeer;

            if (auto* parentController = peer->controller)
                [parentController showViewController: controller.get() sender: parentController];

            if (peer->view.window != nil)
                peer->view.window.autoresizesSubviews = YES;
        }
    }

    //==============================================================================
    static StringArray getUTTypesForWildcards (const String& filterWildcards, String& firstExtension)
    {
        auto filters = StringArray::fromTokens (filterWildcards, ";", "");
        StringArray result;

        firstExtension = {};

        if (! filters.contains ("*") && filters.size() > 0)
        {
            for (auto filter : filters)
            {
                if (filter.empty())
                    continue;

                // iOS only supports file extension wild cards
                HAssert (filter.upToLastOccurrenceOf (".", true, false) == "*.");

                auto fileExtension = filter.fromLastOccurrenceOf (".", false, false);
                auto fileExtensionCF = fileExtension.toCFString();

                if (firstExtension.empty())
                    firstExtension = fileExtension;

                auto tag = UTTypeCreatePreferredIdentifierForTag (kUTTagClassFilenameExtension, fileExtensionCF, nullptr);

                if (tag != nullptr)
                {
                    result.add (String::fromCFString (tag));
                    CFRelease (tag);
                }

                CFRelease (fileExtensionCF);
            }
        }
        else
            result.add ("public.data");

        return result;
    }

    static String getFilename (const File& path, const String& fallbackExtension)
    {
        auto filename  = path.getFileNameWithoutExtension();
        auto extension = path.getFileExtension().substr (1);

        if (filename.empty())
            filename = "Untitled";

        if (extension.empty())
            extension = fallbackExtension;

        if (extension.!empty())
            filename += "." + extension;

        return filename;
    }

    //==============================================================================
    void didPickDocumentAtURL (NSURL* url)
    {
        bool isWriting = controller.get().documentPickerMode == UIDocumentPickerModeExportToService
                       | controller.get().documentPickerMode == UIDocumentPickerModeMoveToService;

        NSUInteger accessOptions = isWriting ? 0 : NSFileCoordinatorReadingWithoutChanges;

        auto* fileAccessIntent = isWriting
                               ? [NSFileAccessIntent writingIntentWithURL: url options: accessOptions]
                               : [NSFileAccessIntent readingIntentWithURL: url options: accessOptions];

        NSArray<NSFileAccessIntent*>* intents = @[fileAccessIntent];

        auto fileCoordinator = [[NSFileCoordinator alloc] initWithFilePresenter: nil];

        [fileCoordinator coordinateAccessWithIntents: intents queue: [NSOperationQueue mainQueue] byAccessor: ^(NSError* err)
        {
            Array<URL> chooserResults;

            if (err == nil)
            {
                [url startAccessingSecurityScopedResource];

                NSError* error = nil;

                NSData* bookmark = [url bookmarkDataWithOptions: 0
                                 includingResourceValuesForKeys: nil
                                                  relativeToURL: nil
                                                          error: &error];

                [bookmark retain];

                [url stopAccessingSecurityScopedResource];

                URL HUrl (nsStringToJuce ([url absoluteString]));

                if (error == nil)
                {
                    setURLBookmark (HUrl, (void*) bookmark);
                }
                else
                {
                    auto desc = [error localizedDescription];
                    ignoreUnused (desc);
                    HAssertfalse;
                }

                chooserResults.add (HUrl);
            }
            else
            {
                auto desc = [err localizedDescription];
                ignoreUnused (desc);
                HAssertfalse;
            }

            owner.finished (chooserResults);
        }];
    }

    void pickerWasCancelled()
    {
        Array<URL> chooserResults;

        owner.finished (chooserResults);
        exitModalState (0);
    }

    //==============================================================================
    struct FileChooserDelegateClass  : public ObjCClass<NSObject<UIDocumentPickerDelegate>>
    {
        FileChooserDelegateClass()  : ObjCClass<NSObject<UIDocumentPickerDelegate>> ("FileChooserDelegate_")
        {
            addIvar<Native*> ("owner");

            addMethod (@selector (documentPicker:didPickDocumentAtURL:), didPickDocumentAtURL,       "v@:@@");
            addMethod (@selector (documentPickerWasCancelled:),          documentPickerWasCancelled, "v@:@");

            addProtocol (@protocol (UIDocumentPickerDelegate));

            registerClass();
        }

        static void setOwner (id self, Native* owner)   { object_setInstanceVariable (self, "owner", owner); }
        static Native* getOwner (id self)               { return getIvar<Native*> (self, "owner"); }

        //==============================================================================
        static void didPickDocumentAtURL (id self, SEL, UIDocumentPickerViewController*, NSURL* url)
        {
            auto picker = getOwner (self);

            if (picker != nullptr)
                picker->didPickDocumentAtURL (url);
        }

        static void documentPickerWasCancelled (id self, SEL, UIDocumentPickerViewController*)
        {
            auto picker = getOwner (self);

            if (picker != nullptr)
                picker->pickerWasCancelled();
        }
    };

    //==============================================================================
    FileChooser& owner;
    std::unique_ptr<NSObject<UIDocumentPickerDelegate>, NSObjectDeleter> delegate;
    std::unique_ptr<UIDocumentPickerViewController,     NSObjectDeleter> controller;
    UIViewComponentPeer* peer = nullptr;

    static FileChooserDelegateClass fileChooserDelegateClass;

    //==============================================================================
    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Native)
};

//==============================================================================
bool FileChooser::isPlatformDialogAvailable()
{
   #if HDISABLE_NATIVE_FILECHOOSERS
    return false;
   #else
    return true;
   #endif
}

FileChooser::Pimpl* FileChooser::showPlatformDialog (FileChooser& owner, int flags,
                                                     FilePreviewComponent*)
{
    return new FileChooser::Native (owner, flags);
}


