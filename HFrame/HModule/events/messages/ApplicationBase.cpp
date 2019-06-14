

HApplicationBase::CreateInstanceFunction HApplicationBase::createInstance = nullptr;
HApplicationBase* HApplicationBase::appInstance = nullptr;

#if HIOS
void* HApplicationBase::iOSCustomDelegate = nullptr;
#endif

HApplicationBase::HApplicationBase()
{
    HAssert (isStandaloneApp() && appInstance == nullptr);
    appInstance = this;
}

HApplicationBase::~HApplicationBase()
{
    HAssert (appInstance == this);
    appInstance = nullptr;
}

void HApplicationBase::setApplicationReturnValue (const int newReturnValue) noexcept
{
    appReturnValue = newReturnValue;
}

// This is called on the Mac and iOS where the OS doesn't allow the stack to unwind on shutdown..
void HApplicationBase::appWillTerminateByForce()
{
    HAUTORELEASEPOOL
    {
        {
            const std::unique_ptr<HApplicationBase> app (appInstance);

            if (app != nullptr)
                app->shutdownApp();
        }

        DeletedAtShutdown::deleteAll();
        MessageManager::deleteInstance();
    }
}

void HApplicationBase::quit()
{
    MessageManager::getInstance()->stopDispatchLoop();
}

void HApplicationBase::sendUnhandledException (const std::exception* const e,
                                                  const char* const sourceFile,
                                                  const int lineNumber)
{
    if (auto* app = HApplicationBase::getInstance())
    {
        // If you hit this assertion then the __FILE__ macro is providing a
        // relative path instead of an absolute path. On Windows this will be
        // a path relative to the build directory rather than the currently
        // running application. To fix this you must compile with the /FC flag.
        HAssert (File::isAbsolutePath (sourceFile));

        app->unhandledException (e, sourceFile, lineNumber);
    }
}

//==============================================================================
#if ! (HIOS || HANDROID)
 #define HHANDLE_MULTIPLE_INSTANCES 1
#endif

#if HHANDLE_MULTIPLE_INSTANCES
struct HApplicationBase::MultipleInstanceHandler  : public ActionListener
{
    MultipleInstanceHandler (const String& appName)
        : appLock ("HAppLock_" + appName)
    {
    }

    bool sendCommandLineToPreexistingInstance()
    {
        if (appLock.enter (0))
            return false;

        if (auto* app = HApplicationBase::getInstance())
        {
            MessageManager::broadcastMessage (app->getApplicationName() + "/" + app->getCommandLineParameters());
            return true;
        }

        HAssertfalse;
        return false;
    }

    void actionListenerCallback (const String& message) override
    {
        if (auto* app = HApplicationBase::getInstance())
        {
            auto appName = app->getApplicationName();

            if (message.startsWith (appName + "/"))
                app->anotherInstanceStarted (message.substr (appName.length() + 1));
        }
    }

private:
    InterProcessLock appLock;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultipleInstanceHandler)
};

bool HApplicationBase::sendCommandLineToPreexistingInstance()
{
    HAssert (multipleInstanceHandler == nullptr); // this must only be called once!

    multipleInstanceHandler.reset (new MultipleInstanceHandler (getApplicationName()));
    return multipleInstanceHandler->sendCommandLineToPreexistingInstance();
}

#else
struct HApplicationBase::MultipleInstanceHandler {};
#endif

//==============================================================================
#if HANDROID

StringArray HApplicationBase::getCommandLineParameterArray() { return {}; }
String HApplicationBase::getCommandLineParameters()          { return {}; }

#else

#if HWINDOWS && ! defined (_CONSOLE)

String CALLTYPE HApplicationBase::getCommandLineParameters()
{
    return CharacterFunctions::findEndOfToken (CharPointer_UTF16 (GetCommandLineW()),
                                               CharPointer_UTF16 (L" "),
                                               CharPointer_UTF16 (L"\"")).findEndOfWhitespace();
}

StringArray CALLTYPE HApplicationBase::getCommandLineParameterArray()
{
    StringArray s;
    int argc = 0;

    if (auto argv = CommandLineToArgvW (GetCommandLineW(), &argc))
    {
        s = StringArray (argv + 1, argc - 1);
        LocalFree (argv);
    }

    return s;
}

