

//==============================================================================
/**
    A message that invokes a callback method when it gets delivered.

    You can use this class to fire off actions that you want to be performed later
    on the message thread.

    To use it, create a subclass of CallbackMessage which implements the messageCallback()
    method, then call post() to dispatch it. The event thread will then invoke your
    messageCallback() method later on, and will automatically delete the message object
    afterwards.

    Always create a new instance of a CallbackMessage on the heap, as it will be
    deleted automatically after the message has been delivered.

    Note that this class was essential back in the days before C++11, but in modern
    times you may prefer to use MessageManager::callAsync() with a lambda.

    @see MessageManager::callAsync, MessageListener, ActionListener, ChangeListener

    @tags{Events}
*/
class API CallbackMessage   : public MessageManager::MessageBase
{
public:
    //==============================================================================
    CallbackMessage() = default;

    /** Destructor. */
    ~CallbackMessage() override = default;

    //==============================================================================
    /** Called when the message is delivered.

        You should implement this method and make it do whatever action you want
        to perform.

        Note that like all other messages, this object will be deleted immediately
        after this method has been invoked.
    */
    virtual void messageCallback() override = 0;

private:
    // Avoid the leak-detector because for plugins, the host can unload our DLL with undelivered
    // messages still in the system event queue. These aren't harmful, but can cause annoying assertions.
    HDECLARE_NON_COPYABLE (CallbackMessage)
};


