/*
  ==============================================================================

   This file is part of the H examples.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

#include "../BlockFinder.h"

// A simple H app containing our BlockFinder. This is a quick way of
// setting up an event loop so we can receive Block topology change events.
class MyHApp  : public HApplicationBase
{
public:
    MyHApp()  {}
    ~MyHApp() {}

    void initialise (const String&) override {}
    void shutdown() override                       {}

    const String getApplicationName() override           { return "BlockFinder"; }
    const String getApplicationVersion() override        { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override                 { return true; }
    void anotherInstanceStarted (const String&) override {}

    void suspended() override           {}
    void resumed() override             {}
    void systemRequestedQuit() override {}
    void unhandledException(const std::exception*, const String&,
                            int lineNumber) override {}

private:
    // Our BLOCKS class.
    BlockFinder finder;
};

START_HAPPLICATION (MyHApp)
