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

namespace H
{
    extern bool isIOSAppActive;

    struct AppInactivityCallback // NB: careful, this declaration is duplicated in other modules
    {
        virtual ~AppInactivityCallback() {}
        virtual void appBecomingInactive() = 0;
    };

    // This is an internal list of callbacks (but currently used between modules)
    Array<AppInactivityCallback*> appBecomingInactiveCallbacks;
}

#if HPUSH_NOTIFICATIONS && defined (__IPHONE_10_0) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
@interface JuceAppStartupDelegate : NSObject <UIApplicationDelegate, UNUserNotificationCenterDelegate>
#else
@interface JuceAppStartupDelegate : NSObject <UIApplicationDelegate>
#endif
{
    UIBackgroundTaskIdentifier appSuspendTask;
}

@property (strong, nonatomic) UIWindow *window;
- (id) init;
- (void) dealloc;
- (void) applicationDidFinishLaunching: (UIApplication*) application;
- (void) applicationWillTerminate: (UIApplication*) application;
- (void) applicationDidEnterBackground: (UIApplication*) application;
- (void) applicationWillEnterForeground: (UIApplication*) application;
- (void) applicationDidBecomeActive: (UIApplication*) application;
- (void) applicationWillResignActive: (UIApplication*) application;
- (void) application: (UIApplication*) application handleEventsForBackgroundURLSession: (NSString*) identifier
   completionHandler: (void (^)(void)) completionHandler;
- (void) applicationDidReceiveMemoryWarning: (UIApplication *) application;
#if HPUSH_NOTIFICATIONS
- (void) application: (UIApplication*) application didRegisterUserNotificationSettings: (UIUserNotificationSettings*) notificationSettings;
- (void) application: (UIApplication*) application didRegisterForRemoteNotificationsWithDeviceToken: (NSData*) deviceToken;
- (void) application: (UIApplication*) application didFailToRegisterForRemoteNotificationsWithError: (NSError*) error;
- (void) application: (UIApplication*) application didReceiveRemoteNotification: (NSDictionary*) userInfo;
- (void) application: (UIApplication*) application didReceiveRemoteNotification: (NSDictionary*) userInfo
  fetchCompletionHandler: (void (^)(UIBackgroundFetchResult result)) completionHandler;
- (void) application: (UIApplication*) application handleActionWithIdentifier: (NSString*) identifier
  forRemoteNotification: (NSDictionary*) userInfo withResponseInfo: (NSDictionary*) responseInfo
   completionHandler: (void(^)()) completionHandler;
- (void) application: (UIApplication*) application didReceiveLocalNotification: (UILocalNotification*) notification;
- (void) application: (UIApplication*) application handleActionWithIdentifier: (NSString*) identifier
  forLocalNotification: (UILocalNotification*) notification completionHandler: (void(^)()) completionHandler;
- (void) application: (UIApplication*) application handleActionWithIdentifier: (NSString*) identifier
  forLocalNotification: (UILocalNotification*) notification withResponseInfo: (NSDictionary*) responseInfo
   completionHandler: (void(^)()) completionHandler;
#if defined (__IPHONE_10_0) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
- (void) userNotificationCenter: (UNUserNotificationCenter*) center willPresentNotification: (UNNotification*) notification
          withCompletionHandler: (void (^)(UNNotificationPresentationOptions options)) completionHandler;
- (void) userNotificationCenter: (UNUserNotificationCenter*) center didReceiveNotificationResponse: (UNNotificationResponse*) response
          withCompletionHandler: (void(^)())completionHandler;
#endif
#endif

@end

@implementation JuceAppStartupDelegate

    NSObject* _pushNotificationsDelegate;

- (id) init
{
    self = [super init];
    appSuspendTask = UIBackgroundTaskInvalid;

   #if HPUSH_NOTIFICATIONS && defined (__IPHONE_10_0) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
    [UNUserNotificationCenter currentNotificationCenter].delegate = self;
   #endif

    return self;
}

- (void) dealloc
{
    [super dealloc];
}

- (void) applicationDidFinishLaunching: (UIApplication*) application
{
    ignoreUnused (application);
    initialiseJuce_GUI();

    if (auto* app = HApplicationBase::createInstance())
    {
        if (! app->initialiseApp())
            exit (app->shutdownApp());
    }
    else
    {
        HAssertfalse; // you must supply an application object for an iOS app!
    }
}

