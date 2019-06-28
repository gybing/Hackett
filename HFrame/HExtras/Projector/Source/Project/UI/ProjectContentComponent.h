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

#include "../../CodeEditor/OpenDocumentManager.h"

class CompileEngineChildProcess;
class ProjectTab;
class LiveBuildTab;
class HeaderComponent;

//==============================================================================
class ProjectContentComponent  : public Component,
                                 public ApplicationCommandTarget,
                                 private ChangeListener,
                                 private OpenDocumentManager::DocumentCloseListener,
                                 private FocusChangeListener,
                                 private Timer
{
public:
    //==============================================================================
    ProjectContentComponent();
    ~ProjectContentComponent() override;

    Project* getProject() const noexcept    { return project; }
    virtual void setProject (Project*);

    void saveTreeViewState();
    void saveOpenDocumentList();
    void reloadLastOpenDocuments();

    bool showEditorForFile (const File&, bool grabFocus);
    bool hasFileInRecentList (const File&) const;
    File getCurrentFile() const;

    bool showDocument (OpenDocumentManager::Document*, bool grabFocus);
    void hideDocument (OpenDocumentManager::Document*);
    OpenDocumentManager::Document* getCurrentDocument() const    { return currentDocument; }
    void closeDocument();
    void saveDocument();
    void saveAs();

    void hideEditor();
    bool setEditorComponent (Component* editor, OpenDocumentManager::Document* doc);
    Component* getEditorComponentContent() const;
    Component* getEditorComponent() const    { return contentView.get(); }
    Component& getSidebarComponent()         { return sidebarTabs; }

    bool goToPreviousFile();
    bool goToNextFile();
    bool canGoToCounterpart() const;
    bool goToCounterpart();

    bool saveProject (bool shouldWait = false, bool openInIDE = false);
    void closeProject();
    void openInSelectedIDE (bool saveFirst);
    void showNewExporterMenu();

    void showProjectTab()       { sidebarTabs.setCurrentTabIndex (0); }
    void showBuildTab()         { sidebarTabs.setCurrentTabIndex (1); }
    int getCurrentTabIndex()    { return sidebarTabs.getCurrentTabIndex(); }

    void showFilesPanel()        { showProjectPanel (0); }
    void showModulesPanel()      { showProjectPanel (1); }
    void showExportersPanel()    { showProjectPanel (2); }

    void showProjectSettings();
    void showCurrentExporterSettings();
    void showExporterSettings (const String& exporterName);
    void showModule (const String& moduleID);
    void showLiveBuildSettings();
    void showUserSettings();

    void deleteSelectedTreeItems();

    void refreshProjectTreeFileStatuses();
    void updateMissingFileStatuses();
    void createProjectTabs();
    void deleteProjectTabs();
    void rebuildProjectTabs();
    void refreshTabsIfBuildStatusChanged();
    void toggleWarnings();
    void showNextError();
    void showPreviousError();
    void reinstantiateLivePreviewWindows();

    void showBubbleMessage (Rectangle<int>, const String&);

    StringArray getExportersWhichCanLaunch() const;

    static void getSelectedProjectItemsBeingDragged (const DragAndDropTarget::SourceDetails&,
                                                     OwnedArray<Project::Item>& selectedNodes);

    //==============================================================================
    void killChildProcess();
    void cleanAll();
    void handleMissingSystemHeaders();
    bool isBuildTabEnabled() const;
    void setBuildEnabled (bool enabled, bool displayError = false);
    bool isBuildEnabled() const;
    bool areWarningsEnabled() const;

    //==============================================================================
    ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands (Array<CommandID>&) override;
    void getCommandInfo (CommandID, ApplicationCommandInfo&) override;
    bool perform (const InvocationInfo&) override;

    bool isSaveCommand (const CommandID id);

    void paint (Graphics&) override;
    void resized() override;
    void childBoundsChanged (Component*) override;
    void lookAndFeelChanged() override;

    String lastCrashMessage;

private:
    friend HeaderComponent;

    //==============================================================================
    Project* project = nullptr;
    OpenDocumentManager::Document* currentDocument = nullptr;
    RecentDocumentList recentDocumentList;
    std::unique_ptr<Component> logo, translationTool, contentView, header;

    TabbedComponent sidebarTabs  { TabbedButtonBar::TabsAtTop };
    std::unique_ptr<ResizableEdgeComponent> resizerBar;
    ComponentBoundsConstrainer sidebarSizeConstrainer;

    BubbleMessageComponent bubbleMessage;
    ReferenceCountedObjectPtr<CompileEngineChildProcess> childProcess;
    bool isForeground = false;

    std::unique_ptr<Label> fileNameLabel;

    int lastViewedTab = 0;

    //==============================================================================
    bool documentAboutToClose (OpenDocumentManager::Document*) override;
    void changeListenerCallback (ChangeBroadcaster*) override;
    void showTranslationTool();

    void globalFocusChanged (Component*) override;
    void timerCallback() override;

    bool isContinuousRebuildEnabled();
    void setContinuousRebuildEnabled (bool b);

    void rebuildNow();
    void handleCrash (const String& message);
    void updateWarningState();
    void launchApp();
    void killApp();

    ReferenceCountedObjectPtr<CompileEngineChildProcess> getChildProcess();

    //==============================================================================
    void showProjectPanel (const int index);
    ProjectTab* getProjectTab();
    LiveBuildTab* getLiveBuildTab();
    bool canSelectedProjectBeLaunch();

    //==============================================================================
    struct ContentViewport  : public Component
    {
        ContentViewport (Component* content)
        {
            addAndMakeVisible (viewport);
            viewport.setViewedComponent (content, true);
        }

        void resized() override
        {
            viewport.setBounds (getLocalBounds());
        }

        Viewport viewport;

        HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ContentViewport)
    };

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProjectContentComponent)
};
