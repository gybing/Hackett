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

#pragma once

#include "Headers.h"

//==============================================================================
enum ProjectorAnalyticsEvent
{
    appEvent,
    projectEvent,
    userEvent,
    exampleEvent,
    startPageEvent
};

//==============================================================================
class ProjectorAnalyticsDestination    : public ThreadedAnalyticsDestination
{
public:
    ProjectorAnalyticsDestination();
    ~ProjectorAnalyticsDestination() override;

    //==============================================================================
    bool logBatchedEvents (const Array<AnalyticsEvent>&) override;
    void stopLoggingEvents() override;

    int getMaximumBatchSize() override    { return 20; }

private:
    void saveUnloggedEvents (const std::deque<AnalyticsEvent>&) override;
    void restoreUnloggedEvents (std::deque<AnalyticsEvent>&) override;

    //==============================================================================
    String apiKey;

    const int initialPeriodMs = 1000;
    int periodMs = initialPeriodMs;

    CriticalSection webStreamCreation;
    bool shouldExit = false;
    std::unique_ptr<WebInputStream> webStream;

    File savedEventsFile;
};
