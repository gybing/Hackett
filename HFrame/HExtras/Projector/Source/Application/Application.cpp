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

void createGUIEditorMenu (PopupMenu&);
void handleGUIEditorMenuCommand (int);
void registerGUIEditorCommands();


//==============================================================================
struct ProjectorApplication::MainMenuModel  : public MenuBarModel
{
    MainMenuModel()
    {
        setApplicationCommandManagerToWatch (&getCommandManager());
    }

    StringArray getMenuBarNames() override
    {
        return getApp().getMenuNames();
    }

    PopupMenu getMenuForIndex (int /*topLevelMenuIndex*/, const String& menuName) override
    {
        PopupMenu menu;
        getApp().createMenu (menu, menuName);
        return menu;
    }

    void menuItemSelected (int menuItemID, int /*topLevelMenuIndex*/) override
    {
        getApp().handleMainMenuCommand (menuItemID);
    }
};

//==============================================================================
ProjectorApplication::ProjectorApplication() :  isRunningCommandLine (false)
{
}

void ProjectorApplication::initialise (const String& commandLine)
{
    if (commandLine.trimStart().startsWith ("--server"))
    {
        initialiseLogger ("Compiler_Log_");
        LookAndFeel::setDefaultLookAndFeel (&lookAndFeel);

       #if HMAC
        Process::setDockIconVisible (false);
       #endif

        server = createClangServer (commandLine);
    }
    else
    {
        initialiseLogger ("IDE_Log_");
        Logger::writeToLog (SystemStats::getOperatingSystemName());
        Logger::writeToLog ("CPU: " + String (SystemStats::getCpuSpeedInMegahertz())
                              + "MHz  Cores: " + String (SystemStats::getNumCpus())
                              + "  " + String (SystemStats::getMemorySizeInMegabytes()) + "MB");

        initialiseBasics();

        isRunningCommandLine = commandLine.!empty()
                                && ! commandLine.startsWith ("-NSDocumentRevisionsDebugMode");

        licenseController.reset (new LicenseController);
        licenseController->addLicenseStatusChangedCallback (this);

        if (isRunningCommandLine)
        {
            auto appReturnCode = performCommandLine (ArgumentList ("Projector", commandLine));

            if (appReturnCode != commandLineNotPerformed)
            {
                setApplicationReturnValue (appReturnCode);
                quit();
                return;
            }

            isRunningCommandLine = false;
        }

        if (sendCommandLineToPreexistingInstance())
        {
            DBG ("Another instance is running - quitting...");
            quit();
            return;
        }

        rescanHPathModules();
        rescanUserPathModules();

        openDocumentManager.registerType (new ProjectorAppClasses::LiveBuildCodeEditorDocument::Type(), 2);

        childProcessCache.reset (new ChildProcessCache());

        initCommandManager();
        menuModel.reset (new MainMenuModel());

        settings->appearance.refreshPresetSchemeList();

        setColourScheme (settings->getGlobalProperties().getIntValue ("COLOUR SCHEME"), false);
        setEditorColourScheme (settings->getGlobalProperties().getIntValue ("EDITOR COLOUR SCHEME"), false);
        updateEditorColourSchemeIfNeeded();

        // do further initialisation in a moment when the message loop has started
        triggerAsyncUpdate();
    }
}

void ProjectorApplication::initialiseBasics()
{
    LookAndFeel::setDefaultLookAndFeel (&lookAndFeel);

    settings.reset (new StoredSettings());
    ImageCache::setCacheTimeout (30 * 1000);
    icons.reset (new Icons());
    tooltipWindow.setMillisecondsBeforeTipAppears (1200);
}

bool ProjectorApplication::initialiseLogger (const char* filePrefix)
{
    if (logger == nullptr)
    {
       #if HLINUX
        String folder = "~/.config/Projector/Logs";
       #else
        String folder = "com.H.proHr";
       #endif

        logger.reset (FileLogger::createDateStampedLogger (folder, filePrefix, ".txt",
                                                           getApplicationName() + " " + getApplicationVersion()
                                                               + "  ---  Build date: " __DATE__));
        Logger::setCurrentLogger (logger.get());
    }

    return logger != nullptr;
}

void ProjectorApplication::handleAsyncUpdate()
{
    if (licenseController != nullptr)
        licenseController->startWebviewIfNeeded();

   #if HMAC
    PopupMenu extraAppleMenuItems;
    createExtraAppleMenuItems (extraAppleMenuItems);

    // workaround broken "Open Recent" submenu: not passing the
    // submenu's title here avoids the defect in JuceMainMenuHandler::addMenuItem
    MenuBarModel::setMacMainMenu (menuModel.get(), &extraAppleMenuItems); //, "Open Recent");
   #endif

    if (getGlobalProperties().getValue (Ids::dontQueryForUpdate, {}).empty())
        LatestVersionCheckerAndUpdater::getInstance()->checkForNewVersion (false);

    if (licenseController != nullptr)
    {
        setAnalyticsEnabled (licenseController->getState().applicationUsageDataState == LicenseState::ApplicationUsageData::enabled);
        Analytics::getInstance()->logEvent ("Startup", {}, ProjectorAnalyticsEvent::appEvent);
    }

    if (! isRunningCommandLine && settings->shouldAskUserToSetHPath())
        showSetHPathAlert();
}

void ProjectorApplication::initialiseWindows (const String& commandLine)
{
    const String commandLineWithoutNSDebug (commandLine.replace ("-NSDocumentRevisionsDebugMode YES", StringRef()));

    if (commandLineWithoutNSDebug.trim().!empty() && ! commandLineWithoutNSDebug.trim().startsWithChar ('-'))
        anotherInstanceStarted (commandLine);
    else
        mainWindowList.reopenLastProjects();

    mainWindowList.createWindowIfNoneAreOpen();

    if (licenseController->getState().applicationUsageDataState == LicenseState::ApplicationUsageData::notChosenYet)
        showApplicationUsageDataAgreementPopup();
}

static void deleteTemporaryFiles()
{
    auto tempDirectory = File::getSpecialLocation (File::SpecialLocationType::tempDirectory).getChildFile ("PIPs");

    if (tempDirectory.exists())
        tempDirectory.deleteRecursively();
}

void ProjectorApplication::shutdown()
{
    if (server != nullptr)
    {
        destroyClangServer (server);
        Logger::writeToLog ("Server shutdown cleanly");
    }

    utf8Window.reset();
    svgPathWindow.reset();
    aboutWindow.reset();
    pathsWindow.reset();
    editorColourSchemeWindow.reset();
    pipCreatorWindow.reset();

    if (licenseController != nullptr)
    {
        licenseController->removeLicenseStatusChangedCallback (this);
        licenseController.reset();
    }

    mainWindowList.forceCloseAllWindows();
    openDocumentManager.clear();

    childProcessCache.reset();

   #if HMAC
    MenuBarModel::setMacMainMenu (nullptr);
   #endif

    menuModel.reset();
    commandManager.reset();
    settings.reset();

    LookAndFeel::setDefaultLookAndFeel (nullptr);

    // clean up after ourselves and delete any temp project files that may have
    // been created from PIPs
    deleteTemporaryFiles();

    if (! isRunningCommandLine)
        Logger::writeToLog ("Shutdown");

    deleteLogger();

    Analytics::getInstance()->logEvent ("Shutdown", {}, ProjectorAnalyticsEvent::appEvent);
}