- (void) applicationWillTerminate: (UIApplication*) application
{
    ignoreUnused (application);
    HApplicationBase::appWillTerminateByForce();
}

- (void) applicationDidEnterBackground: (UIApplication*) application
{
    if (auto* app = HApplicationBase::getInstance())
    {
       #if HEXECUTE_APP_SUSPEND_ON_BACKGROUND_TASK
        appSuspendTask = [application beginBackgroundTaskWithName:@"H Suspend Task" expirationHandler:^{
            if (appSuspendTask != UIBackgroundTaskInvalid)
            {
                [application endBackgroundTask:appSuspendTask];
                appSuspendTask = UIBackgroundTaskInvalid;
            }
        }];

        MessageManager::callAsync ([app] { app->suspended(); });
       #else
        ignoreUnused (application);
        app->suspended();
       #endif
    }
}

- (void) applicationWillEnterForeground: (UIApplication*) application
{
    ignoreUnused (application);

    if (auto* app = HApplicationBase::getInstance())
        app->resumed();
}

- (void) applicationDidBecomeActive: (UIApplication*) application
{
    application.applicationIconBadgeNumber = 0;

    isIOSAppActive = true;
}

- (void) applicationWillResignActive: (UIApplication*) application
{
    ignoreUnused (application);
    isIOSAppActive = false;

    for (int i = appBecomingInactiveCallbacks.size(); --i >= 0;)
        appBecomingInactiveCallbacks.getReference(i)->appBecomingInactive();
}

- (void) application: (UIApplication*) application handleEventsForBackgroundURLSession: (NSString*)identifier
   completionHandler: (void (^)(void))completionHandler
{
    ignoreUnused (application);
    URL::DownloadTask::iosURLSessionNotify (nsStringToJuce (identifier));
    completionHandler();
}

- (void) applicationDidReceiveMemoryWarning: (UIApplication*) application
{
    ignoreUnused (application);

    if (auto* app = HApplicationBase::getInstance())
        app->memoryWarningReceived();
}

- (void) setPushNotificationsDelegateToUse: (NSObject*) delegate
{
    _pushNotificationsDelegate = delegate;
}

#if HPUSH_NOTIFICATIONS
- (void) application: (UIApplication*) application didRegisterUserNotificationSettings: (UIUserNotificationSettings*) notificationSettings
{
    ignoreUnused (application);

    SEL selector = NSSelectorFromString (@"application:didRegisterUserNotificationSettings:");

    if (_pushNotificationsDelegate != nil && [_pushNotificationsDelegate respondsToSelector: selector])
    {
        NSInvocation* invocation = [NSInvocation invocationWithMethodSignature: [_pushNotificationsDelegate methodSignatureForSelector: selector]];
        [invocation setSelector: selector];
        [invocation setTarget: _pushNotificationsDelegate];
        [invocation setArgument: &application          atIndex:2];
        [invocation setArgument: &notificationSettings atIndex:3];

        [invocation invoke];
    }
}

- (void) application: (UIApplication*) application didRegisterForRemoteNotificationsWithDeviceToken: (NSData*) deviceToken
{
    ignoreUnused (application);

    SEL selector = NSSelectorFromString (@"application:didRegisterForRemoteNotificationsWithDeviceToken:");

    if (_pushNotificationsDelegate != nil && [_pushNotificationsDelegate respondsToSelector: selector])
    {
        NSInvocation* invocation = [NSInvocation invocationWithMethodSignature: [_pushNotificationsDelegate methodSignatureForSelector: selector]];
        [invocation setSelector: selector];
        [invocation setTarget: _pushNotificationsDelegate];
        [invocation setArgument: &application atIndex:2];
        [invocation setArgument: &deviceToken atIndex:3];

        [invocation invoke];
    }
}

- (void) application: (UIApplication*) application didFailToRegisterForRemoteNotificationsWithError: (NSError*) error
{
    ignoreUnused (application);

    SEL selector = NSSelectorFromString (@"application:didFailToRegisterForRemoteNotificationsWithError:");

    if (_pushNotificationsDelegate != nil && [_pushNotificationsDelegate respondsToSelector: selector])
    {
        NSInvocation* invocation = [NSInvocation invocationWithMethodSignature: [_pushNotificationsDelegate methodSignatureForSelector: selector]];
        [invocation setSelector: selector];
        [invocation setTarget: _pushNotificationsDelegate];
        [invocation setArgument: &application atIndex:2];
        [invocation setArgument: &error       atIndex:3];

        [invocation invoke];
    }
}

