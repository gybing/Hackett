

WebInputStream::WebInputStream (const URL& url, const bool usePost)
    : pimpl (new Pimpl (*this, url, usePost)), hasCalledConnect (false)
{
}

WebInputStream::~WebInputStream()
{
    delete pimpl;
}

WebInputStream& WebInputStream::withExtraHeaders (const String& extra)         { pimpl->withExtraHeaders (extra);       return *this; }
WebInputStream& WebInputStream::withCustomRequestCommand (const String& cmd)   { pimpl->withCustomRequestCommand(cmd);  return *this; }
WebInputStream& WebInputStream::withConnectionTimeout (int t)                  { pimpl->withConnectionTimeout (t);      return *this; }
WebInputStream& WebInputStream::withNumRedirectsToFollow (int num)             { pimpl->withNumRedirectsToFollow (num); return *this; }
StringPairArray WebInputStream::getRequestHeaders() const                      { return pimpl->getRequestHeaders(); }
StringPairArray WebInputStream::getResponseHeaders()                           { connect (nullptr); return pimpl->getResponseHeaders(); }
bool WebInputStream::isError() const                                           { return pimpl->isError(); }
void WebInputStream::cancel()                                                  { pimpl->cancel(); }
bool WebInputStream::isExhausted()                                             { return pimpl->isExhausted(); }
int64 WebInputStream::getPosition()                                            { return pimpl->getPosition(); }
int64 WebInputStream::getTotalLength()                                         { connect (nullptr); return pimpl->getTotalLength(); }
int WebInputStream::read (void* buffer, int bytes)                             { connect (nullptr); return pimpl->read (buffer, bytes); }
bool WebInputStream::setPosition (int64 pos)                                   { return pimpl->setPosition (pos); }
int WebInputStream::getStatusCode()                                            { connect (nullptr); return pimpl->getStatusCode(); }

bool WebInputStream::connect (Listener* listener)
{
    if (hasCalledConnect)
        return ! isError();

    hasCalledConnect = true;
    return pimpl->connect (listener);
}

StringPairArray WebInputStream::parseHttpHeaders (const String& headerData)
{
    StringPairArray headerPairs;
    StringArray headerLines = StringArray::fromLines (headerData);

    // ignore the first line as this is the status line
    for (int i = 1; i < headerLines.size(); ++i)
    {
        const String& headersEntry = headerLines[i];

        if (headersEntry.isNotEmpty())
        {
            const String key   (headersEntry.upToFirstOccurrenceOf (": ", false, false));
            const String value (headersEntry.fromFirstOccurrenceOf (": ", false, false));
            const String previousValue (headerPairs [key]);
            headerPairs.set (key, previousValue.isEmpty() ? value : (previousValue + "," + value));
        }
    }

    return headerPairs;
}

void WebInputStream::createHeadersAndPostData (const URL& aURL, String& headers, MemoryBlock& data)
{
    aURL.createHeadersAndPostData (headers, data);
}


