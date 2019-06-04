

//==============================================================================
/**
    MessageListener subclasses can post and receive Message objects.

    @see Message, MessageManager, ActionListener, ChangeListener

    @tags{Events}
*/
class API MessageListener
{
public:
    //==============================================================================
    MessageListener() noexcept;

    /** Destructor. */
    virtual ~MessageListener();

    //==============================================================================
    /** This is the callback method that receives incoming messages.

        This is called by the MessageManager from its dispatch loop.

        @see postMessage
    */
    virtual void handleMessage (const Message& message) = 0;

    //==============================================================================
    /** Sends a message to the message queue, for asynchronous delivery to this listener
        later on.

        This method can be called safely by any thread.

        @param message      the message object to send - this will be deleted
                            automatically by the message queue, so make sure it's
                            allocated on the heap, not the stack!
        @see handleMessage
    */
    void postMessage (Message* message) const;

private:
    WeakReference<MessageListener>::Master masterReference;
    friend class WeakReference<MessageListener>;
};


