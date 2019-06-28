

//==============================================================================
/**
    Abstract base class for application classes.

    Note that in the gui_basics module, there's a utility class HApplication
    which derives from HApplicationBase, and takes care of a few chores. Most
    of the time you'll want to derive your class from HApplication rather than
    using HApplicationBase directly, but if you're not using the gui_basics
    module then you might need to go straight to this base class.

    Any application that wants to run an event loop must declare a subclass of
    HApplicationBase, and implement its various pure virtual methods.

    It then needs to use the START_HAPPLICATION macro somewhere in a CPP file
    to declare an instance of this class and generate suitable platform-specific
    boilerplate code to launch the app.

    e.g. @code
        class MyHApp  : public HApplication
        {
        public:
            MyHApp()  {}
            ~MyHApp() {}

            void initialise (const String& commandLine) override
            {
                myMainWindow.reset (new MyApplicationWindow());
                myMainWindow->setBounds (100, 100, 400, 500);
                myMainWindow->setVisible (true);
            }

            void shutdown() override
            {
                myMainWindow = nullptr;
            }

            const String getApplicationName() override
            {
                return "Super H-o-matic";
            }

            const String getApplicationVersion() override
            {
                return "1.0";
            }

        private:
            std::unique_ptr<MyApplicationWindow> myMainWindow;
        };

        // this generates boilerplate code to launch our app class:
        START_HAPPLICATION (MyHApp)
    @endcode

    @see HApplication, START_HAPPLICATION

    @tags{Events}
*/
class API HApplicationBase
{
protected:
    //==============================================================================
    HApplicationBase();

public:
    /** Destructor. */
    virtual ~HApplicationBase();

    //==============================================================================
    /** Returns the global instance of the application object that's running. */
    static HApplicationBase* getInstance() noexcept          { return appInstance; }

    //==============================================================================
    /** Returns the application's name. */
    virtual const String getApplicationName() = 0;

    /** Returns the application's version number. */
    virtual const String getApplicationVersion() = 0;

    /** Checks whether multiple instances of the app are allowed.

        If your application class returns true for this, more than one instance is
        permitted to run (except on the Mac where this isn't possible).

        If it's false, the second instance won't start, but you will still get a
        callback to anotherInstanceStarted() to tell you about this - which
        gives you a chance to react to what the user was trying to do.

        @see anotherInstanceStarted
    */
    virtual bool moreThanOneInstanceAllowed() = 0;

    /** Called when the application starts.

        This will be called once to let the application do whatever initialisation
        it needs, create its windows, etc.

        After the method returns, the normal event-dispatch loop will be run,
        until the quit() method is called, at which point the shutdown()
        method will be called to let the application clear up anything it needs
        to delete.

        If during the initialise() method, the application decides not to start-up
        after all, it can just call the quit() method and the event loop won't be run.

        @param commandLineParameters    the line passed in does not include the name of
                                        the executable, just the parameter list. To get the
                                        parameters as an array, you can call
                                        HApplication::getCommandLineParameters()
        @see shutdown, quit
    */
    virtual void initialise (const String& commandLineParameters) = 0;

    /* Called to allow the application to clear up before exiting.

       After HApplication::quit() has been called, the event-dispatch loop will
       terminate, and this method will get called to allow the app to sort itself
       out.

       Be careful that nothing happens in this method that might rely on messages
       being sent, or any kind of window activity, because the message loop is no
       longer running at this point.

        @see DeletedAtShutdown
    */
    virtual void shutdown() = 0;

    /** Indicates that the user has tried to start up another instance of the app.

        This will get called even if moreThanOneInstanceAllowed() is false.
        It is currently only implemented on Windows and Mac.

        @see moreThanOneInstanceAllowed
    */
    virtual void anotherInstanceStarted (const String& commandLine) = 0;

    /** Called when the operating system is trying to close the application.

        The default implementation of this method is to call quit(), but it may
        be overloaded to ignore the request or do some other special behaviour
        instead. For example, you might want to offer the user the chance to save
        their changes before quitting, and give them the chance to cancel.

        If you want to send a quit signal to your app, this is the correct method
        to call, because it means that requests that come from the system get handled
        in the same way as those from your own application code. So e.g. you'd
        call this method from a "quit" item on a menu bar.
    */
    virtual void systemRequestedQuit() = 0;

    /** This method is called when the application is being put into background mode
        by the operating system.
    */
    virtual void suspended() = 0;

    /** This method is called when the application is being woken from background mode
        by the operating system.
    */
    virtual void resumed() = 0;

