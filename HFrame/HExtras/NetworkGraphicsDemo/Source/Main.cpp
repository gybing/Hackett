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

#include "../JuceLibraryCode/JuceHeader.h"

namespace
{
    String getBroadcastIPAddress()
    {
        return IPAddress::getLocalAddress().toString().upToLastOccurrenceOf (".", false, false) + ".255";
    }

    static const int masterPortNumber = 9001;  // the UDP port the master sends on / the clients receive.
    static const int clientPortNumber = 9002;  // the UDP port the clients send on / the master receives.

    static const String canvasStateOSCAddress = "/H/nfd/canvasState";
    static const String newClientOSCAddress   = "/H/nfd/newClient";
    static const String userInputOSCAddress   = "/H/nfd/userInput";
};

#include "SharedCanvas.h"
#include "SlaveComponent.h"
#include "Demos.h"
#include "MasterComponent.h"


//==============================================================================
class NetworkGraphicsDemoApplication  : public HApplication
{
public:
    NetworkGraphicsDemoApplication()  : properties (getPropertyFileOptions())
    {}

    const String getApplicationName() override           { return ProjectInfo::projectName; }
    const String getApplicationVersion() override        { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override           { return true; }
    void anotherInstanceStarted (const String&) override {}

    //==============================================================================
    void initialise (const String& commandLine) override
    {
       #if ! HIOS && ! HANDROID
        // Run as the master if we have a command-line flag "master" or if the exe itself
        // has been renamed to include the word "master"..
        bool isMaster = commandLine.containsIgnoreCase ("master")
                          || File::getSpecialLocation (File::currentApplicationFile)
                                .getFileName().containsIgnoreCase ("master");

        if (isMaster)
            mainWindows.add (new MainWindow (properties));
       #endif

        mainWindows.add (new MainWindow (properties, 0));

        Desktop::getInstance().setScreenSaverEnabled (false);
    }

    void shutdown() override
    {
        mainWindows.clear();
        properties.saveIfNeeded();
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    //==============================================================================
    struct MainWindow    : public DocumentWindow
    {
        MainWindow (PropertiesFile& props)
            : DocumentWindow ("H Networked Graphics Demo - Master", Colours::white, DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new MasterContentComponent (props), true);
            setBounds (100, 50, getWidth(), getHeight());
            setResizable (true, false);
            setVisible (true);

            glContext.attachTo (*this);
        }

        MainWindow (PropertiesFile& props, int windowIndex)
            : DocumentWindow ("H Networked Graphics Demo", Colours::black, DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new SlaveCanvasComponent (props, windowIndex), true);
            setBounds (500, 100, getWidth(), getHeight());
            setResizable (true, false);
            setVisible (true);

           #if ! HIOS
            glContext.attachTo (*this);
           #endif

           #if HIOS || HANDROID
            setFullScreen (true);
           #endif
        }

        ~MainWindow()
        {
            glContext.detach();
        }

        void closeButtonPressed() override
        {
            HApplication::getInstance()->systemRequestedQuit();
        }

        OpenGLContext glContext;

        HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    static PropertiesFile::Options getPropertyFileOptions()
    {
        PropertiesFile::Options o;
        o.applicationName = "H Network Graphics Demo";
        o.filenameSuffix = ".settings";
        o.folderName = "H Network Graphics Demo";
        o.osxLibrarySubFolder = "Application Support/H Network Graphics Demo";
        o.millisecondsBeforeSaving = 2000;
        return o;
    }

    PropertiesFile properties;
    OwnedArray<MainWindow> mainWindows;
};


//==============================================================================
// This macro generates the main() routine that launches the app.
START_HAPPLICATION (NetworkGraphicsDemoApplication)
