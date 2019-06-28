

//==============================================================================
/** Initialises H's GUI classes.

    If you're embedding H into an application that uses its own event-loop rather
    than using the START_HAPPLICATION macro, call this function before making any
    H calls, to make sure things are initialised correctly.

    Note that if you're creating a H DLL for Windows, you may also need to call the
    Process::setCurrentModuleInstanceHandle() method.

    @see shutdownJuce_GUI()
*/
API void CALLTYPE  initialiseJuce_GUI();

/** Clears up any static data being used by H's GUI classes.

    If you're embedding H into an application that uses its own event-loop rather
    than using the START_HAPPLICATION macro, call this function in your shutdown
    code to clean up any H objects that might be lying around.

    @see initialiseJuce_GUI()
*/
API void CALLTYPE  shutdownJuce_GUI();


//==============================================================================
/** A utility object that helps you initialise and shutdown H correctly
    using an RAII pattern.

    When the first instance of this class is created, it calls initialiseJuce_GUI(),
    and when the last instance is deleted, it calls shutdownJuce_GUI(), so that you
    can easily be sure that as long as at least one instance of the class exists, the
    library will be initialised.

    This class is particularly handy to use at the beginning of a console app's
    main() function, because it'll take care of shutting down whenever you return
    from the main() call.

    Be careful with your threading though - to be safe, you should always make sure
    that these objects are created and deleted on the message thread.

    @tags{Events}
*/
class API ScopedJuceInitialiser_GUI  final
{
public:
    /** The constructor simply calls initialiseJuce_GUI(). */
    ScopedJuceInitialiser_GUI();

    /** The destructor simply calls shutdownJuce_GUI(). */
    ~ScopedJuceInitialiser_GUI();
};


//==============================================================================
/**
    To start a H app, use this macro: START_HAPPLICATION (AppSubClass) where
    AppSubClass is the name of a class derived from HApplication or HApplicationBase.

    See the HApplication and HApplicationBase class documentation for more details.
*/
#ifdef DOXYGEN
 #define START_HAPPLICATION(AppClass)
#else
 #if HWINDOWS && ! defined (_CONSOLE)
  #define HMAIN_FUNCTION       int __stdcall WinMain (struct HINSTANCE__*, struct HINSTANCE__*, char*, int)
  #define HMAIN_FUNCTION_ARGS
 #else
  #define HMAIN_FUNCTION       int main (int argc, char* argv[])
  #define HMAIN_FUNCTION_ARGS  argc, (const char**) argv
 #endif

 #if HIOS

  #define HCREATE_APPLICATION_DEFINE(AppClass) \
    HApplicationBase* CreateApplication() { return new AppClass(); } \
    void* GetIOSCustomDelegateClass()              { return nullptr; }

  #define HCREATE_APPLICATION_DEFINE_CUSTOM_DELEGATE(AppClass, DelegateClass) \
    HApplicationBase* CreateApplication() { return new AppClass(); } \
    void* GetIOSCustomDelegateClass()              { return [DelegateClass class]; }

  #define HMAIN_FUNCTION_DEFINITION \
    extern "C" HMAIN_FUNCTION \
    { \
       HApplicationBase::createInstance = &CreateApplication; \
       HApplicationBase::iOSCustomDelegate = GetIOSCustomDelegateClass(); \
       return HApplicationBase::main (HMAIN_FUNCTION_ARGS); \
    }

 #elif HANDROID

  #define HCREATE_APPLICATION_DEFINE(AppClass) \
    extern "C" HApplicationBase* CreateApplication() { return new AppClass(); }

  #define HMAIN_FUNCTION_DEFINITION

 #else

  #define HCREATE_APPLICATION_DEFINE(AppClass) \
    HApplicationBase* CreateApplication(); \
    HApplicationBase* CreateApplication() { return new AppClass(); }

  #define HMAIN_FUNCTION_DEFINITION \
    extern "C" HMAIN_FUNCTION \
    { \
       HApplicationBase::createInstance = &CreateApplication; \
       return HApplicationBase::main (HMAIN_FUNCTION_ARGS); \
    }

 #endif

 #if JucePlugin_Build_Standalone
  #if HUSE_CUSTOM_PLUGIN_STANDALONE_APP
    #define START_HAPPLICATION(AppClass) HCREATE_APPLICATION_DEFINE(AppClass)
    #if HIOS
     #define START_HAPPLICATION_WITH_CUSTOM_DELEGATE(AppClass, DelegateClass) HCREATE_APPLICATION_DEFINE_CUSTOM_DELEGATE(AppClass, DelegateClass)
    #endif
  #else
   #define START_HAPPLICATION(AppClass) static_assert(false, "You are trying to use START_HAPPLICATION in an audio plug-in. Define HUSE_CUSTOM_PLUGIN_STANDALONE_APP=1 if you want to use a custom standalone target app.");
   #if HIOS
    #define START_HAPPLICATION_WITH_CUSTOM_DELEGATE(AppClass, DelegateClass) static_assert(false, "You are trying to use START_HAPPLICATION in an audio plug-in. Define HUSE_CUSTOM_PLUGIN_STANDALONE_APP=1 if you want to use a custom standalone target app.");
   #endif
  #endif
 #else

  #define START_HAPPLICATION(AppClass) \
     HCREATE_APPLICATION_DEFINE(AppClass) \
     HMAIN_FUNCTION_DEFINITION

  #if HIOS
    /**
       You can instruct H to use a custom iOS app delegate class instead of H's default
       app delegate. For H to work you must pass all messages to H's internal app delegate.
       Below is an example of minimal forwarding custom delegate. Note that you are at your own
       risk if you decide to use your own delegate and subtle, hard to debug bugs may occur.

       @interface MyCustomDelegate : NSObject <UIApplicationDelegate> { NSObject<UIApplicationDelegate>* HDelegate; } @end

       @implementation MyCustomDelegate

       -(id) init
       {
           self = [super init];
           HDelegate = reinterpret_cast<NSObject<UIApplicationDelegate>*> ([[NSClassFromString (@"JuceAppStartupDelegate") alloc] init]);
           return self;
       }

       -(void) dealloc
       {
           [HDelegate release];
           [super dealloc];
       }

       - (void) forwardInvocation: (NSInvocation*) anInvocation
       {
           if (HDelegate != nullptr && [HDelegate respondsToSelector: [anInvocation selector]])
               [anInvocation invokeWithTarget: HDelegate];
           else
               [super forwardInvocation: anInvocation];
       }

       -(BOOL) respondsToSelector: (SEL) aSelector
       {
           if (HDelegate != nullptr && [HDelegate respondsToSelector: aSelector])
               return YES;

           return [super respondsToSelector: aSelector];
       }
       @end
   */
   #define START_HAPPLICATION_WITH_CUSTOM_DELEGATE(AppClass, DelegateClass) \
      HCREATE_APPLICATION_DEFINE_CUSTOM_DELEGATE(AppClass, DelegateClass) \
      HMAIN_FUNCTION_DEFINITION
  #endif
 #endif
#endif


