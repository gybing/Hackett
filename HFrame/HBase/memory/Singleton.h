

//==============================================================================
/**
    Used by the HDECLARE_SINGLETON macros to manage a static pointer
    to a singleton instance.

    You generally won't use this directly, but see the macros HDECLARE_SINGLETON,
    HDECLARE_SINGLETON_SINGLETHREADED, HDECLARE_SINGLETON_SINGLETHREADED_MINIMAL,
    and HIMPLEMENT_SINGLETON for how it is intended to be used.

    @tags{Core}
*/
template <typename Type, typename MutexType, bool onlyCreateOncePerRun>
struct SingletonHolder  : private MutexType // (inherited so we can use the empty-base-class optimisation)
{
    SingletonHolder() = default;

    ~SingletonHolder()
    {
        /* The static singleton holder is being deleted before the object that it holds
           has been deleted. This could mean that you've forgotten to call clearSingletonInstance()
           in the class's destructor, or have failed to delete it before your app shuts down.
           If you're having trouble cleaning up your singletons, perhaps consider using the
           SharedResourcePointer class instead.
        */
        HAssert (instance == nullptr);
    }

    /** Returns the current instance, or creates a new instance if there isn't one. */
    Type* get()
    {
        if (instance == nullptr)
        {
            typename MutexType::ScopedLockType sl (*this);

            if (instance == nullptr)
            {
                auto once = onlyCreateOncePerRun; // (local copy avoids VS compiler warning about this being constant)

                if (once)
                {
                    static bool createdOnceAlready = false;

                    if (createdOnceAlready)
                    {
                        // This means that the doNotRecreateAfterDeletion flag was set
                        // and you tried to create the singleton more than once.
                        HAssertfalse;
                        return nullptr;
                    }

                    createdOnceAlready = true;
                }

                static bool alreadyInside = false;

                if (alreadyInside)
                {
                    // This means that your object's constructor has done something which has
                    // ended up causing a recursive loop of singleton creation..
                    HAssertfalse;
                }
                else
                {
                    alreadyInside = true;
                    getWithoutChecking();
                    alreadyInside = false;
                }
            }
        }

        return instance;
    }

    /** Returns the current instance, or creates a new instance if there isn't one, but doesn't do
        any locking, or checking for recursion or error conditions.
    */
    Type* getWithoutChecking()
    {
        if (instance == nullptr)
        {
            auto newObject = new Type(); // (create into a local so that instance is still null during construction)
            instance = newObject;
        }

        return instance;
    }

    /** Deletes and resets the current instance, if there is one. */
    void deleteInstance()
    {
        typename MutexType::ScopedLockType sl (*this);
        auto old = instance;
        instance = nullptr;
        delete old;
    }

    /** Called by the class's destructor to clear the pointer if it is currently set to the given object. */
    void clear (Type* expectedObject) noexcept
    {
        if (instance == expectedObject)
            instance = nullptr;
    }

    Type* instance = nullptr;
};


//==============================================================================
/**
    Macro to generate the appropriate methods and boilerplate for a singleton class.

    To use this, add the line HDECLARE_SINGLETON(MyClass, doNotRecreateAfterDeletion)
    to the class's definition.

    Then put a macro HIMPLEMENT_SINGLETON(MyClass) along with the class's
    implementation code.

    It's also a very good idea to also add the call clearSingletonInstance() in your class's
    destructor, in case it is deleted by other means than deleteInstance()

    Clients can then call the static method MyClass::getInstance() to get a pointer
    to the singleton, or MyClass::getInstanceWithoutCreating() which will return nullptr if
    no instance currently exists.

    e.g. @code

        struct MySingleton
        {
            MySingleton() {}

            ~MySingleton()
            {
                // this ensures that no dangling pointers are left when the
                // singleton is deleted.
                clearSingletonInstance();
            }

            HDECLARE_SINGLETON (MySingleton, false)
        };

        // ..and this goes in a suitable .cpp file:
        HIMPLEMENT_SINGLETON (MySingleton)


        // example of usage:
        auto* m = MySingleton::getInstance(); // creates the singleton if there isn't already one.

        ...

        MySingleton::deleteInstance(); // safely deletes the singleton (if it's been created).

    @endcode

    If doNotRecreateAfterDeletion = true, it won't allow the object to be created more
    than once during the process's lifetime - i.e. after you've created and deleted the
    object, getInstance() will refuse to create another one. This can be useful to stop
    objects being accidentally re-created during your app's shutdown code.

    If you know that your object will only be created and deleted by a single thread, you
    can use the slightly more efficient HDECLARE_SINGLETON_SINGLETHREADED macro instead
    of this one.

    @see HIMPLEMENT_SINGLETON, HDECLARE_SINGLETON_SINGLETHREADED
*/
#define HDECLARE_SINGLETON(Classname, doNotRecreateAfterDeletion) \
\
    static SingletonHolder<Classname, CriticalSection, doNotRecreateAfterDeletion> singletonHolder; \
    friend decltype (singletonHolder); \
