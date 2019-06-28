

class ActionBroadcaster::ActionMessage  : public MessageManager::MessageBase
{
public:
    ActionMessage (const ActionBroadcaster* ab,
                   const String& messageText, ActionListener* l) noexcept
        : broadcaster (const_cast<ActionBroadcaster*> (ab)),
          message (messageText),
          listener (l)
    {}

    void messageCallback() override
    {
        if (auto b = broadcaster.get())
            if (b->actionListeners.contains (listener))
                listener->actionListenerCallback (message);
    }

private:
    WeakReference<ActionBroadcaster> broadcaster;
    const String message;
    ActionListener* const listener;

    HDECLARE_NON_COPYABLE (ActionMessage)
};

//==============================================================================
ActionBroadcaster::ActionBroadcaster()
{
    // are you trying to create this object before or after H has been intialised??
    HASSERT_MESSAGE_MANAGER_EXISTS
}

ActionBroadcaster::~ActionBroadcaster()
{
    // all event-based objects must be deleted BEFORE H is shut down!
    HASSERT_MESSAGE_MANAGER_EXISTS
}

void ActionBroadcaster::addActionListener (ActionListener* const listener)
{
    const ScopedLock sl (actionListenerLock);

    if (listener != nullptr)
        actionListeners.add (listener);
}

void ActionBroadcaster::removeActionListener (ActionListener* const listener)
{
    const ScopedLock sl (actionListenerLock);
    actionListeners.removeValue (listener);
}

void ActionBroadcaster::removeAllActionListeners()
{
    const ScopedLock sl (actionListenerLock);
    actionListeners.clear();
}

void ActionBroadcaster::sendActionMessage (const String& message) const
{
    const ScopedLock sl (actionListenerLock);

    for (int i = actionListeners.size(); --i >= 0;)
        (new ActionMessage (this, message, actionListeners.getUnchecked(i)))->post();
}