struct AsyncQuitRetrier  : private Timer
{
    AsyncQuitRetrier()   { startTimer (500); }

    void timerCallback() override
    {
        stopTimer();
        delete this;

        if (auto* app = HApplicationBase::getInstance())
            app->systemRequestedQuit();
    }

    HDECLARE_NON_COPYABLE (AsyncQuitRetrier)
};

void ProjectorApplication::systemRequestedQuit()
{
    if (server != nullptr)
    {
        sendQuitMessageToIDE (server);
    }
    else if (ModalComponentManager::getInstance()->cancelAllModalComponents())
    {
        new AsyncQuitRetrier();
    }
    else
    {
        if (closeAllMainWindows())
            quit();
    }
}

//==============================================================================
void ProjectorApplication::licenseStateChanged (const LicenseState& state)
{
   #if ! HR_ENABLE_GPL_MODE
    if (state.type != LicenseState::Type::notLoggedIn
     && state.type != LicenseState::Type::noLicenseChosenYet)
   #else
    ignoreUnused (state);
   #endif
    {
        initialiseWindows (getCommandLineParameters());
    }
}

void ProjectorApplication::doLogout()
{
    if (licenseController != nullptr)
    {
        const LicenseState& state = licenseController->getState();

        if (state.type != LicenseState::Type::notLoggedIn && closeAllMainWindows())
            licenseController->logout();
    }
}

//==============================================================================
String ProjectorApplication::getVersionDescription() const
{
    String s;

    const Time buildDate (Time::getCompilationDate());

    s << "Projector " << ProjectInfo::versionString
      << newLine
      << "Build date: " << buildDate.getDayOfMonth()
      << " " << Time::getMonthName (buildDate.getMonth(), true)
      << " " << buildDate.getYear();

    return s;
}

void ProjectorApplication::anotherInstanceStarted (const String& commandLine)
{
    if (server == nullptr && ! commandLine.trim().startsWithChar ('-'))
        openFile (File (commandLine.unquoted()));
}

ProjectorApplication& ProjectorApplication::getApp()
{
    ProjectorApplication* const app = dynamic_cast<ProjectorApplication*> (HApplication::getInstance());
    HAssert (app != nullptr);
    return *app;
}

ApplicationCommandManager& ProjectorApplication::getCommandManager()
{
    auto* cm = ProjectorApplication::getApp().commandManager.get();
    HAssert (cm != nullptr);
    return *cm;
}


//==============================================================================
enum
{
    recentProjectsBaseID = 100,
    openWindowsBaseID = 300,
    activeDocumentsBaseID = 400,
    colourSchemeBaseID = 1000,
    codeEditorColourSchemeBaseID = 1500,
    showPathsID = 1999,
    examplesBaseID = 2000
};

MenuBarModel* ProjectorApplication::getMenuModel()
{
    return menuModel.get();
}

StringArray ProjectorApplication::getMenuNames()
{
    return { "File", "Edit", "View", "Build", "Window", "Document", "GUI Editor", "Tools", "Help" };
}

void ProjectorApplication::createMenu (PopupMenu& menu, const String& menuName)
{
    if (menuName == "File")             createFileMenu      (menu);
    else if (menuName == "Edit")        createEditMenu      (menu);
    else if (menuName == "View")        createViewMenu      (menu);
    else if (menuName == "Build")       createBuildMenu     (menu);
    else if (menuName == "Window")      createWindowMenu    (menu);
    else if (menuName == "Document")    createDocumentMenu  (menu);
    else if (menuName == "Tools")       createToolsMenu     (menu);
    else if (menuName == "Help")        createHelpMenu      (menu);
    else if (menuName == "GUI Editor")  createGUIEditorMenu (menu);
    else                                HAssertfalse; // names have changed?
}

void ProjectorApplication::createFileMenu (PopupMenu& menu)
{
    menu.addCommandItem (commandManager.get(), CommandIDs::newProject);
    menu.addCommandItem (commandManager.get(), CommandIDs::newProjectFromClipboard);
    menu.addCommandItem (commandManager.get(), CommandIDs::newPIP);
    menu.addSeparator();
    menu.addCommandItem (commandManager.get(), CommandIDs::open);

    {
        PopupMenu recentFiles;

        settings->recentFiles.createPopupMenuItems (recentFiles, recentProjectsBaseID, true, true);

        if (recentFiles.getNumItems() > 0)
        {
            recentFiles.addSeparator();
            recentFiles.addCommandItem (commandManager.get(), CommandIDs::clearRecentFiles);
        }

        menu.addSubMenu ("Open Recent", recentFiles);
    }

    {
        PopupMenu examples;

        createExamplesPopupMenu (examples);
        menu.addSubMenu ("Open Example", examples);
    }

    menu.addSeparator();
    menu.addCommandItem (commandManager.get(), CommandIDs::closeDocument);
    menu.addCommandItem (commandManager.get(), CommandIDs::saveDocument);
    menu.addCommandItem (commandManager.get(), CommandIDs::saveDocumentAs);
    menu.addCommandItem (commandManager.get(), CommandIDs::saveAll);
    menu.addSeparator();
    menu.addCommandItem (commandManager.get(), CommandIDs::closeProject);
    menu.addCommandItem (commandManager.get(), CommandIDs::saveProject);
    menu.addSeparator();
    menu.addCommandItem (commandManager.get(), CommandIDs::openInIDE);
    menu.addCommandItem (commandManager.get(), CommandIDs::saveAndOpenInIDE);
    menu.addSeparator();

   #if ! HR_ENABLE_GPL_MODE
    menu.addCommandItem (commandManager.get(), CommandIDs::loginLogout);
   #endif

    #if ! HMAC
      menu.addCommandItem (commandManager.get(), CommandIDs::showAboutWindow);
      menu.addCommandItem (commandManager.get(), CommandIDs::showAppUsageWindow);
      menu.addCommandItem (commandManager.get(), CommandIDs::checkForNewVersion);
      menu.addCommandItem (commandManager.get(), CommandIDs::showGlobalPathsWindow);
      menu.addSeparator();
      menu.addCommandItem (commandManager.get(), StandardApplicationCommandIDs::quit);
    #endif
}

