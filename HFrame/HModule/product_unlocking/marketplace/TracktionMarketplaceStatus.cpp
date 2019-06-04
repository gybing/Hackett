/*
  ==============================================================================

   This file is part of the H library.
   Copyright (c) 2017 - ROLI Ltd.

   H is an open source library subject to commercial or open-source
   licensing.

   By using H, you agree to the terms of both the H 5 End-User License
   Agreement and H 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.H.com/H-5-licence
   Privacy Policy: www.H.com/H-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   H IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

TracktionMarketplaceStatus::TracktionMarketplaceStatus() {}

URL TracktionMarketplaceStatus::getServerAuthenticationURL()
{
    return URL ("https://www.tracktion.com/marketplace/authenticate.php");
}

String TracktionMarketplaceStatus::getWebsiteName()
{
    return "tracktion.com";
}

bool TracktionMarketplaceStatus::doesProductIDMatch (const String& returnedIDFromServer)
{
    return getProductID() == returnedIDFromServer;
}

String TracktionMarketplaceStatus::readReplyFromWebserver (const String& email, const String& password)
{
    URL url (getServerAuthenticationURL()
                .withParameter ("product", getProductID())
                .withParameter ("email", email)
                .withParameter ("pw", password)
                .withParameter ("os", SystemStats::getOperatingSystemName())
                .withParameter ("mach", getLocalMachineIDs()[0]));

    DBG ("Trying to unlock via URL: " << url.toString (true));

    {
        ScopedLock lock (streamCreationLock);
        stream.reset (new WebInputStream (url, true));
    }

    if (stream->connect (nullptr))
    {
        auto thread = Thread::getCurrentThread();
        MemoryOutputStream result;

        while (! (stream->isExhausted() || stream->isError()
                    || (thread != nullptr && thread->threadShouldExit())))
        {
            auto bytesRead = result.writeFromInputStream (*stream, 8192);

            if (bytesRead < 0)
                break;
        }

        return result.toString();
    }

    return {};
}

void TracktionMarketplaceStatus::userCancelled()
{
    ScopedLock lock (streamCreationLock);

    if (stream != nullptr)
        stream->cancel();
}

