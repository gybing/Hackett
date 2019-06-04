

//==============================================================================
/** Manages a list of ActionListeners, and can send them messages.

    To quickly add methods to your class that can add/remove action
    listeners and broadcast to them, you can derive from this.

    @see ActionListener, ChangeListener

    @tags{Events}
*/
class API ActionBroadcaster
{
public:
    //==============================================================================
    /** Creates an ActionBroadcaster. */
    ActionBroadcaster();

    /** Destructor. */
    virtual ~ActionBroadcaster();

    //==============================================================================
    /** Adds a listener to the list.
        Trying to add a listener that's already on the list will have no effect.
    */
    void addActionListener (ActionListener* listener);

    /** Removes a listener from the list.
        If the listener isn't on the list, this won't have any effect.
    */
    void removeActionListener (ActionListener* listener);

    /** Removes all listeners from the list. */
    void removeAllActionListeners();

    //==============================================================================
    /** Broadcasts a message to all the registered listeners.
        @see ActionListener::actionListenerCallback
    */
    void sendActionMessage (const String& message) const;


private:
    //==============================================================================
    class ActionMessage;
    friend class ActionMessage;

    SortedSet<ActionListener*> actionListeners;
    CriticalSection actionListenerLock;

    HDECLARE_WEAK_REFERENCEABLE (ActionBroadcaster)
    HDECLARE_NON_COPYABLE (ActionBroadcaster)
};


