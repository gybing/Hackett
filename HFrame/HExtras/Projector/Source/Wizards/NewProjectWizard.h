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


//==============================================================================
static void setExecutableNameForAllTargets (Project& project, const String& exeName)
{
    for (Project::ExporterIterator exporter (project); exporter.next();)
        for (ProjectExporter::ConfigIterator config (*exporter); config.next();)
            config->getValue (Ids::targetName) = exeName;
}

static Project::Item createSourceGroup (Project& project)
{
    return project.getMainGroup().addNewSubGroup ("Source", 0);
}

static File& getLastWizardFolder()
{
    if (getAppSettings().lastWizardFolder.isDirectory())
        return getAppSettings().lastWizardFolder;

   #if HWINDOWS
    static File lastFolderFallback (File::getSpecialLocation (File::userDocumentsDirectory));
   #else
    static File lastFolderFallback (File::getSpecialLocation (File::userHomeDirectory));
   #endif

    return lastFolderFallback;
}

//==============================================================================
struct NewProjectWizard
{
    NewProjectWizard() {}
    virtual ~NewProjectWizard() {}

    //==============================================================================
    virtual String getName() const = 0;
    virtual String getDescription() const = 0;
    virtual const char* getIcon() const = 0;

    virtual StringArray getFileCreationOptions()                        { return {}; }
    virtual Result processResultsFromSetupItems (WizardComp&)           { return Result::ok(); }

    virtual bool initialiseProject (Project& project) = 0;

    virtual StringArray getDefaultModules()
    {
        return
        {
            "core",
            "events",
            "graphics",
            "data_structures",
            "gui_basics",
            "gui_extra",
            "cryptography",
            "opengl",
            "audio_basics",
            "audio_devices",
            "audio_formats",
            "audio_processors"
        };
    }

    String appTitle;
    File targetFolder, projectFile, modulesFolder;
    WizardComp* ownerWizardComp;
    StringArray failedFiles;

    bool selectJuceFolder()
    {
        return ModulesFolderPathBox::selectJuceFolder (modulesFolder);
    }

    //==============================================================================
    Project* runWizard (WizardComp& wc,
                        const String& projectName,
                        const File& target,
                        bool useGlobalPath)
    {
        ownerWizardComp = &wc;
        appTitle = projectName;
        targetFolder = target;

        if (! targetFolder.exists())
        {
            if (! targetFolder.createDirectory())
                failedFiles.add (targetFolder.getFullPathName());
        }
        else if (FileHelpers::containsAnyNonHiddenFiles (targetFolder))
        {
            if (! AlertWindow::showOkCancelBox (AlertWindow::InfoIcon,
                                                TRANS("New H Project"),
                                                TRANS("You chose the folder:\n\nXFLDRX\n\n").replace ("XFLDRX", targetFolder.getFullPathName())
                                                  + TRANS("This folder isn't empty - are you sure you want to create the project there?")
                                                  + "\n\n"
                                                  + TRANS("Any existing files with the same names may be overwritten by the new files.")))
                return nullptr;
        }

        projectFile = targetFolder.getChildFile (File::createLegalFileName (appTitle))
                                  .withFileExtension (Project::projectFileExtension);

        std::unique_ptr<Project> project (new Project (projectFile));

        if (failedFiles.size() == 0)
        {
            project->setFile (projectFile);
            project->setTitle (appTitle);

            if (! initialiseProject (*project))
                return nullptr;

            project->getConfigFlag ("HSTRICT_REFCOUNTEDPOINTER") = true;

            addExporters (*project, wc);
            addDefaultModules (*project, useGlobalPath);

            if (project->save (false, true) != FileBasedDocument::savedOk)
                return nullptr;

            project->setChangedFlag (false);
        }

        if (failedFiles.size() > 0)
        {
            AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                              TRANS("Errors in Creating Project!"),
                                              TRANS("The following files couldn't be written:")
                                                + "\n\n"
                                                + failedFiles.joinIntoString ("\n", 0, 10));
            return nullptr;
        }

        StringPairArray data;
        data.set ("label", "Project Type = " + project->getProjectTypeString());

        Analytics::getInstance()->logEvent ("Project Setting", data, ProjectorAnalyticsEvent::projectEvent);

        return project.release();
    }

    //==============================================================================
    File getSourceFilesFolder() const
    {
        return projectFile.getSiblingFile ("Source");
    }

    void createSourceFolder()
    {
        if (! getSourceFilesFolder().createDirectory())
            failedFiles.add (getSourceFilesFolder().getFullPathName());
    }

    void addDefaultModules (Project& project, bool useGlobalPath)
    {
        auto defaultModules = getDefaultModules();

        AvailableModuleList list;
        list.scanPaths ({ modulesFolder });

        for (auto& mod : list.getAllModules())
            if (defaultModules.contains (mod.first))
                project.getEnabledModules().addModule (mod.second, false, useGlobalPath, false);
    }

    void addExporters (Project& project, WizardComp& wizardComp)
    {
        StringArray types (wizardComp.platformTargets.getSelectedPlatforms());

        for (int i = 0; i < types.size(); ++i)
            project.addNewExporter (types[i]);

        if (project.getNumExporters() == 0)
            project.createExporterForCurrentPlatform();
    }
};