- (void) application: (UIApplication*) application didReceiveRemoteNotification: (NSDictionary*) userInfo
{
    ignoreUnused (application);

    SEL selector = NSSelectorFromString (@"application:didReceiveRemoteNotification:");

    if (_pushNotificationsDelegate != nil && [_pushNotificationsDelegate respondsToSelector: selector])
    {
        NSInvocation* invocation = [NSInvocation invocationWithMethodSignature: [_pushNotificationsDelegate methodSignatureForSelector: selector]];
        [invocation setSelector: selector];
        [invocation setTarget: _pushNotificationsDelegate];
        [invocation setArgument: &application atIndex:2];
        [invocation setArgument: &userInfo    atIndex:3];

        [invocation invoke];
    }
}

- (void) application: (UIApplication*) application didReceiveRemoteNotification: (NSDictionary*) userInfo
  fetchCompletionHandler: (void (^)(UIBackgroundFetchResult result)) completionHandler
{
    ignoreUnused (application);

    SEL selector = NSSelectorFromString (@"application:didReceiveRemoteNotification:fetchCompletionHandler:");

    if (_pushNotificationsDelegate != nil && [_pushNotificationsDelegate respondsToSelector: selector])
    {
        NSInvocation* invocation = [NSInvocation invocationWithMethodSignature: [_pushNotificationsDelegate methodSignatureForSelector: selector]];
        [invocation setSelector: selector];
        [invocation setTarget: _pushNotificationsDelegate];
        [invocation setArgument: &application       atIndex:2];
        [invocation setArgument: &userInfo          atIndex:3];
        [invocation setArgument: &completionHandler atIndex:4];

        [invocation invoke];
    }
}

- (void) application: (UIApplication*) application handleActionWithIdentifier: (NSString*) identifier
  forRemoteNotification: (NSDictionary*) userInfo withResponseInfo: (NSDictionary*) responseInfo
  completionHandler: (void(^)()) completionHandler
{
    ignoreUnused (application);

    SEL selector = NSSelectorFromString (@"application:handleActionWithIdentifier:forRemoteNotification:withResponseInfo:completionHandler:");

    if (_pushNotificationsDelegate != nil && [_pushNotificationsDelegate respondsToSelector: selector])
    {
        NSInvocation* invocation = [NSInvocation invocationWithMethodSignature: [_pushNotificationsDelegate methodSignatureForSelector: selector]];
        [invocation setSelector: selector];
        [invocation setTarget: _pushNotificationsDelegate];
        [invocation setArgument: &application       atIndex:2];
        [invocation setArgument: &identifier        atIndex:3];
        [invocation setArgument: &userInfo          atIndex:4];
        [invocation setArgument: &responseInfo      atIndex:5];
        [invocation setArgument: &completionHandler atIndex:6];

        [invocation invoke];
    }
}

- (void) application: (UIApplication*) application didReceiveLocalNotification: (UILocalNotification*) notification
{
    ignoreUnused (application);

    SEL selector = NSSelectorFromString (@"application:didReceiveLocalNotification:");

    if (_pushNotificationsDelegate != nil && [_pushNotificationsDelegate respondsToSelector: selector])
    {
        NSInvocation* invocation = [NSInvocation invocationWithMethodSignature: [_pushNotificationsDelegate methodSignatureForSelector: selector]];
        [invocation setSelector: selector];
        [invocation setTarget: _pushNotificationsDelegate];
        [invocation setArgument: &application  atIndex:2];
        [invocation setArgument: &notification atIndex:3];

        [invocation invoke];
    }
}

- (void) application: (UIApplication*) application handleActionWithIdentifier: (NSString*) identifier
  forLocalNotification: (UILocalNotification*) notification completionHandler: (void(^)()) completionHandler
{
    ignoreUnused (application);

    SEL selector = NSSelectorFromString (@"application:handleActionWithIdentifier:forLocalNotification:completionHandler:");

    if (_pushNotificationsDelegate != nil && [_pushNotificationsDelegate respondsToSelector: selector])
    {
        NSInvocation* invocation = [NSInvocation invocationWithMethodSignature: [_pushNotificationsDelegate methodSignatureForSelector: selector]];
        [invocation setSelector: selector];
        [invocation setTarget: _pushNotificationsDelegate];
        [invocation setArgument: &application       atIndex:2];
        [invocation setArgument: &identifier        atIndex:3];
        [invocation setArgument: &notification      atIndex:4];
        [invocation setArgument: &completionHandler atIndex:5];

        [invocation invoke];
    }
}

