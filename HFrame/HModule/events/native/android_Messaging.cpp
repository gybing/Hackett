

//==============================================================================
namespace Android
{
    class Runnable  : public AndroidInterfaceImplementer
    {
    public:
        virtual void run() = 0;

    private:
        jobject invoke (jobject proxy, jobject method, jobjectArray args) override
        {
            auto* env = getEnv();
            auto methodName = HString ((jstring) env->CallObjectMethod (method, JavaMethod.getName));

            if (methodName == "run")
            {
                run();
                return nullptr;
            }

            // invoke base class
            return AndroidInterfaceImplementer::invoke (proxy, method, args);
        }
    };

    struct Handler
    {
        Handler() : nativeHandler (LocalRef<jobject> (getEnv()->NewObject (AndroidHandler, AndroidHandler.constructor))) {}
        ~Handler() { clearSingletonInstance(); }

        HDECLARE_SINGLETON (Handler, false)

        bool post (jobject runnable)
        {
            return (getEnv()->CallBooleanMethod (nativeHandler.get(), AndroidHandler.post, runnable) != 0);
        }

        GlobalRef nativeHandler;
    };

    HIMPLEMENT_SINGLETON (Handler)
}

//==============================================================================
struct AndroidMessageQueue     : private Android::Runnable
{
    HDECLARE_SINGLETON_SINGLETHREADED (AndroidMessageQueue, true)

    AndroidMessageQueue()
        : self (CreateJavaInterface (this, "java/lang/Runnable"))
    {
    }

    ~AndroidMessageQueue()
    {
        HASSERT_MESSAGE_THREAD
        clearSingletonInstance();
    }

    bool post (MessageManager::MessageBase::Ptr&& message)
    {
        queue.add (std::move (message));

        // this will call us on the message thread
        return handler.post (self.get());
    }

private:

    void run() override
    {
        for (;;)
        {
            MessageManager::MessageBase::Ptr message (queue.removeAndReturn (0));

            if (message == nullptr)
                break;

            message->messageCallback();
        }
    }

    // the this pointer to this class in Java land
    GlobalRef self;

    ReferenceCountedArray<MessageManager::MessageBase, CriticalSection> queue;
    Android::Handler handler;
};

HIMPLEMENT_SINGLETON (AndroidMessageQueue)

//==============================================================================
void MessageManager::doPlatformSpecificInitialisation() { AndroidMessageQueue::getInstance(); }
void MessageManager::doPlatformSpecificShutdown()       { AndroidMessageQueue::deleteInstance(); }

//==============================================================================
bool MessageManager::dispatchNextMessageOnSystemQueue (const bool)
{
    Logger::outputDebugString ("*** Modal loops are not possible in Android!! Exiting...");
    exit (1);

    return true;
}

bool MessageManager::postMessageToSystemQueue (MessageManager::MessageBase* const message)
{
    return AndroidMessageQueue::getInstance()->post (message);
}

//==============================================================================
void MessageManager::broadcastMessage (const String&)
{
}

void MessageManager::runDispatchLoop()
{
}

void MessageManager::stopDispatchLoop()
{
    struct QuitCallback  : public CallbackMessage
    {
        QuitCallback() {}

        void messageCallback() override
        {
            auto* env = getEnv();
            LocalRef<jobject> activity (getCurrentActivity());

            if (activity != nullptr)
            {
                jmethodID quitMethod = env->GetMethodID (AndroidActivity, "finishAndRemoveTask", "()V");

                if (quitMethod != 0)
                {
                    env->CallVoidMethod (activity.get(), quitMethod);
                    return;
                }

                quitMethod = env->GetMethodID (AndroidActivity, "finish", "()V");
                HAssert (quitMethod != 0);
                env->CallVoidMethod (activity.get(), quitMethod);
            }
            else
            {
                HAssertfalse;
            }
        }
    };

    (new QuitCallback())->post();
    quitMessagePosted = true;
}

//==============================================================================
class JuceAppLifecycle   : public ActivityLifecycleCallbacks
{
public:
    JuceAppLifecycle (HApplicationBase* (*initSymbolAddr)())
        : createApplicationSymbol (initSymbolAddr)
    {
        LocalRef<jobject> appContext (getAppContext());

        if (appContext != nullptr)
        {
            auto* env = getEnv();

            myself = GlobalRef (CreateJavaInterface (this, "android/app/Application$ActivityLifecycleCallbacks"));
            env->CallVoidMethod (appContext.get(), AndroidApplication.registerActivityLifecycleCallbacks, myself.get());
        }
    }

    ~JuceAppLifecycle()
    {
        LocalRef<jobject> appContext (getAppContext());

        if (appContext != nullptr && myself != nullptr)
        {
            auto* env = getEnv();

            clear();
            env->CallVoidMethod (appContext.get(), AndroidApplication.unregisterActivityLifecycleCallbacks, myself.get());
            myself.clear();
        }
    }

    void onActivityCreated (jobject, jobject) override
    {
        checkCreated();
    }

    void onActivityDestroyed (jobject activity) override
    {
        auto* env = getEnv();

        // if the main activity is being destroyed, only then tear-down H
        if (env->IsSameObject (getMainActivity().get(), activity) != 0)
        {
            HApplicationBase::appWillTerminateByForce();
            JNIClassBase::releaseAllClasses (env);

            jclass systemClass = (jclass) env->FindClass ("java/lang/System");
            jmethodID exitMethod = env->GetStaticMethodID (systemClass, "exit", "(I)V");
            env->CallStaticVoidMethod (systemClass, exitMethod, 0);
        }
    }

    void onActivityStarted (jobject) override
    {
        checkCreated();
    }

    void onActivityPaused (jobject) override
    {
        if (auto* app = HApplicationBase::getInstance())
            app->suspended();
    }

    void onActivityResumed (jobject) override
    {
        checkInitialised();

        if (auto* app = HApplicationBase::getInstance())
            app->resumed();
    }

    static JuceAppLifecycle& getInstance (HApplicationBase* (*initSymbolAddr)())
    {
        static JuceAppLifecycle HAppLifecycle (initSymbolAddr);
        return HAppLifecycle;
    }

private:
    void checkCreated()
    {
        if (HApplicationBase::getInstance() == nullptr)
        {
            DBG (SystemStats::getHVersion());

            HApplicationBase::createInstance = createApplicationSymbol;

            initialiseJuce_GUI();

            if (! HApplicationBase::createInstance())
                HAssertfalse; // you must supply an application object for an android app!

            HAssert (MessageManager::getInstance()->isThisTheMessageThread());
        }
    }

    void checkInitialised()
    {
        checkCreated();

        if (! hasBeenInitialised)
        {
            if (auto* app = HApplicationBase::getInstance())
            {
                hasBeenInitialised = app->initialiseApp();

                if (! hasBeenInitialised)
                    exit (app->shutdownApp());
            }
        }
    }

    GlobalRef myself;
    HApplicationBase* (*createApplicationSymbol)();
    bool hasBeenInitialised = false;
};

//==============================================================================
File getExecutableFile();

void HEventsAndroidStartApp()
{
    auto dllPath = getExecutableFile().getFullPathName();
    auto addr = reinterpret_cast<HApplicationBase*(*)()> (DynamicLibrary (dllPath)
                                                                    .getFunction ("CreateApplication"));

    if (addr != nullptr)
        JuceAppLifecycle::getInstance (addr);
}