void ProjectorApplication::createEditMenu (PopupMenu& menu)
{
    menu.addCommandItem (commandManager.get(), StandardApplicationCommandIDs::undo);
    menu.addCommandItem (commandManager.get(), StandardApplicationCommandIDs::redo);
    menu.addSeparator();
    menu.addCommandItem (commandManager.get(), StandardApplicationCommandIDs::cut);
    menu.addCommandItem (commandManager.get(), StandardApplicationCommandIDs::copy);
    menu.addCommandItem (commandManager.get(), StandardApplicationCommandIDs::paste);
    menu.addCommandItem (commandManager.get(), StandardApplicationCommandIDs::del);
    menu.addCommandItem (commandManager.get(), StandardApplicationCommandIDs::selectAll);
    menu.addCommandItem (commandManager.get(), StandardApplicationCommandIDs::deselectAll);
    menu.addSeparator();
    menu.addCommandItem (commandManager.get(), CommandIDs::showFindPanel);
    menu.addCommandItem (commandManager.get(), CommandIDs::findSelection);
    menu.addCommandItem (commandManager.get(), CommandIDs::findNext);
    menu.addCommandItem (commandManager.get(), CommandIDs::findPrevious);
}

void ProjectorApplication::createViewMenu (PopupMenu& menu)
{
    menu.addCommandItem (commandManager.get(), CommandIDs::showProjectSettings);
    menu.addCommandItem (commandManager.get(), CommandIDs::showProjectTab);
    menu.addCommandItem (commandManager.get(), CommandIDs::showBuildTab);
    menu.addCommandItem (commandManager.get(), CommandIDs::showFileExplorerPanel);
    menu.addCommandItem (commandManager.get(), CommandIDs::showModulesPanel);
    menu.addCommandItem (commandManager.get(), CommandIDs::showExportersPanel);
    menu.addCommandItem (commandManager.get(), CommandIDs::showExporterSettings);

    menu.addSeparator();
    createColourSchemeItems (menu);
}

void ProjectorApplication::createBuildMenu (PopupMenu& menu)
{
    menu.addCommandItem (commandManager.get(), CommandIDs::toggleBuildEnabled);
    menu.addCommandItem (commandManager.get(), CommandIDs::buildNow);
    menu.addCommandItem (commandManager.get(), CommandIDs::toggleContinuousBuild);
    menu.addSeparator();
    menu.addCommandItem (commandManager.get(), CommandIDs::launchApp);
    menu.addCommandItem (commandManager.get(), CommandIDs::killApp);
    menu.addCommandItem (commandManager.get(), CommandIDs::cleanAll);
    menu.addSeparator();
    menu.addCommandItem (commandManager.get(), CommandIDs::reinstantiateComp);
    menu.addCommandItem (commandManager.get(), CommandIDs::showWarnings);
    menu.addSeparator();
    menu.addCommandItem (commandManager.get(), CommandIDs::nextError);
    menu.addCommandItem (commandManager.get(), CommandIDs::prevError);
}

void ProjectorApplication::createColourSchemeItems (PopupMenu& menu)
{
    PopupMenu colourSchemes;

    colourSchemes.addItem (colourSchemeBaseID + 0, "Dark", true, selectedColourSchemeIndex == 0);
    colourSchemes.addItem (colourSchemeBaseID + 1, "Grey", true, selectedColourSchemeIndex == 1);
    colourSchemes.addItem (colourSchemeBaseID + 2, "Light", true, selectedColourSchemeIndex == 2);

    menu.addSubMenu ("Colour Scheme", colourSchemes);

    //==========================================================================
    PopupMenu editorColourSchemes;

    auto& appearanceSettings = getAppSettings().appearance;

    appearanceSettings.refreshPresetSchemeList();
    auto schemes = appearanceSettings.getPresetSchemes();

    auto i = 0;
    for (auto s : schemes)
    {
        editorColourSchemes.addItem (codeEditorColourSchemeBaseID + i, s,
                                     editorColourSchemeWindow == nullptr,
                                     selectedEditorColourSchemeIndex == i);
        ++i;
    }

    numEditorColourSchemes = i;

    editorColourSchemes.addSeparator();
    editorColourSchemes.addItem (codeEditorColourSchemeBaseID + numEditorColourSchemes,
                                 "Create...", editorColourSchemeWindow == nullptr);

    menu.addSubMenu ("Editor Colour Scheme", editorColourSchemes);
}

void ProjectorApplication::createWindowMenu (PopupMenu& menu)
{
    menu.addCommandItem (commandManager.get(), CommandIDs::goToPreviousWindow);
    menu.addCommandItem (commandManager.get(), CommandIDs::goToNextWindow);
    menu.addCommandItem (commandManager.get(), CommandIDs::closeWindow);
    menu.addSeparator();

    int counter = 0;
    for (auto* window : mainWindowList.windows)
    {
        if (window != nullptr)
        {
            if (auto* project = window->getProject())
                menu.addItem (openWindowsBaseID + counter++, project->getProjectNameString());
        }
    }

    menu.addSeparator();
    menu.addCommandItem (commandManager.get(), CommandIDs::closeAllWindows);
}

void ProjectorApplication::createDocumentMenu (PopupMenu& menu)
{
    menu.addCommandItem (commandManager.get(), CommandIDs::goToPreviousDoc);
    menu.addCommandItem (commandManager.get(), CommandIDs::goToNextDoc);
    menu.addCommandItem (commandManager.get(), CommandIDs::goToCounterpart);
    menu.addSeparator();

    auto numDocs = jmin (50, openDocumentManager.getNumOpenDocuments());

    for (int i = 0; i < numDocs; ++i)
    {
        OpenDocumentManager::Document* doc = openDocumentManager.getOpenDocument(i);
        menu.addItem (activeDocumentsBaseID + i, doc->getName());
    }

    menu.addSeparator();
    menu.addCommandItem (commandManager.get(), CommandIDs::closeAllDocuments);
}

void ProjectorApplication::createToolsMenu (PopupMenu& menu)
{
    menu.addCommandItem (commandManager.get(), CommandIDs::showUTF8Tool);
    menu.addCommandItem (commandManager.get(), CommandIDs::showSVGPathTool);
    menu.addCommandItem (commandManager.get(), CommandIDs::showTranslationTool);
}

void ProjectorApplication::createHelpMenu (PopupMenu& menu)
{
    menu.addCommandItem (commandManager.get(), CommandIDs::showForum);
    menu.addSeparator();
    menu.addCommandItem (commandManager.get(), CommandIDs::showAPIModules);
    menu.addCommandItem (commandManager.get(), CommandIDs::showAPIClasses);
    menu.addCommandItem (commandManager.get(), CommandIDs::showTutorials);
}

void ProjectorApplication::createExtraAppleMenuItems (PopupMenu& menu)
{
    menu.addCommandItem (commandManager.get(), CommandIDs::showAboutWindow);
    menu.addCommandItem (commandManager.get(), CommandIDs::showAppUsageWindow);
    menu.addCommandItem (commandManager.get(), CommandIDs::checkForNewVersion);
    menu.addSeparator();
    menu.addCommandItem (commandManager.get(), CommandIDs::showGlobalPathsWindow);
}

