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

void Analytics::addDestination (AnalyticsDestination* destination)
{
    destinations.add (destination);
}

OwnedArray<AnalyticsDestination>& Analytics::getDestinations()
{
    return destinations;
}

void Analytics::setUserId (String newUserId)
{
    userId = newUserId;
}

void Analytics::setUserProperties (StringPairArray properties)
{
    userProperties = properties;
}

void Analytics::logEvent (const String& eventName,
                          const StringPairArray& parameters,
                          int eventType)
{
    if (! isSuspended)
    {
        AnalyticsDestination::AnalyticsEvent event
        {
            eventName,
            eventType,
            Time::getMillisecondCounter(),
            parameters,
            userId,
            userProperties
        };

        for (auto* destination : destinations)
            destination->logEvent (event);
    }
}

void Analytics::setSuspended (bool shouldBeSuspended)
{
    isSuspended = shouldBeSuspended;
}

HIMPLEMENT_SINGLETON (Analytics)

}