\
    static Classname* CALLTYPE getInstance()                           { return singletonHolder.get(); } \
    static Classname* CALLTYPE getInstanceWithoutCreating() noexcept   { return singletonHolder.instance; } \
    static void CALLTYPE deleteInstance() noexcept                     { singletonHolder.deleteInstance(); } \
    void clearSingletonInstance() noexcept                                  { singletonHolder.clear (this); }


//==============================================================================
/** This is a counterpart to the HDECLARE_SINGLETON macros.

    After adding the HDECLARE_SINGLETON to the class definition, this macro has
    to be used in the cpp file.
*/
#define HIMPLEMENT_SINGLETON(Classname) \
\
    decltype (Classname::singletonHolder) Classname::singletonHolder;


//==============================================================================
/**
    Macro to declare member variables and methods for a singleton class.

    This is exactly the same as HDECLARE_SINGLETON, but doesn't use a critical
    section to make access to it thread-safe. If you know that your object will
    only ever be created or deleted by a single thread, then this is a
    more efficient version to use.

    If doNotRecreateAfterDeletion = true, it won't allow the object to be created more
    than once during the process's lifetime - i.e. after you've created and deleted the
    object, getInstance() will refuse to create another one. This can be useful to stop
    objects being accidentally re-created during your app's shutdown code.

    See the documentation for HDECLARE_SINGLETON for more information about
    how to use it. Just like HDECLARE_SINGLETON you need to also have a
    corresponding HIMPLEMENT_SINGLETON statement somewhere in your code.

    @see HIMPLEMENT_SINGLETON, HDECLARE_SINGLETON, HDECLARE_SINGLETON_SINGLETHREADED_MINIMAL
*/
#define HDECLARE_SINGLETON_SINGLETHREADED(Classname, doNotRecreateAfterDeletion) \
\
    static SingletonHolder<Classname, DummyCriticalSection, doNotRecreateAfterDeletion> singletonHolder; \
    friend decltype (singletonHolder); \
\
    static Classname* CALLTYPE getInstance()                           { return singletonHolder.get(); } \
    static Classname* CALLTYPE getInstanceWithoutCreating() noexcept   { return singletonHolder.instance; } \
    static void CALLTYPE deleteInstance() noexcept                     { singletonHolder.deleteInstance(); } \
    void clearSingletonInstance() noexcept                                  { singletonHolder.clear (this); }


//==============================================================================
/**
    Macro to declare member variables and methods for a singleton class.

    This is like HDECLARE_SINGLETON_SINGLETHREADED, but doesn't do any checking
    for recursion or repeated instantiation. It's intended for use as a lightweight
    version of a singleton, where you're using it in very straightforward
    circumstances and don't need the extra checking.

    See the documentation for HDECLARE_SINGLETON for more information about
    how to use it. Just like HDECLARE_SINGLETON you need to also have a
    corresponding HIMPLEMENT_SINGLETON statement somewhere in your code.

    @see HIMPLEMENT_SINGLETON, HDECLARE_SINGLETON
*/
#define HDECLARE_SINGLETON_SINGLETHREADED_MINIMAL(Classname) \
\
    static SingletonHolder<Classname, DummyCriticalSection, false> singletonHolder; \
    friend decltype (singletonHolder); \
\
    static Classname* CALLTYPE getInstance()                           { return singletonHolder.getWithoutChecking(); } \
    static Classname* CALLTYPE getInstanceWithoutCreating() noexcept   { return singletonHolder.instance; } \
    static void CALLTYPE deleteInstance() noexcept                     { singletonHolder.deleteInstance(); } \
    void clearSingletonInstance() noexcept                                  { singletonHolder.clear (this); }


//==============================================================================
#ifndef DOXYGEN
 // These are ancient macros, and have now been updated with new names to match the H style guide,
 // so please update your code to use the newer versions!
 #define DeclareSingleton(Classname, doNotRecreate)                HDECLARE_SINGLETON(Classname, doNotRecreate)
 #define DeclareSingleton_SingleThreaded(Classname, doNotRecreate) HDECLARE_SINGLETON_SINGLETHREADED(Classname, doNotRecreate)
 #define DeclareSingleton_SingleThreaded_Minimal(Classname)        HDECLARE_SINGLETON_SINGLETHREADED_MINIMAL(Classname)
 #define ImplementSingleton(Classname)                             HIMPLEMENT_SINGLETON(Classname)
 #define ImplementSingleton_SingleThreaded(Classname)              HIMPLEMENT_SINGLETON(Classname)
#endif