void ProjectorApplication::createExamplesPopupMenu (PopupMenu& menu) noexcept
{
    numExamples = 0;
    for (auto& dir : getSortedExampleDirectories())
    {
        PopupMenu m;
        for (auto& f : getSortedExampleFilesInDirectory (dir))
        {
            m.addItem (examplesBaseID + numExamples, f.getFileNameWithoutExtension());
            ++numExamples;
        }

        menu.addSubMenu (dir.getFileName(), m);
    }

    if (numExamples == 0)
    {
        menu.addItem (showPathsID, "Set path to H...");
    }
    else
    {
        menu.addSeparator();
        menu.addCommandItem (commandManager.get(), CommandIDs::launchDemoRunner);
    }
}

//==========================================================================
static File getHExamplesDirectoryPathFromGlobal()
{
    auto globalPath = File::createFileWithoutCheckingPath (getAppSettings().getStoredPath (Ids::HPath, TargetOS::getThisOS()).get().toString()
                                                                           .replace ("~", File::getSpecialLocation (File::userHomeDirectory).getFullPathName()));

    if (globalPath.exists())
        return File (globalPath).getChildFile ("examples");

    return {};
}

Array<File> ProjectorApplication::getSortedExampleDirectories() noexcept
{
    Array<File> exampleDirectories;

    auto examplesPath = getHExamplesDirectoryPathFromGlobal();

    if (! isValidHExamplesDirectory (examplesPath))
        return {};

    DirectoryIterator iter (examplesPath, false, "*", File::findDirectories);
    while (iter.next())
    {
        auto exampleDirectory = iter.getFile();

        if (exampleDirectory.getNumberOfChildFiles (File::findFiles | File::ignoreHiddenFiles) > 0
            && exampleDirectory.getFileName() != "DemoRunner" && exampleDirectory.getFileName() != "Assets")
            exampleDirectories.add (exampleDirectory);
    }

    exampleDirectories.sort();

    return exampleDirectories;
}

Array<File> ProjectorApplication::getSortedExampleFilesInDirectory (const File& directory) const noexcept
{
    Array<File> exampleFiles;

    DirectoryIterator iter (directory, false, "*.h", File::findFiles);
    while (iter.next())
        exampleFiles.add (iter.getFile());

    exampleFiles.sort();

    return exampleFiles;
}

bool ProjectorApplication::findWindowAndOpenPIP (const File& pip)
{
    auto* window = mainWindowList.getFrontmostWindow();
    bool shouldCloseWindow = false;

    if (window == nullptr)
    {
        window = mainWindowList.getOrCreateEmptyWindow();
        shouldCloseWindow = true;
    }

    if (window->tryToOpenPIP (pip))
        return true;

    if (shouldCloseWindow)
        mainWindowList.closeWindow (window);

    return false;
}

void ProjectorApplication::findAndLaunchExample (int selectedIndex)
{
    File example;

    for (auto& dir : getSortedExampleDirectories())
    {
        auto exampleFiles = getSortedExampleFilesInDirectory (dir);

        if (selectedIndex < exampleFiles.size())
        {
            example = exampleFiles.getUnchecked (selectedIndex);
            break;
        }

        selectedIndex -= exampleFiles.size();
    }

    // example doesn't exist?
    HAssert (example != File());

    findWindowAndOpenPIP (example);

    StringPairArray data;
    data.set ("label", example.getFileNameWithoutExtension());

    Analytics::getInstance()->logEvent ("Example Opened", data, ProjectorAnalyticsEvent::exampleEvent);
}

//==========================================================================
static String getPlatformSpecificFileExtension()
{
   #if HMAC
    return ".app";
   #elif HWINDOWS
    return ".exe";
   #elif HLINUX
    return {};
   #else
    HAssertfalse;
    return {};
   #endif
}

static File getPlatformSpecificProjectFolder()
{
    auto examplesDir = getHExamplesDirectoryPathFromGlobal();

    if (examplesDir == File())
        return {};

    auto buildsFolder = examplesDir.getChildFile ("DemoRunner").getChildFile ("Builds");

   #if HMAC
    return buildsFolder.getChildFile ("MacOSX");
   #elif HWINDOWS
    return buildsFolder.getChildFile ("VisualStudio2017");
   #elif HLINUX
    return buildsFolder.getChildFile ("LinuxMakefile");
   #else
    HAssertfalse;
    return {};
   #endif
}

static File tryToFindDemoRunnerExecutableInBuilds()
{
    auto projectFolder = getPlatformSpecificProjectFolder();

    if (projectFolder == File())
        return {};

   #if HMAC
    projectFolder = projectFolder.getChildFile ("build");
    auto demoRunnerExecutable = projectFolder.getChildFile ("Release").getChildFile ("DemoRunner.app");

    if (demoRunnerExecutable.exists())
        return demoRunnerExecutable;

    demoRunnerExecutable = projectFolder.getChildFile ("Debug").getChildFile ("DemoRunner.app");

    if (demoRunnerExecutable.exists())
        return demoRunnerExecutable;
   #elif HWINDOWS
    projectFolder = projectFolder.getChildFile ("x64");
    auto demoRunnerExecutable = projectFolder.getChildFile ("Release").getChildFile ("App").getChildFile ("DemoRunner.exe");

    if (demoRunnerExecutable.existsAsFile())
        return demoRunnerExecutable;

    demoRunnerExecutable = projectFolder.getChildFile ("Debug").getChildFile ("App").getChildFile ("DemoRunner.exe");

    if (demoRunnerExecutable.existsAsFile())
        return demoRunnerExecutable;
   #elif HLINUX
    projectFolder = projectFolder.getChildFile ("LinuxMakefile").getChildFile ("build");
    auto demoRunnerExecutable = projectFolder.getChildFile ("DemoRunner");

    if (demoRunnerExecutable.existsAsFile())
        return demoRunnerExecutable;
   #endif

    return {};
}

static File tryToFindPrebuiltDemoRunnerExecutable()
{
    auto prebuiltFile = File (getAppSettings().getStoredPath (Ids::HPath, TargetOS::getThisOS()).get().toString())
                               .getChildFile ("DemoRunner" + getPlatformSpecificFileExtension());

   #if HMAC
    if (prebuiltFile.exists())
   #else
    if (prebuiltFile.existsAsFile())
   #endif
        return prebuiltFile;

    return {};
}

void ProjectorApplication::checkIfGlobalHPathHasChanged()
{
    auto globalHPath = File (getAppSettings().getStoredPath (Ids::HPath, TargetOS::getThisOS()).get());

    if (lastHPath != globalHPath)
    {
        hasScannedForDemoRunnerProject = false;
        hasScannedForDemoRunnerExecutable = false;

        lastHPath = globalHPath;
    }
}

File ProjectorApplication::tryToFindDemoRunnerExecutable()
{
    checkIfGlobalHPathHasChanged();

    if (hasScannedForDemoRunnerExecutable)
        return lastDemoRunnerExectuableFile;

    hasScannedForDemoRunnerExecutable = true;

    auto demoRunnerExecutable = tryToFindDemoRunnerExecutableInBuilds();

    if (demoRunnerExecutable == File())
        demoRunnerExecutable = tryToFindPrebuiltDemoRunnerExecutable();

    lastDemoRunnerExectuableFile = demoRunnerExecutable;

    return demoRunnerExecutable;
}

