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

/**
    An implementation of the OnlineUnlockStatus class which talks to the
    Tracktion Marketplace server.

    For details about how to use this class, see the docs for the base
    class: OnlineUnlockStatus. Basically, you need to inherit from it, and
    implement all the pure virtual methods to tell it about your product.

    @see OnlineUnlockStatus, OnlineUnlockForm, KeyGeneration

    @tags{ProductUnlocking}
*/
class API TracktionMarketplaceStatus   : public OnlineUnlockStatus
{
public:
    TracktionMarketplaceStatus();

    /** @internal */
    bool doesProductIDMatch (const String& returnedIDFromServer) override;
    /** @internal */
    URL getServerAuthenticationURL() override;
    /** @internal */
    String getWebsiteName() override;
    /** @internal */
    String readReplyFromWebserver (const String& email, const String& password) override;
    /** @internal */
    void userCancelled() override;

private:
    CriticalSection streamCreationLock;
    std::unique_ptr<WebInputStream> stream;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TracktionMarketplaceStatus)
};