#else

#if HIOS
 extern int iOSMain (int argc, const char* argv[], void* classPtr);
#endif

#if HMAC
 extern void initialiseNSApplication();
#endif

#if HLINUX && (! defined(HWEB_BROWSER) || HWEB_BROWSER)
 extern int gtkWebkitMain (int argc, const char* argv[]);
#endif

#if HWINDOWS
 const char* const* argv = nullptr;
 int argc = 0;
#else
 extern const char* const* argv;  // declared in core
 extern int argc;
#endif

String HApplicationBase::getCommandLineParameters()
{
    String argString;

    for (int i = 1; i < argc; ++i)
    {
        String arg (argv[i]);

        if (arg.containsChar (' ') && ! arg.isQuotedString())
            arg = arg.quoted ('"');

        argString << arg << ' ';
    }

    return argString.trim();
}

StringArray HApplicationBase::getCommandLineParameterArray()
{
    return StringArray (argv + 1, argc - 1);
}

int HApplicationBase::main (int argc, const char* argv[])
{
    HAUTORELEASEPOOL
    {
        argc = argc;
        argv = argv;

       #if HMAC
        initialiseNSApplication();
       #endif

       #if HLINUX && (! defined(HWEB_BROWSER) || HWEB_BROWSER)
        if (argc >= 2 && String (argv[1]) == "--H-gtkwebkitfork-child")
            return gtkWebkitMain (argc, argv);
       #endif

       #if HIOS
        return iOSMain (argc, argv, iOSCustomDelegate);
       #else

        return HApplicationBase::main();
       #endif
    }
}

#endif

//==============================================================================
int HApplicationBase::main()
{
    ScopedJuceInitialiser_GUI libraryInitialiser;
    HAssert (createInstance != nullptr);

    const std::unique_ptr<HApplicationBase> app (createInstance());
    HAssert (app != nullptr);

    if (! app->initialiseApp())
        return app->shutdownApp();

    HTRY
    {
        // loop until a quit message is received..
        MessageManager::getInstance()->runDispatchLoop();
    }
    HCATCH_EXCEPTION

    return app->shutdownApp();
}

#endif

//==============================================================================
bool HApplicationBase::initialiseApp()
{
   #if HHANDLE_MULTIPLE_INSTANCES
    if ((! moreThanOneInstanceAllowed()) && sendCommandLineToPreexistingInstance())
    {
        DBG ("Another instance is running - quitting...");
        return false;
    }
   #endif

   #if HWINDOWS && HSTANDALONE_APPLICATION && (! defined (_CONSOLE)) && (! HMINGW)
    if (AttachConsole (ATTACH_PARENT_PROCESS) != 0)
    {
        // if we've launched a GUI app from cmd.exe or PowerShell, we need this to enable printf etc.
        // However, only reassign stdout, stderr, stdin if they have not been already opened by
        // a redirect or similar.
        FILE* ignore;

        if (_fileno(stdout) < 0) freopen_s (&ignore, "CONOUT$", "w", stdout);
        if (_fileno(stderr) < 0) freopen_s (&ignore, "CONOUT$", "w", stderr);
        if (_fileno(stdin)  < 0) freopen_s (&ignore, "CONIN$",  "r", stdin);
    }
   #endif

    // let the app do its setting-up..
    initialise (getCommandLineParameters());

    stillInitialising = false;

    if (MessageManager::getInstance()->hasStopMessageBeenSent())
        return false;

   #if HHANDLE_MULTIPLE_INSTANCES
    if (auto* mih = multipleInstanceHandler.get())
        MessageManager::getInstance()->registerBroadcastListener (mih);
   #endif

    return true;
}

int HApplicationBase::shutdownApp()
{
    HAssert (HApplicationBase::getInstance() == this);

   #if HHANDLE_MULTIPLE_INSTANCES
    if (auto* mih = multipleInstanceHandler.get())
        MessageManager::getInstance()->deregisterBroadcastListener (mih);
   #endif

    HTRY
    {
        // give the app a chance to clean up..
        shutdown();
    }
    HCATCH_EXCEPTION

    multipleInstanceHandler.reset();
    return getApplicationReturnValue();
}