- (void) application: (UIApplication*) application handleActionWithIdentifier: (NSString*) identifier
  forLocalNotification: (UILocalNotification*) notification withResponseInfo: (NSDictionary*) responseInfo
  completionHandler: (void(^)()) completionHandler
{
    ignoreUnused (application);

    SEL selector = NSSelectorFromString (@"application:handleActionWithIdentifier:forLocalNotification:withResponseInfo:completionHandler:");

    if (_pushNotificationsDelegate != nil && [_pushNotificationsDelegate respondsToSelector: selector])
    {
        NSInvocation* invocation = [NSInvocation invocationWithMethodSignature: [_pushNotificationsDelegate methodSignatureForSelector: selector]];
        [invocation setSelector: selector];
        [invocation setTarget: _pushNotificationsDelegate];
        [invocation setArgument: &application       atIndex:2];
        [invocation setArgument: &identifier        atIndex:3];
        [invocation setArgument: &notification      atIndex:4];
        [invocation setArgument: &responseInfo      atIndex:5];
        [invocation setArgument: &completionHandler atIndex:6];

        [invocation invoke];
    }
}

#if defined (__IPHONE_10_0) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
- (void) userNotificationCenter: (UNUserNotificationCenter*) center willPresentNotification: (UNNotification*) notification
         withCompletionHandler: (void (^)(UNNotificationPresentationOptions options)) completionHandler
{
    ignoreUnused (center);

    SEL selector = NSSelectorFromString (@"userNotificationCenter:willPresentNotification:withCompletionHandler:");

    if (_pushNotificationsDelegate != nil && [_pushNotificationsDelegate respondsToSelector: selector])
    {
        NSInvocation* invocation = [NSInvocation invocationWithMethodSignature: [_pushNotificationsDelegate methodSignatureForSelector: selector]];
        [invocation setSelector: selector];
        [invocation setTarget: _pushNotificationsDelegate];
        [invocation setArgument: &center            atIndex:2];
        [invocation setArgument: &notification      atIndex:3];
        [invocation setArgument: &completionHandler atIndex:4];

        [invocation invoke];
    }
}

- (void) userNotificationCenter: (UNUserNotificationCenter*) center didReceiveNotificationResponse: (UNNotificationResponse*) response
         withCompletionHandler: (void(^)()) completionHandler
{
    ignoreUnused (center);

    SEL selector = NSSelectorFromString (@"userNotificationCenter:didReceiveNotificationResponse:withCompletionHandler:");

    if (_pushNotificationsDelegate != nil && [_pushNotificationsDelegate respondsToSelector: selector])
    {
        NSInvocation* invocation = [NSInvocation invocationWithMethodSignature: [_pushNotificationsDelegate methodSignatureForSelector: selector]];
        [invocation setSelector: selector];
        [invocation setTarget: _pushNotificationsDelegate];
        [invocation setArgument: &center            atIndex:2];
        [invocation setArgument: &response          atIndex:3];
        [invocation setArgument: &completionHandler atIndex:4];

        [invocation invoke];
    }
}
#endif
#endif

@end

int iOSMain (int argc, const char* argv[], void* customDelegatePtr);
int iOSMain (int argc, const char* argv[], void* customDelegatePtr)
{
    Class delegateClass = (customDelegatePtr != nullptr ? reinterpret_cast<Class> (customDelegatePtr) : [JuceAppStartupDelegate class]);

    return UIApplicationMain (argc, const_cast<char**> (argv), nil, NSStringFromClass (delegateClass));
}

//==============================================================================
void LookAndFeel::playAlertSound()
{
    // TODO
}

//==============================================================================
class iOSMessageBox;

#if defined (__IPHONE_8_0) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_8_0
 #define HUSE_NEW_IOS_ALERTWINDOW 1
#endif

