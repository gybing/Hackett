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

#include "MainWindow.h"
#include "../Project/Module.h"
#include "AutoUpdater.h"
#include "../CodeEditor/SourceCodeEditor.h"
#include "../Utility/UI/ProjectorLookAndFeel.h"
#include "../Licenses/LicenseController.h"

#if HMODULE_AVAILABLE_analytics
 #include "ProjectorAnalytics.h"
#endif

struct ChildProcessCache;

//==============================================================================
class ProjectorApplication   : public HApplication,
                              private AsyncUpdater,
                              private LicenseController::StateChangedCallback
{
public:
    ProjectorApplication();

    static ProjectorApplication& getApp();
    static ApplicationCommandManager& getCommandManager();

    //==============================================================================
    void initialise (const String& commandLine) override;
    void initialiseBasics();
    bool initialiseLogger (const char* filePrefix);
    void initialiseWindows (const String& commandLine);

    void shutdown() override;
    void systemRequestedQuit() override;
    void deleteLogger();

    //==============================================================================
    const String getApplicationName() override       { return "Projector"; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }

    String getVersionDescription() const;
    bool moreThanOneInstanceAllowed() override       { return true; } // this is handled manually in initialise()

    void anotherInstanceStarted (const String& commandLine) override;

    //==============================================================================
    MenuBarModel* getMenuModel();
    StringArray getMenuNames();
    void createMenu (PopupMenu&, const String& menuName);
    void createFileMenu (PopupMenu&);
    void createEditMenu (PopupMenu&);
    void createViewMenu (PopupMenu&);
    void createBuildMenu (PopupMenu&);
    void createColourSchemeItems (PopupMenu&);
    void createWindowMenu (PopupMenu&);
    void createDocumentMenu (PopupMenu&);
    void createToolsMenu (PopupMenu&);
    void createHelpMenu (PopupMenu&);
    void createExtraAppleMenuItems (PopupMenu&);
    void handleMainMenuCommand (int menuItemID);

    //==============================================================================
    void getAllCommands (Array<CommandID>&) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo&) override;
    bool perform (const InvocationInfo&) override;

    //==============================================================================
    void createNewProject();
    void createNewProjectFromClipboard();
    void createNewPIP();
    void askUserToOpenFile();
    bool openFile (const File&);
    void saveAllDocuments();
    bool closeAllDocuments (bool askUserToSave);
    bool closeAllMainWindows();
    void closeAllMainWindowsAndQuitIfNeeded();
    void clearRecentFiles();

    PropertiesFile::Options getPropertyFileOptionsFor (const String& filename, bool isProjectSettings);

    //==============================================================================
    void showUTF8ToolWindow();
    void showSVGPathDataToolWindow();

    void showAboutWindow();
    void showApplicationUsageDataAgreementPopup();
    void dismissApplicationUsageDataAgreementPopup();

    void showPathsWindow (bool highlightHPath = false);
    void showEditorColourSchemeWindow();

    void showPIPCreatorWindow();

    void launchForumBrowser();
    void launchModulesBrowser();
    void launchClassesBrowser();
    void launchTutorialsBrowser();

    void updateAllBuildTabs();

    //==============================================================================
    void licenseStateChanged (const LicenseState&) override;
    void doLogout();

    bool isPaidOrGPL() const              { return licenseController == nullptr || licenseController->getState().isPaidOrGPL(); }

    //==============================================================================
    void selectEditorColourSchemeWithName (const String& schemeName);
    static bool isEditorColourSchemeADefaultScheme (const StringArray& schemes, int editorColourSchemeIndex);
    static int getEditorColourSchemeForGUIColourScheme (const StringArray& schemes, int guiColourSchemeIndex);

    //==============================================================================
    void setAnalyticsEnabled (bool);

    //==============================================================================
    void rescanHPathModules();
    void rescanUserPathModules();

    AvailableModuleList& getHPathModuleList()     { return HPathModuleList; }
    AvailableModuleList& getUserPathsModuleList()    { return userPathsModuleList; }

    //==============================================================================
    ProjectorLookAndFeel lookAndFeel;

    std::unique_ptr<StoredSettings> settings;
    std::unique_ptr<Icons> icons;

    struct MainMenuModel;
    std::unique_ptr<MainMenuModel> menuModel;

    MainWindowList mainWindowList;
    OpenDocumentManager openDocumentManager;
    std::unique_ptr<ApplicationCommandManager> commandManager;

    std::unique_ptr<Component> utf8Window, svgPathWindow, aboutWindow, applicationUsageDataWindow,
                               pathsWindow, editorColourSchemeWindow, pipCreatorWindow;

    std::unique_ptr<FileLogger> logger;

    bool isRunningCommandLine;
    std::unique_ptr<ChildProcessCache> childProcessCache;
    std::unique_ptr<LicenseController> licenseController;

private:
    //==============================================================================
    void handleAsyncUpdate() override;
    void initCommandManager();

    void createExamplesPopupMenu (PopupMenu&) noexcept;
    Array<File> getSortedExampleDirectories() noexcept;
    Array<File> getSortedExampleFilesInDirectory (const File&) const noexcept;
    bool findWindowAndOpenPIP (const File&);
    void findAndLaunchExample (int);

    void checkIfGlobalHPathHasChanged();
    File tryToFindDemoRunnerExecutable();
    File tryToFindDemoRunnerProject();
    void launchDemoRunner();

    void resetAnalytics() noexcept;
    void setupAnalytics();

    void showSetHPathAlert();

    void setColourScheme (int index, bool saveSetting);
    void setEditorColourScheme (int index, bool saveSetting);
    void updateEditorColourSchemeIfNeeded();

    //==============================================================================
    void* server = nullptr;

    TooltipWindow tooltipWindow;

    AvailableModuleList HPathModuleList, userPathsModuleList;

    int numExamples = 0;
    std::unique_ptr<AlertWindow> demoRunnerAlert;
    std::unique_ptr<AlertWindow> pathAlert;
    bool hasScannedForDemoRunnerExecutable = false, hasScannedForDemoRunnerProject = false;
    File lastHPath, lastDemoRunnerExectuableFile, lastDemoRunnerProjectFile;
   #if HLINUX
    ChildProcess makeProcess;
   #endif

    int selectedColourSchemeIndex = 0, selectedEditorColourSchemeIndex = 0, numEditorColourSchemes = 0;

    //==============================================================================
    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProjectorApplication)
};
