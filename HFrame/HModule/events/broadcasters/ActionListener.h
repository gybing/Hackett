

//==============================================================================
/**
    Interface class for delivery of events that are sent by an ActionBroadcaster.

    @see ActionBroadcaster, ChangeListener

    @tags{Events}
*/
class API ActionListener
{
public:
    /** Destructor. */
    virtual ~ActionListener() = default;

    /** Overridden by your subclass to receive the callback.

        @param message  the string that was specified when the event was triggered
                        by a call to ActionBroadcaster::sendActionMessage()
    */
    virtual void actionListenerCallback (const String& message) = 0;
};


