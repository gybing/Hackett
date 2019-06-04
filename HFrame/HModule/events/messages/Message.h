

class MessageListener;


//==============================================================================
/** The base class for objects that can be sent to a MessageListener.

    If you want to send a message that carries some kind of custom data, just
    create a subclass of Message with some appropriate member variables to hold
    your data.

    Always create a new instance of a Message object on the heap, as it will be
    deleted automatically after the message has been delivered.

    @see MessageListener, MessageManager, ActionListener, ChangeListener

    @tags{Events}
*/
class API Message  : public MessageManager::MessageBase
{
public:
    //==============================================================================
    /** Creates an uninitialised message. */
    Message() noexcept;
    ~Message() override;

    using Ptr = ReferenceCountedObjectPtr<Message>;

    //==============================================================================
private:
    friend class MessageListener;
    WeakReference<MessageListener> recipient;
    void messageCallback() override;

    // Avoid the leak-detector because for plugins, the host can unload our DLL with undelivered
    // messages still in the system event queue. These aren't harmful, but can cause annoying assertions.
    HDECLARE_NON_COPYABLE (Message)
};


