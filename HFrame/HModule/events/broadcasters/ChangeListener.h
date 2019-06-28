

class ChangeBroadcaster;

//==============================================================================
/**
    Receives change event callbacks that are sent out by a ChangeBroadcaster.

    A ChangeBroadcaster keeps a set of listeners to which it broadcasts a message when
    the ChangeBroadcaster::sendChangeMessage() method is called. A subclass of
    ChangeListener is used to receive these callbacks.

    Note that the major difference between an ActionListener and a ChangeListener
    is that for a ChangeListener, multiple changes will be coalesced into fewer
    callbacks, but ActionListeners perform one callback for every event posted.

    @see ChangeBroadcaster, ActionListener

    @tags{Events}
*/
class API ChangeListener
{
public:
    /** Destructor. */
    virtual ~ChangeListener() = default;

    /** Your subclass should implement this method to receive the callback.
        @param source the ChangeBroadcaster that triggered the callback.
    */
    virtual void changeListenerCallback (ChangeBroadcaster* source) = 0;


    //==============================================================================
   #if HCATCH_DEPRECATED_CODE_MISUSE
    // This method's signature has changed to take a ChangeBroadcaster parameter - please update your code!
    private: virtual int changeListenerCallback (void*) { return 0; }
   #endif
};


