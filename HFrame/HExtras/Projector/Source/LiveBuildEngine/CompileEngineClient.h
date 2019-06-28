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

#include "ActivityList.h"
#include "ErrorList.h"
class Project;

//==============================================================================
class CompileEngineChildProcess  : public ReferenceCountedObject,
                                   private OpenDocumentManager::DocumentCloseListener
{
public:
    CompileEngineChildProcess (Project&);
    ~CompileEngineChildProcess() override;

    //==============================================================================
    bool openedOk() const       { return process != nullptr; }

    void editorOpened (const File& file, CodeDocument& document);
    bool documentAboutToClose (OpenDocumentManager::Document*) override;

    //==============================================================================
    void cleanAll();
    void openPreview (const ClassDatabase::Class&);
    void reinstantiatePreviews();
    void processActivationChanged (bool isForeground);

    //==============================================================================
    bool canLaunchApp() const;
    void launchApp();
    bool canKillApp() const;
    void killApp();
    bool isAppRunning() const noexcept;

    //==============================================================================
    const ClassDatabase::ClassList& getComponentList() const        { return lastComponentList; }

    //==============================================================================
    void flushEditorChanges();
    static void cleanAllCachedFilesForProject (Project&);

    //==============================================================================
    Project& project;
    ActivityList activityList;
    ErrorList errorList;

    //==============================================================================
    std::function<void (const String&)> crashHandler;

    //==============================================================================
    // from server..
    void handleNewDiagnosticList (const ValueTree& newList);
    void handleClearErrors();
    void handleActivityListChanged (const StringArray&);
    void handleClassListChanged (const ValueTree& newList);
    void handleBuildFailed();
    void handleChangeCode (const SourceCodeRange& location, const String& newText);
    void handleAppLaunched();
    void handleAppQuit();
    void handleHighlightCode (const SourceCodeRange& location);
    void handlePing();
    void handleCrash (const String& message);
    void handleCloseIDE();
    void handleKeyPress (const String& className, const KeyPress& key);
    void handleUndoInEditor (const String& className);
    void handleRedoInEditor (const String& className);
    void handleMissingSystemHeaders();

    using Ptr = ReferenceCountedObjectPtr<CompileEngineChildProcess>;

private:
    //==============================================================================
    class ChildProcess;
    std::unique_ptr<ChildProcess> process, runningAppProcess;
    ClassDatabase::ClassList lastComponentList;

    struct Editor;
    OwnedArray<Editor> editors;
    void updateAllEditors();

    void createProcess();
    Editor* getOrOpenEditorFor (const File&);
    ProjectContentComponent* findProjectContentComponent() const;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompileEngineChildProcess)
};

//==============================================================================
struct ChildProcessCache
{
    ChildProcessCache() {}

    CompileEngineChildProcess::Ptr getExisting (Project& project) const noexcept
    {
        for (auto& p : processes)
            if (&(p->project) == &project)
                return *p;

        return {};
    }

    CompileEngineChildProcess::Ptr getOrCreate (Project& project)
    {
        if (auto p = getExisting (project))
            return p;

        auto p = new CompileEngineChildProcess (project);
        tellNewProcessAboutExistingEditors (*p);
        processes.add (p);
        return *p;
    }

    static void tellNewProcessAboutExistingEditors (CompileEngineChildProcess& process)
    {
        auto& odm = ProjectorApplication::getApp().openDocumentManager;

        for (int i = odm.getNumOpenDocuments(); --i >= 0;)
            if (auto d = dynamic_cast<SourceCodeDocument*> (odm.getOpenDocument (i)))
                process.editorOpened (d->getFile(), d->getCodeDocument());
    }

    void removeOrphans()
    {
        processes.clear();
    }

private:
    ReferenceCountedArray<CompileEngineChildProcess> processes;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChildProcessCache)
};
