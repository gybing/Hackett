

//==============================================================================
/**
    Allows threads to wait for events triggered by other threads.

    A thread can call WaitableEvent::wait() to suspend the calling thread until
    another thread wakes it up by calling the WaitableEvent::signal() method.

    @tags{Core}
*/
class API WaitableEvent
{
public:
    //==============================================================================
    /** Creates a WaitableEvent object.

        The object is initially in an unsignalled state.

        @param manualReset  If this is false, the event will be reset automatically when the wait()
                            method is called. If manualReset is true, then once the event is signalled,
                            the only way to reset it will be by calling the reset() method.
    */
    explicit WaitableEvent (bool manualReset = false) noexcept;

    /** Destructor.

        If other threads are waiting on this object when it gets deleted, this
        can cause nasty errors, so be careful!
    */
    ~WaitableEvent() noexcept;

    //==============================================================================
    /** Suspends the calling thread until the event has been signalled.

        This will wait until the object's signal() method is called by another thread,
        or until the timeout expires.

        After the event has been signalled, this method will return true and if manualReset
        was set to false in the WaitableEvent's constructor, then the event will be reset.

        @param timeOutMilliseconds  the maximum time to wait, in milliseconds. A negative
                                    value will cause it to wait forever.

        @returns    true if the object has been signalled, false if the timeout expires first.
        @see signal, reset
    */
    bool wait (int timeOutMilliseconds = -1) const noexcept;

    //==============================================================================
    /** Wakes up any threads that are currently waiting on this object.

        If signal() is called when nothing is waiting, the next thread to call wait()
        will return immediately and reset the signal.

        If the WaitableEvent is manual reset, all current and future threads that wait upon this
        object will be woken, until reset() is explicitly called.

        If the WaitableEvent is automatic reset, and one or more threads is waiting upon the object,
        then one of them will be woken up. If no threads are currently waiting, then the next thread
        to call wait() will be woken up. As soon as a thread is woken, the signal is automatically
        reset.

        @see wait, reset
    */
    void signal() const noexcept;

    //==============================================================================
    /** Resets the event to an unsignalled state.
        If it's not already signalled, this does nothing.
    */
    void reset() const noexcept;


private:
    //==============================================================================
   #if HWINDOWS
    void* handle;
   #else
    mutable pthread_cond_t condition;
    mutable pthread_mutex_t mutex;
    mutable bool triggered, manualReset;
   #endif

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaitableEvent)
};