File ProjectorApplication::tryToFindDemoRunnerProject()
{
    checkIfGlobalHPathHasChanged();

    if (hasScannedForDemoRunnerProject)
        return lastDemoRunnerProjectFile;

    hasScannedForDemoRunnerProject = true;

    auto projectFolder = getPlatformSpecificProjectFolder();

    if (projectFolder == File())
    {
        lastDemoRunnerProjectFile = File();
        return {};
    }

   #if HMAC
    auto demoRunnerProjectFile = projectFolder.getChildFile ("DemoRunner.xcodeproj");
   #elif HWINDOWS
    auto demoRunnerProjectFile = projectFolder.getChildFile ("DemoRunner.sln");
   #elif HLINUX
    auto demoRunnerProjectFile = projectFolder.getChildFile ("Makefile");
   #endif

   #if HMAC
    if (! demoRunnerProjectFile.exists())
   #else
    if (! demoRunnerProjectFile.existsAsFile())
   #endif
        demoRunnerProjectFile = File();

    lastDemoRunnerProjectFile = demoRunnerProjectFile;

    return demoRunnerProjectFile;
}

void ProjectorApplication::launchDemoRunner()
{
    auto demoRunnerFile = tryToFindDemoRunnerExecutable();

    if (demoRunnerFile != File())
    {
        auto succeeded = demoRunnerFile.startAsProcess();

        StringPairArray data;
        data.set ("label", succeeded ? "Success" : "Failure");

        Analytics::getInstance()->logEvent ("Launch DemoRunner", data, ProjectorAnalyticsEvent::exampleEvent);

        if (succeeded)
            return;
    }

    demoRunnerFile = tryToFindDemoRunnerProject();

    if (demoRunnerFile != File())
    {
        auto& lf = Desktop::getInstance().getDefaultLookAndFeel();

        demoRunnerAlert.reset (lf.createAlertWindow ("Open Project",
                                                     "Couldn't find a compiled version of the Demo Runner."
                                                    #if HLINUX
                                                     " Do you want to build it now?", "Build project", "Cancel",
                                                    #else
                                                     " Do you want to open the project?", "Open project", "Cancel",
                                                    #endif
                                                     {},
                                                     AlertWindow::QuestionIcon, 2,
                                                     mainWindowList.getFrontmostWindow (false)));

        demoRunnerAlert->enterModalState (true, ModalCallbackFunction::create ([this, demoRunnerFile] (int retVal)
                                                {
                                                    demoRunnerAlert.reset (nullptr);

                                                    StringPairArray data;
                                                    data.set ("label", retVal == 1 ? "Opened" : "Cancelled");

                                                    Analytics::getInstance()->logEvent ("Open DemoRunner Project", data, ProjectorAnalyticsEvent::exampleEvent);

                                                    if (retVal == 1)
                                                    {
                                                       #if HLINUX
                                                        String command ("make -C " + demoRunnerFile.getParentDirectory().getFullPathName() + " CONFIG=Release -j3");

                                                        if (! makeProcess.start (command))
                                                            AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon, "Error", "Error building Demo Runner.");
                                                       #else
                                                        demoRunnerFile.startAsProcess();
                                                       #endif
                                                    }
                                                }), false);
    }
}

//==========================================================================
void ProjectorApplication::handleMainMenuCommand (int menuItemID)
{
    if (menuItemID >= recentProjectsBaseID && menuItemID < (recentProjectsBaseID + 100))
    {
        // open a file from the "recent files" menu
        openFile (settings->recentFiles.getFile (menuItemID - recentProjectsBaseID));
    }
    else if (menuItemID >= openWindowsBaseID && menuItemID < (openWindowsBaseID + 100))
    {
        if (auto* window = mainWindowList.windows.getUnchecked (menuItemID - openWindowsBaseID))
            window->toFront (true);
    }
    else if (menuItemID >= activeDocumentsBaseID && menuItemID < (activeDocumentsBaseID + 200))
    {
        if (auto* doc = openDocumentManager.getOpenDocument (menuItemID - activeDocumentsBaseID))
            mainWindowList.openDocument (doc, true);
        else
            HAssertfalse;
    }
    else if (menuItemID >= colourSchemeBaseID && menuItemID < (colourSchemeBaseID + 3))
    {
        setColourScheme (menuItemID - colourSchemeBaseID, true);
        updateEditorColourSchemeIfNeeded();
    }
    else if (menuItemID >= codeEditorColourSchemeBaseID && menuItemID < (codeEditorColourSchemeBaseID + numEditorColourSchemes))
    {
        setEditorColourScheme (menuItemID - codeEditorColourSchemeBaseID, true);
    }
    else if (menuItemID == (codeEditorColourSchemeBaseID + numEditorColourSchemes))
    {
        showEditorColourSchemeWindow();
    }
    else if (menuItemID == showPathsID)
    {
        showPathsWindow (true);
    }
    else if (menuItemID >= examplesBaseID && menuItemID < (examplesBaseID + numExamples))
    {
        findAndLaunchExample (menuItemID - examplesBaseID);
    }
    else
    {
        handleGUIEditorMenuCommand (menuItemID);
    }
}

//==============================================================================
void ProjectorApplication::getAllCommands (Array <CommandID>& commands)
{
    HApplication::getAllCommands (commands);

    const CommandID ids[] = { CommandIDs::newProject,
                              CommandIDs::newProjectFromClipboard,
                              CommandIDs::newPIP,
                              CommandIDs::open,
                              CommandIDs::launchDemoRunner,
                              CommandIDs::closeAllWindows,
                              CommandIDs::closeAllDocuments,
                              CommandIDs::clearRecentFiles,
                              CommandIDs::saveAll,
                              CommandIDs::showGlobalPathsWindow,
                              CommandIDs::showUTF8Tool,
                              CommandIDs::showSVGPathTool,
                              CommandIDs::showAboutWindow,
                              CommandIDs::showAppUsageWindow,
                              CommandIDs::checkForNewVersion,
                              CommandIDs::showForum,
                              CommandIDs::showAPIModules,
                              CommandIDs::showAPIClasses,
                              CommandIDs::showTutorials,
                              CommandIDs::loginLogout };

    commands.addArray (ids, numElementsInArray (ids));
}

