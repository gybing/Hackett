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
#include "UI/MainHostWindow.h"
#include "Filters/InternalFilters.h"

#if ! (HPLUGINHOST_VST || HPLUGINHOST_VST3 || HPLUGINHOST_AU)
 #error "If you're building the audio plugin host, you probably want to enable VST and/or AU support"
#endif


//==============================================================================
class PluginHostApp  : public HApplication,
                       private AsyncUpdater
{
public:
    PluginHostApp() {}

    void initialise (const String&) override
    {
        // initialise our settings file..

        PropertiesFile::Options options;
        options.applicationName     = "Juce Audio Plugin Host";
        options.filenameSuffix      = "settings";
        options.osxLibrarySubFolder = "Preferences";

        appProperties.reset (new ApplicationProperties());
        appProperties->setStorageParameters (options);

        mainWindow.reset (new MainHostWindow());
        mainWindow->setUsingNativeTitleBar (true);

        commandManager.registerAllCommandsForTarget (this);
        commandManager.registerAllCommandsForTarget (mainWindow.get());

        mainWindow->menuItemsChanged();

        // Important note! We're going to use an async update here so that if we need
        // to re-open a file and instantiate some plugins, it will happen AFTER this
        // initialisation method has returned.
        // On Windows this probably won't make a difference, but on OSX there's a subtle event loop
        // issue that can happen if a plugin runs one of those irritating modal dialogs while it's
        // being loaded. If that happens inside this method, the OSX event loop seems to be in some
        // kind of special "initialisation" mode and things get confused. But if we load the plugin
        // later when the normal event loop is running, everything's fine.
        triggerAsyncUpdate();
    }

    void handleAsyncUpdate() override
    {
        File fileToOpen;

       #if HANDROID || HIOS
        fileToOpen = FilterGraph::getDefaultGraphDocumentOnMobile();
       #else
        for (int i = 0; i < getCommandLineParameterArray().size(); ++i)
        {
            fileToOpen = File::getCurrentWorkingDirectory().getChildFile (getCommandLineParameterArray()[i]);

            if (fileToOpen.existsAsFile())
                break;
        }
       #endif

        if (! fileToOpen.existsAsFile())
        {
            RecentlyOpenedFilesList recentFiles;
            recentFiles.restoreFromString (getAppProperties().getUserSettings()->getValue ("recentFilterGraphFiles"));

            if (recentFiles.getNumFiles() > 0)
                fileToOpen = recentFiles.getFile (0);
        }

        if (fileToOpen.existsAsFile())
            if (auto* graph = mainWindow->graphHolder.get())
                if (auto* ioGraph = graph->graph.get())
                    ioGraph->loadFrom (fileToOpen, true);
    }

    void shutdown() override
    {
        mainWindow = nullptr;
        appProperties = nullptr;
        LookAndFeel::setDefaultLookAndFeel (nullptr);
    }

    void suspended() override
    {
       #if HANDROID || HIOS
        if (GraphDocumentComponent* graph = mainWindow->graphHolder.get())
            if (FilterGraph* ioGraph = graph->graph.get())
                ioGraph->saveDocument (FilterGraph::getDefaultGraphDocumentOnMobile());
       #endif
    }

    void systemRequestedQuit() override
    {
        if (mainWindow != nullptr)
            mainWindow->tryToQuitApplication();
        else
            HApplicationBase::quit();
    }

    void backButtonPressed() override
    {
        if (mainWindow->graphHolder != nullptr)
            mainWindow->graphHolder->hideLastSidePanel();
    }

    const String getApplicationName() override       { return "Juce Plug-In Host"; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return true; }

    ApplicationCommandManager commandManager;
    std::unique_ptr<ApplicationProperties> appProperties;

private:
    std::unique_ptr<MainHostWindow> mainWindow;
};

static PluginHostApp& getApp()                    { return *dynamic_cast<PluginHostApp*>(HApplication::getInstance()); }

ApplicationProperties& getAppProperties()         { return *getApp().appProperties; }
ApplicationCommandManager& getCommandManager()    { return getApp().commandManager; }

bool isOnTouchDevice()                            { return Desktop::getInstance().getMainMouseSource().isTouch(); }

// This kicks the whole thing off..
START_HAPPLICATION (PluginHostApp)
