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

HApplication::HApplication() {}
HApplication::~HApplication() {}

//==============================================================================
HApplication* CALLTYPE HApplication::getInstance() noexcept
{
    return dynamic_cast<HApplication*> (HApplicationBase::getInstance());
}

bool HApplication::moreThanOneInstanceAllowed()  { return true; }
void HApplication::anotherInstanceStarted (const String&) {}

void HApplication::suspended() {}
void HApplication::resumed() {}

void HApplication::systemRequestedQuit()         { quit(); }

void HApplication::unhandledException (const std::exception*, const String&, int)
{
    HAssertfalse;
}

//==============================================================================
ApplicationCommandTarget* HApplication::getNextCommandTarget()
{
    return nullptr;
}

void HApplication::getAllCommands (Array<CommandID>& commands)
{
    commands.add (StandardApplicationCommandIDs::quit);
}

void HApplication::getCommandInfo (const CommandID commandID, ApplicationCommandInfo& result)
{
    if (commandID == StandardApplicationCommandIDs::quit)
    {
        result.setInfo (TRANS("Quit"),
                        TRANS("Quits the application"),
                        "Application", 0);

        result.defaultKeypresses.add (KeyPress ('q', ModifierKeys::commandModifier, 0));
    }
}

bool HApplication::perform (const InvocationInfo& info)
{
    if (info.commandID == StandardApplicationCommandIDs::quit)
    {
        systemRequestedQuit();
        return true;
    }

    return false;
}

//==============================================================================
#if HMAC
 extern void initialiseMacMainMenu();
#endif

bool HApplication::initialiseApp()
{
    if (HApplicationBase::initialiseApp())
    {
       #if HMAC
        initialiseMacMainMenu(); // (needs to get the app's name)
       #endif

        return true;
    }

    return false;
}


