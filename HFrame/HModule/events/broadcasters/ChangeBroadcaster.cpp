

ChangeBroadcaster::ChangeBroadcaster() noexcept
{
    broadcastCallback.owner = this;
}

ChangeBroadcaster::~ChangeBroadcaster()
{
}

void ChangeBroadcaster::addChangeListener (ChangeListener* const listener)
{
    // Listeners can only be safely added when the event thread is locked
    // You can  use a MessageManagerLock if you need to call this from another thread.
    HASSERT_MESSAGE_MANAGER_IS_LOCKED

    changeListeners.add (listener);
}

void ChangeBroadcaster::removeChangeListener (ChangeListener* const listener)
{
    // Listeners can only be safely removed when the event thread is locked
    // You can  use a MessageManagerLock if you need to call this from another thread.
    HASSERT_MESSAGE_MANAGER_IS_LOCKED

    changeListeners.remove (listener);
}

void ChangeBroadcaster::removeAllChangeListeners()
{
    // Listeners can only be safely removed when the event thread is locked
    // You can  use a MessageManagerLock if you need to call this from another thread.
    HASSERT_MESSAGE_MANAGER_IS_LOCKED

    changeListeners.clear();
}

void ChangeBroadcaster::sendChangeMessage()
{
    if (changeListeners.size() > 0)
        broadcastCallback.triggerAsyncUpdate();
}

void ChangeBroadcaster::sendSynchronousChangeMessage()
{
    // This can only be called by the event thread.
    HASSERT_MESSAGE_MANAGER_IS_LOCKED

    broadcastCallback.cancelPendingUpdate();
    callListeners();
}

void ChangeBroadcaster::dispatchPendingMessages()
{
    broadcastCallback.handleUpdateNowIfNeeded();
}

void ChangeBroadcaster::callListeners()
{
    changeListeners.call ([this] (ChangeListener& l) { l.changeListenerCallback (this); });
}

//==============================================================================
ChangeBroadcaster::ChangeBroadcasterCallback::ChangeBroadcasterCallback()
    : owner (nullptr)
{
}

void ChangeBroadcaster::ChangeBroadcasterCallback::handleAsyncUpdate()
{
    HAssert (owner != nullptr);
    owner->callListeners();
}