void ProjectorApplication::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::newProject:
        result.setInfo ("New Project...", "Creates a new H project", CommandCategories::general, 0);
        result.defaultKeypresses.add (KeyPress ('n', ModifierKeys::commandModifier, 0));
        break;

    case CommandIDs::newProjectFromClipboard:
        result.setInfo ("New Project From Clipboard...", "Creates a new H project from the clipboard contents", CommandCategories::general, 0);
        result.defaultKeypresses.add (KeyPress ('n', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;

    case CommandIDs::newPIP:
        result.setInfo ("New PIP...", "Opens the PIP Creator utility for creating a new PIP", CommandCategories::general, 0);
        result.defaultKeypresses.add (KeyPress ('p', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;

    case CommandIDs::launchDemoRunner:
       #if HLINUX
        if (makeProcess.isRunning())
        {
            result.setInfo ("Building Demo Runner...", "The Demo Runner project is currently building", CommandCategories::general, 0);
            result.setActive (false);
        }
        else
       #endif
        {
            result.setInfo ("Launch Demo Runner", "Launches the H demo runner application, or the project if it can't be found", CommandCategories::general, 0);
            result.setActive (tryToFindDemoRunnerExecutable() != File() || tryToFindDemoRunnerProject() != File());
        }
        break;

    case CommandIDs::open:
        result.setInfo ("Open...", "Opens a H project", CommandCategories::general, 0);
        result.defaultKeypresses.add (KeyPress ('o', ModifierKeys::commandModifier, 0));
        break;

    case CommandIDs::showGlobalPathsWindow:
        result.setInfo ("Global Paths...",
                        "Shows the window to change the stored global paths.",
                        CommandCategories::general, 0);
        break;

    case CommandIDs::closeAllWindows:
        result.setInfo ("Close All Windows", "Closes all open windows", CommandCategories::general, 0);
        result.setActive (mainWindowList.windows.size() > 0);
        break;

    case CommandIDs::closeAllDocuments:
        result.setInfo ("Close All Documents", "Closes all open documents", CommandCategories::general, 0);
        result.setActive (openDocumentManager.getNumOpenDocuments() > 0);
        break;

    case CommandIDs::clearRecentFiles:
        result.setInfo ("Clear Recent Files", "Clears all recent files from the menu", CommandCategories::general, 0);
        result.setActive (settings->recentFiles.getNumFiles() > 0);
        break;

    case CommandIDs::saveAll:
        result.setInfo ("Save All", "Saves all open documents", CommandCategories::general, 0);
        result.defaultKeypresses.add (KeyPress ('s', ModifierKeys::commandModifier | ModifierKeys::altModifier, 0));
        break;

    case CommandIDs::showUTF8Tool:
        result.setInfo ("UTF-8 String-Literal Helper", "Shows the UTF-8 string literal utility", CommandCategories::general, 0);
        break;

    case CommandIDs::showSVGPathTool:
        result.setInfo ("SVG Path Converter", "Shows the SVG->Path data conversion utility", CommandCategories::general, 0);
        break;

    case CommandIDs::showAboutWindow:
        result.setInfo ("About Projector", "Shows the Projector's 'About' page.", CommandCategories::general, 0);
        break;

    case CommandIDs::showAppUsageWindow:
        result.setInfo ("Application Usage Data", "Shows the application usage data agreement window", CommandCategories::general, 0);
        break;

    case CommandIDs::checkForNewVersion:
        result.setInfo ("Check for New Version...", "Checks the web server for a new version of H", CommandCategories::general, 0);
        break;

    case CommandIDs::showForum:
        result.setInfo ("H Community Forum", "Shows the H community forum in a browser", CommandCategories::general, 0);
        break;

    case CommandIDs::showAPIModules:
        result.setInfo ("API Modules", "Shows the API modules documentation in a browser", CommandCategories::general, 0);
        break;

    case CommandIDs::showAPIClasses:
        result.setInfo ("API Classes", "Shows the API classes documentation in a browser", CommandCategories::general, 0);
        break;

    case CommandIDs::showTutorials:
        result.setInfo ("H Tutorials", "Shows the H tutorials in a browser", CommandCategories::general, 0);
        break;

    case CommandIDs::loginLogout:
        {
            bool isLoggedIn = false;
            String username;

            if (licenseController != nullptr)
            {
                const LicenseState state = licenseController->getState();
                isLoggedIn = (state.type != LicenseState::Type::notLoggedIn && state.type != LicenseState::Type::GPL);
                username = state.username;
            }

            result.setInfo (isLoggedIn
                               ? String ("Sign out ") + username + "..."
                               : String ("Sign in..."),
                            "Log out of your H account", CommandCategories::general, 0);
        }
        break;

    default:
        HApplication::getCommandInfo (commandID, result);
        break;
    }
}

bool ProjectorApplication::perform (const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::newProject:                createNewProject(); break;
        case CommandIDs::newProjectFromClipboard:   createNewProjectFromClipboard(); break;
        case CommandIDs::newPIP:                    createNewPIP(); break;
        case CommandIDs::open:                      askUserToOpenFile(); break;
        case CommandIDs::launchDemoRunner:          launchDemoRunner(); break;
        case CommandIDs::saveAll:                   saveAllDocuments(); break;
        case CommandIDs::closeAllWindows:           closeAllMainWindowsAndQuitIfNeeded(); break;
        case CommandIDs::closeAllDocuments:         closeAllDocuments (true); break;
        case CommandIDs::clearRecentFiles:          clearRecentFiles(); break;
        case CommandIDs::showUTF8Tool:              showUTF8ToolWindow(); break;
        case CommandIDs::showSVGPathTool:           showSVGPathDataToolWindow(); break;
        case CommandIDs::showGlobalPathsWindow:     showPathsWindow (false); break;
        case CommandIDs::showAboutWindow:           showAboutWindow(); break;
        case CommandIDs::showAppUsageWindow:        showApplicationUsageDataAgreementPopup(); break;
        case CommandIDs::checkForNewVersion:        LatestVersionCheckerAndUpdater::getInstance()->checkForNewVersion (true); break;
        case CommandIDs::showForum:                 launchForumBrowser(); break;
        case CommandIDs::showAPIModules:            launchModulesBrowser(); break;
        case CommandIDs::showAPIClasses:            launchClassesBrowser(); break;
        case CommandIDs::showTutorials:             launchTutorialsBrowser(); break;
        case CommandIDs::loginLogout:               doLogout(); break;
        default:                                    return HApplication::perform (info);
    }

    return true;
}

//==============================================================================
void ProjectorApplication::createNewProject()
{
    auto* mw = mainWindowList.getOrCreateEmptyWindow();
    mw->showStartPage();

    mainWindowList.avoidSuperimposedWindows (mw);
}

void ProjectorApplication::createNewProjectFromClipboard()
{
    auto tempFile = File::getSpecialLocation (File::SpecialLocationType::tempDirectory).getChildFile ("PIPs").getChildFile ("Clipboard")
                                                                                       .getChildFile ("PIPFile_" + String (std::abs (Random::getSystemRandom().nextInt())) + ".h");

    if (tempFile.existsAsFile())
        tempFile.deleteFile();

    tempFile.create();
    tempFile.appendText (SystemClipboard::getTextFromClipboard());

    if (! findWindowAndOpenPIP (tempFile))
    {
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon, "Error", "Couldn't create project from clipboard contents.");
        tempFile.deleteFile();
    }
}

void ProjectorApplication::createNewPIP()
{
    showPIPCreatorWindow();
}