    /** If any unhandled exceptions make it through to the message dispatch loop, this
        callback will be triggered, in case you want to log them or do some other
        type of error-handling.

        If the type of exception is derived from the std::exception class, the pointer
        passed-in will be valid. If the exception is of unknown type, this pointer
        will be null.
    */
    virtual void unhandledException (const std::exception*,
                                     const String& sourceFilename,
                                     int lineNumber) = 0;

    /** Called by the operating system to indicate that you should reduce your memory
        footprint.

        You should override this method to free up some memory gracefully, if possible,
        otherwise the host may forcibly kill your app.

        At the moment this method is only called on iOS.
    */
    virtual void memoryWarningReceived()     { HAssertfalse; }

    //==============================================================================
    /** Override this method to be informed when the back button is pressed on a device.
        This is currently only implemented on Android devices.
     */
    virtual void backButtonPressed() {}

    //==============================================================================
    /** Signals that the main message loop should stop and the application should terminate.

        This isn't synchronous, it just posts a quit message to the main queue, and
        when this message arrives, the message loop will stop, the shutdown() method
        will be called, and the app will exit.

        Note that this will cause an unconditional quit to happen, so if you need an
        extra level before this, e.g. to give the user the chance to save their work
        and maybe cancel the quit, you'll need to handle this in the systemRequestedQuit()
        method - see that method's help for more info.

        @see MessageManager
    */
    static void quit();

    //==============================================================================
    /** Returns the application's command line parameters as a set of strings.
        @see getCommandLineParameters
    */
    static StringArray CALLTYPE getCommandLineParameterArray();

    /** Returns the application's command line parameters as a single string.
        @see getCommandLineParameterArray
    */
    static String CALLTYPE getCommandLineParameters();

    //==============================================================================
    /** Sets the value that should be returned as the application's exit code when the
        app quits.

        This is the value that's returned by the main() function. Normally you'd leave this
        as 0 unless you want to indicate an error code.

        @see getApplicationReturnValue
    */
    void setApplicationReturnValue (int newReturnValue) noexcept;

    /** Returns the value that has been set as the application's exit code.
        @see setApplicationReturnValue
    */
    int getApplicationReturnValue() const noexcept              { return appReturnValue; }

    //==============================================================================
    /** Returns true if this executable is running as an app (as opposed to being a plugin
        or other kind of shared library. */
    static bool isStandaloneApp() noexcept                      { return createInstance != nullptr; }

    /** Returns true if the application hasn't yet completed its initialise() method
        and entered the main event loop.

        This is handy for things like splash screens to know when the app's up-and-running
        properly.
    */
    bool isInitialising() const noexcept                        { return stillInitialising; }


    //==============================================================================
   #ifndef DOXYGEN
    // The following methods are for internal use only...
    static int main();
    static int main (int argc, const char* argv[]);

    static void appWillTerminateByForce();
    using CreateInstanceFunction = HApplicationBase* (*)();
    static CreateInstanceFunction createInstance;

   #if HIOS
    static void* iOSCustomDelegate;
   #endif

    virtual bool initialiseApp();
    int shutdownApp();
    static void CALLTYPE sendUnhandledException (const std::exception*, const char* sourceFile, int lineNumber);
    bool sendCommandLineToPreexistingInstance();
   #endif

private:
    //==============================================================================
    static HApplicationBase* appInstance;
    int appReturnValue = 0;
    bool stillInitialising = true;

    struct MultipleInstanceHandler;
    std::unique_ptr<MultipleInstanceHandler> multipleInstanceHandler;

    HDECLARE_NON_COPYABLE (HApplicationBase)
};


//==============================================================================
#if HCATCH_UNHANDLED_EXCEPTIONS || defined (DOXYGEN)

 /** The HTRY/HCATCH_EXCEPTION wrappers can be used to pass any uncaught exceptions to
     the HApplicationBase::sendUnhandledException() method.
     This functionality can be enabled with the HCATCH_UNHANDLED_EXCEPTIONS macro.
 */
 #define HTRY try

 /** The HTRY/HCATCH_EXCEPTION wrappers can be used to pass any uncaught exceptions to
     the HApplicationBase::sendUnhandledException() method.
     This functionality can be enabled with the HCATCH_UNHANDLED_EXCEPTIONS macro.
 */
 #define HCATCH_EXCEPTION \
    catch (const std::exception& e) { HApplicationBase::sendUnhandledException (&e,      __FILE__, __LINE__); } \
    catch (...)                     { HApplicationBase::sendUnhandledException (nullptr, __FILE__, __LINE__); }

#else
 #define HTRY
 #define HCATCH_EXCEPTION
#endif