#if ! HUSE_NEW_IOS_ALERTWINDOW
    } // (H namespace)

    @interface JuceAlertBoxDelegate  : NSObject <UIAlertViewDelegate>
    {
    @public
        iOSMessageBox* owner;
    }

    - (void) alertView: (UIAlertView*) alertView clickedButtonAtIndex: (NSInteger) buttonIndex;

    @end

    namespace H
    {
#endif


class iOSMessageBox
{
public:
    iOSMessageBox (const String& title, const String& message,
                   NSString* button1, NSString* button2, NSString* button3,
                   ModalComponentManager::Callback* cb, const bool async)
        : result (0), resultReceived (false), callback (cb), isAsync (async)
    {
       #if HUSE_NEW_IOS_ALERTWINDOW
        if (currentlyFocusedPeer != nullptr)
        {
            UIAlertController* alert = [UIAlertController alertControllerWithTitle: HStringToNS (title)
                                                                           message: HStringToNS (message)
                                                                    preferredStyle: UIAlertControllerStyleAlert];
            addButton (alert, button1, 0);
            addButton (alert, button2, 1);
            addButton (alert, button3, 2);

            [currentlyFocusedPeer->controller presentViewController: alert
                                                           animated: YES
                                                         completion: nil];
        }
        else
        {
            // Since iOS8, alert windows need to be associated with a window, so you need to
            // have at least one window on screen when you use this
            HAssertfalse;
        }

       #else
        delegate = [[JuceAlertBoxDelegate alloc] init];
        delegate->owner = this;

        alert = [[UIAlertView alloc] initWithTitle: HStringToNS (title)
                                           message: HStringToNS (message)
                                          delegate: delegate
                                 cancelButtonTitle: button1
                                 otherButtonTitles: button2, button3, nil];
        [alert retain];
        [alert show];
       #endif
    }

    ~iOSMessageBox()
    {
       #if ! HUSE_NEW_IOS_ALERTWINDOW
        [alert release];
        [delegate release];
       #endif
    }

    int getResult()
    {
        HAssert (callback == nullptr);

        HAUTORELEASEPOOL
        {
           #if HUSE_NEW_IOS_ALERTWINDOW
            while (! resultReceived)
           #else
            while (! (alert.hidden || resultReceived))
           #endif
                [[NSRunLoop mainRunLoop] runUntilDate: [NSDate dateWithTimeIntervalSinceNow: 0.01]];
        }

        return result;
    }

    void buttonClicked (const int buttonIndex) noexcept
    {
        result = buttonIndex;
        resultReceived = true;

        if (callback != nullptr)
            callback->modalStateFinished (result);

        if (isAsync)
            delete this;
    }

private:
    int result;
    bool resultReceived;
    std::unique_ptr<ModalComponentManager::Callback> callback;
    const bool isAsync;

   #if HUSE_NEW_IOS_ALERTWINDOW
    void addButton (UIAlertController* alert, NSString* text, int index)
    {
        if (text != nil)
            [alert addAction: [UIAlertAction actionWithTitle: text
                                                       style: UIAlertActionStyleDefault
                                                     handler: ^(UIAlertAction*) { this->buttonClicked (index); }]];
    }
   #else
    UIAlertView* alert;
    JuceAlertBoxDelegate* delegate;
   #endif

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (iOSMessageBox)
};


#if ! HUSE_NEW_IOS_ALERTWINDOW
    } // (H namespace)

    @implementation JuceAlertBoxDelegate

    - (void) alertView: (UIAlertView*) alertView clickedButtonAtIndex: (NSInteger) buttonIndex
    {
        owner->buttonClicked ((int) buttonIndex);
        alertView.hidden = true;
    }

    @end

    namespace H
    {
#endif


//==============================================================================
#if HMODAL_LOOPS_PERMITTED
void CALLTYPE NativeMessageBox::showMessageBox (AlertWindow::AlertIconType /*iconType*/,
                                                     const String& title, const String& message,
                                                     Component* /*associatedComponent*/)
{
    HAUTORELEASEPOOL
    {
        iOSMessageBox mb (title, message, @"OK", nil, nil, nullptr, false);
        ignoreUnused (mb.getResult());
    }
}
#endif

void CALLTYPE NativeMessageBox::showMessageBoxAsync (AlertWindow::AlertIconType /*iconType*/,
                                                          const String& title, const String& message,
                                                          Component* /*associatedComponent*/,
                                                          ModalComponentManager::Callback* callback)
{
    new iOSMessageBox (title, message, @"OK", nil, nil, callback, true);
}

bool CALLTYPE NativeMessageBox::showOkCancelBox (AlertWindow::AlertIconType /*iconType*/,
                                                      const String& title, const String& message,
                                                      Component* /*associatedComponent*/,
                                                      ModalComponentManager::Callback* callback)
{
    std::unique_ptr<iOSMessageBox> mb (new iOSMessageBox (title, message, @"Cancel", @"OK",
                                                          nil, callback, callback != nullptr));

    if (callback == nullptr)
        return mb->getResult() == 1;

    mb.release();
    return false;
}

int CALLTYPE NativeMessageBox::showYesNoCancelBox (AlertWindow::AlertIconType /*iconType*/,
                                                        const String& title, const String& message,
                                                        Component* /*associatedComponent*/,
                                                        ModalComponentManager::Callback* callback)
{
    std::unique_ptr<iOSMessageBox> mb (new iOSMessageBox (title, message, @"Cancel", @"Yes", @"No", callback, callback != nullptr));

    if (callback == nullptr)
        return mb->getResult();

    mb.release();
    return 0;
}

int CALLTYPE NativeMessageBox::showYesNoBox (AlertWindow::AlertIconType /*iconType*/,
                                                  const String& title, const String& message,
                                                  Component* /*associatedComponent*/,
                                                  ModalComponentManager::Callback* callback)
{
    std::unique_ptr<iOSMessageBox> mb (new iOSMessageBox (title, message, @"No", @"Yes", nil, callback, callback != nullptr));

    if (callback == nullptr)
        return mb->getResult();

    mb.release();
    return 0;
}

//==============================================================================
bool DragAndDropContainer::performExternalDragDropOfFiles (const StringArray&, bool, Component*, std::function<void()>)
{
    HAssertfalse;    // no such thing on iOS!
    return false;
}

bool DragAndDropContainer::performExternalDragDropOfText (const String&, Component*, std::function<void()>)
{
    HAssertfalse;    // no such thing on iOS!
    return false;
}

//==============================================================================
void Desktop::setScreenSaverEnabled (const bool isEnabled)
{
    if (! SystemStats::isRunningInAppExtensionSandbox())
        [[UIApplication sharedApplication] setIdleTimerDisabled: ! isEnabled];
}

bool Desktop::isScreenSaverEnabled()
{
    if (SystemStats::isRunningInAppExtensionSandbox())
        return true;

    return ! [[UIApplication sharedApplication] isIdleTimerDisabled];
}

//==============================================================================
bool areThereAnyAlwaysOnTopWindows()
{
    return false;
}

//==============================================================================
Image createIconForFile (const File&)
{
    return Image();
}

//==============================================================================
void SystemClipboard::copyTextToClipboard (const String& text)
{
    [[UIPasteboard generalPasteboard] setValue: HStringToNS (text)
                             forPasteboardType: @"public.text"];
}

String SystemClipboard::getTextFromClipboard()
{
    return nsStringToJuce ([[UIPasteboard generalPasteboard] valueForPasteboardType: @"public.text"]);
}

//==============================================================================
bool MouseInputSource::SourceList::addSource()
{
    addSource (sources.size(), MouseInputSource::InputSourceType::touch);
    return true;
}

bool MouseInputSource::SourceList::canUseTouch()
{
    return true;
}

bool Desktop::canUseSemiTransparentWindows() noexcept
{
    return true;
}

Point<float> MouseInputSource::getCurrentRawMousePosition()
{
    return lastMousePos;
}

void MouseInputSource::setRawMousePosition (Point<float>)
{
}

double Desktop::getDefaultMasterScale()
{
    return 1.0;
}

Desktop::DisplayOrientation Desktop::getCurrentOrientation() const
{
    UIInterfaceOrientation orientation = SystemStats::isRunningInAppExtensionSandbox() ? UIInterfaceOrientationPortrait
                                                                                       : [[UIApplication sharedApplication] statusBarOrientation];

    return Orientations::convertToJuce (orientation);
}

void Displays::findDisplays (float masterScale)
{
    HAUTORELEASEPOOL
    {
        UIScreen* s = [UIScreen mainScreen];

        Display d;
        d.userArea = d.totalArea = UIViewComponentPeer::realScreenPosToRotated (convertToRectInt ([s bounds])) / masterScale;
        d.isMain = true;
        d.scale = masterScale;

        if ([s respondsToSelector: @selector (scale)])
            d.scale *= s.scale;

        d.dpi = 160 * d.scale;

        displays.add (d);
    }
}