void ProjectorApplication::askUserToOpenFile()
{
    FileChooser fc ("Open File");

    if (fc.browseForFileToOpen())
        openFile (fc.getResult());
}

bool ProjectorApplication::openFile (const File& file)
{
    return mainWindowList.openFile (file);
}

void ProjectorApplication::saveAllDocuments()
{
    openDocumentManager.saveAll();

    for (int i = 0; i < mainWindowList.windows.size(); ++i)
        if (auto* pcc = mainWindowList.windows.getUnchecked(i)->getProjectContentComponent())
            pcc->refreshProjectTreeFileStatuses();
}

bool ProjectorApplication::closeAllDocuments (bool askUserToSave)
{
    return openDocumentManager.closeAll (askUserToSave);
}

bool ProjectorApplication::closeAllMainWindows()
{
    return server != nullptr || mainWindowList.askAllWindowsToClose();
}

void ProjectorApplication::closeAllMainWindowsAndQuitIfNeeded()
{
    if (closeAllMainWindows())
    {
       #if ! HMAC
        if (mainWindowList.windows.size() == 0)
            systemRequestedQuit();
       #endif
    }
}

void ProjectorApplication::clearRecentFiles()
{
    settings->recentFiles.clear();
    settings->recentFiles.clearRecentFilesNatively();
    settings->flush();
    menuModel->menuItemsChanged();
}

//==============================================================================
void ProjectorApplication::showUTF8ToolWindow()
{
    if (utf8Window != nullptr)
        utf8Window->toFront (true);
    else
        new FloatingToolWindow ("UTF-8 String Literal Converter", "utf8WindowPos",
                                new UTF8Component(), utf8Window, true,
                                500, 500, 300, 300, 1000, 1000);
}

void ProjectorApplication::showSVGPathDataToolWindow()
{
    if (svgPathWindow != nullptr)
        svgPathWindow->toFront (true);
    else
        new FloatingToolWindow ("SVG Path Converter", "svgPathWindowPos",
                                new SVGPathDataComponent(), svgPathWindow, true,
                                500, 500, 300, 300, 1000, 1000);
}

void ProjectorApplication::showAboutWindow()
{
    if (aboutWindow != nullptr)
        aboutWindow->toFront (true);
    else
        new FloatingToolWindow ({}, {}, new AboutWindowComponent(),
                                aboutWindow, false,
                                500, 300, 500, 300, 500, 300);
}

void ProjectorApplication::showApplicationUsageDataAgreementPopup()
{
    if (applicationUsageDataWindow != nullptr)
        applicationUsageDataWindow->toFront (true);
    else
        new FloatingToolWindow ("Application Usage Analytics", {},
                                new ApplicationUsageDataWindowComponent (isPaidOrGPL()), applicationUsageDataWindow, false,
                                400, 300, 400, 300, 400, 300);
}

void ProjectorApplication::dismissApplicationUsageDataAgreementPopup()
{
    if (applicationUsageDataWindow != nullptr)
        applicationUsageDataWindow.reset();
}

void ProjectorApplication::showPathsWindow (bool highlightHPath)
{
    if (pathsWindow != nullptr)
        pathsWindow->toFront (true);
    else
        new FloatingToolWindow ("Global Paths", "pathsWindowPos",
                                new GlobalPathsWindowComponent(), pathsWindow, false,
                                600, 700, 600, 700, 600, 700);

    if (highlightHPath)
        if (auto* pathsComp = dynamic_cast<GlobalPathsWindowComponent*> (pathsWindow->getChildComponent (0)))
            pathsComp->highlightHPath();
}

void ProjectorApplication::showEditorColourSchemeWindow()
{
    if (editorColourSchemeWindow != nullptr)
        editorColourSchemeWindow->toFront (true);
    else
        new FloatingToolWindow ("Editor Colour Scheme", "editorColourSchemeWindowPos",
                                new EditorColourSchemeWindowComponent(), editorColourSchemeWindow, false,
                                500, 500, 500, 500, 500, 500);
}

void ProjectorApplication::showPIPCreatorWindow()
{
    if (pipCreatorWindow != nullptr)
        pipCreatorWindow->toFront (true);
    else
        new FloatingToolWindow ("PIP Creator", "pipCreatorWindowPos",
                                new PIPCreatorWindowComponent(), pipCreatorWindow, false,
                                600, 750, 600, 750, 600, 750);
}

void ProjectorApplication::launchForumBrowser()
{
    URL forumLink ("https://forum.H.com/");

    if (forumLink.isWellFormed())
        forumLink.launchInDefaultBrowser();
}

void ProjectorApplication::launchModulesBrowser()
{
    URL modulesLink ("https://docs.H.com/master/modules.html");

    if (modulesLink.isWellFormed())
        modulesLink.launchInDefaultBrowser();
}

void ProjectorApplication::launchClassesBrowser()
{
    URL classesLink ("https://docs.H.com/master/classes.html");

    if (classesLink.isWellFormed())
        classesLink.launchInDefaultBrowser();
}

void ProjectorApplication::launchTutorialsBrowser()
{
    URL tutorialsLink ("https://H.com/learn/tutorials");

    if (tutorialsLink.isWellFormed())
        tutorialsLink.launchInDefaultBrowser();
}

//==============================================================================
struct FileWithTime
{
    FileWithTime (const File& f) : file (f), time (f.getLastModificationTime()) {}
    FileWithTime() {}

    bool operator<  (const FileWithTime& other) const    { return time <  other.time; }
    bool operator== (const FileWithTime& other) const    { return time == other.time; }

    File file;
    Time time;
};

void ProjectorApplication::deleteLogger()
{
    const int maxNumLogFilesToKeep = 50;

    Logger::setCurrentLogger (nullptr);

    if (logger != nullptr)
    {
        auto logFiles = logger->getLogFile().getParentDirectory().findChildFiles (File::findFiles, false);

        if (logFiles.size() > maxNumLogFilesToKeep)
        {
            Array<FileWithTime> files;

            for (auto& f : logFiles)
                files.addUsingDefaultSort (f);

            for (int i = 0; i < files.size() - maxNumLogFilesToKeep; ++i)
                files.getReference(i).file.deleteFile();
        }
    }

    logger.reset();
}

PropertiesFile::Options ProjectorApplication::getPropertyFileOptionsFor (const String& filename, bool isProjectSettings)
{
    PropertiesFile::Options options;
    options.applicationName     = filename;
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Application Support";
   #if HLINUX
    options.folderName          = "~/.config/Projector";
   #else
    options.folderName          = "Projector";
   #endif

    if (isProjectSettings)
        options.folderName += "/ProjectSettings";

    return options;
}

void ProjectorApplication::updateAllBuildTabs()
{
    for (int i = 0; i < mainWindowList.windows.size(); ++i)
        if (ProjectContentComponent* p = mainWindowList.windows.getUnchecked(i)->getProjectContentComponent())
            p->rebuildProjectTabs();
}

