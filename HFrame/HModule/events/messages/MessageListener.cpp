

Message::Message() noexcept {}
Message::~Message() {}

void Message::messageCallback()
{
    if (auto* r = recipient.get())
        r->handleMessage (*this);
}

MessageListener::MessageListener() noexcept
{
    // Are you trying to create a messagelistener before or after H has been intialised??
    HASSERT_MESSAGE_MANAGER_EXISTS
}

MessageListener::~MessageListener()
{
    masterReference.clear();
}

void MessageListener::postMessage (Message* const message) const
{
    message->recipient = const_cast<MessageListener*> (this);
    message->post();
}