void ProjectorApplication::initCommandManager()
{
    commandManager.reset (new ApplicationCommandManager());
    commandManager->registerAllCommandsForTarget (this);

    {
        CodeDocument doc;
        CppCodeEditorComponent ed (File(), doc);
        commandManager->registerAllCommandsForTarget (&ed);
    }

    registerGUIEditorCommands();
}

void ProjectorApplication::setAnalyticsEnabled (bool enabled)
{
    resetAnalytics();

    if (enabled)
        setupAnalytics();
}

void ProjectorApplication::resetAnalytics() noexcept
{
    auto analyticsInstance = Analytics::getInstance();

    analyticsInstance->setUserId ({});
    analyticsInstance->setUserProperties ({});
    analyticsInstance->getDestinations().clear();
}

void ProjectorApplication::setupAnalytics()
{
    Analytics::getInstance()->addDestination (new ProjectorAnalyticsDestination());

    auto deviceString = SystemStats::getDeviceIdentifiers().joinIntoString (":");
    auto deviceIdentifier = String::toHexString (deviceString.hashCode64());

    Analytics::getInstance()->setUserId (deviceIdentifier);

    StringPairArray userData;
    userData.set ("cd1", getApplicationName());
    userData.set ("cd2", getApplicationVersion());
    userData.set ("cd3", SystemStats::getDeviceDescription());
    userData.set ("cd4", deviceString);
    userData.set ("cd5", SystemStats::getOperatingSystemName());

    Analytics::getInstance()->setUserProperties (userData);
}

void ProjectorApplication::showSetHPathAlert()
{
    auto& lf = Desktop::getInstance().getDefaultLookAndFeel();
    pathAlert.reset (lf.createAlertWindow ("Set H Path", "Your global H path is invalid. This path is used to access the H examples and demo project - "
                                           "would you like to set it now?",
                                           "Set path", "Cancel", "Don't ask again",
                                           AlertWindow::WarningIcon, 3,
                                           mainWindowList.getFrontmostWindow (false)));

    pathAlert->enterModalState (true, ModalCallbackFunction::create ([this] (int retVal)
                                                                    {
                                                                        pathAlert.reset (nullptr);

                                                                        if (retVal == 1)
                                                                            showPathsWindow (true);
                                                                        else if (retVal == 0)
                                                                            settings->setDontAskAboutHPathAgain();
                                                                    }));

}

void rescanModules (AvailableModuleList& list, const Array<File>& paths, bool async)
{
    if (async)
        list.scanPathsAsync (paths);
    else
        list.scanPaths (paths);
}

void ProjectorApplication::rescanHPathModules()
{
    rescanModules (HPathModuleList, { getAppSettings().getStoredPath (Ids::defaultJuceModulePath, TargetOS::getThisOS()).get().toString() }, ! isRunningCommandLine);
}

void ProjectorApplication::rescanUserPathModules()
{
    rescanModules (userPathsModuleList, { getAppSettings().getStoredPath (Ids::defaultUserModulePath, TargetOS::getThisOS()).get().toString() }, ! isRunningCommandLine);
}

void ProjectorApplication::selectEditorColourSchemeWithName (const String& schemeName)
{
    auto& appearanceSettings = getAppSettings().appearance;
    auto schemes = appearanceSettings.getPresetSchemes();

    auto schemeIndex = schemes.indexOf (schemeName);

    if (schemeIndex >= 0)
        setEditorColourScheme (schemeIndex, true);
}

void ProjectorApplication::setColourScheme (int index, bool saveSetting)
{
    switch (index)
    {
        case 0: lookAndFeel.setColourScheme (LookAndFeel_V4::getDarkColourScheme());  break;
        case 1: lookAndFeel.setColourScheme (LookAndFeel_V4::getGreyColourScheme());  break;
        case 2: lookAndFeel.setColourScheme (LookAndFeel_V4::getLightColourScheme()); break;
        default: break;
    }

    lookAndFeel.setupColours();
    mainWindowList.sendLookAndFeelChange();

    if (utf8Window != nullptr)                  utf8Window->sendLookAndFeelChange();
    if (svgPathWindow != nullptr)               svgPathWindow->sendLookAndFeelChange();
    if (aboutWindow != nullptr)                 aboutWindow->sendLookAndFeelChange();
    if (applicationUsageDataWindow != nullptr)  applicationUsageDataWindow->sendLookAndFeelChange();
    if (pathsWindow != nullptr)                 pathsWindow->sendLookAndFeelChange();
    if (editorColourSchemeWindow != nullptr)    editorColourSchemeWindow->sendLookAndFeelChange();
    if (pipCreatorWindow != nullptr)            pipCreatorWindow->sendLookAndFeelChange();

    auto* mcm = ModalComponentManager::getInstance();
    for (auto i = 0; i < mcm->getNumModalComponents(); ++i)
        mcm->getModalComponent (i)->sendLookAndFeelChange();

    if (saveSetting)
    {
        auto& properties = settings->getGlobalProperties();
        properties.setValue ("COLOUR SCHEME", index);
    }

    selectedColourSchemeIndex = index;

    getCommandManager().commandStatusChanged();
}

void ProjectorApplication::setEditorColourScheme (int index, bool saveSetting)
{
    auto& appearanceSettings = getAppSettings().appearance;
    auto schemes = appearanceSettings.getPresetSchemes();

    index = jmin (index, schemes.size() - 1);

    appearanceSettings.selectPresetScheme (index);

    if (saveSetting)
    {
        auto& properties = settings->getGlobalProperties();
        properties.setValue ("EDITOR COLOUR SCHEME", index);
    }

    selectedEditorColourSchemeIndex = index;

    getCommandManager().commandStatusChanged();
}

bool ProjectorApplication::isEditorColourSchemeADefaultScheme (const StringArray& schemes, int editorColourSchemeIndex)
{
    auto& schemeName = schemes[editorColourSchemeIndex];
    return (schemeName == "Default (Dark)" || schemeName == "Default (Light)");
}

int ProjectorApplication::getEditorColourSchemeForGUIColourScheme (const StringArray& schemes, int guiColourSchemeIndex)
{
    auto defaultDarkEditorIndex  = schemes.indexOf ("Default (Dark)");
    auto defaultLightEditorIndex = schemes.indexOf ("Default (Light)");

    // Can't find default code editor colour schemes!
    HAssert (defaultDarkEditorIndex != -1 && defaultLightEditorIndex != -1);

    return (guiColourSchemeIndex == 2 ? defaultLightEditorIndex : defaultDarkEditorIndex);
}

void ProjectorApplication::updateEditorColourSchemeIfNeeded()
{
    auto& appearanceSettings = getAppSettings().appearance;
    auto schemes = appearanceSettings.getPresetSchemes();

    if (isEditorColourSchemeADefaultScheme (schemes, selectedEditorColourSchemeIndex))
        setEditorColourScheme (getEditorColourSchemeForGUIColourScheme (schemes, selectedColourSchemeIndex), true);
}
