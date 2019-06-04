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
struct ConsoleAppWizard   : public NewProjectWizard
{
    ConsoleAppWizard()  {}

    String getName() const override         { return TRANS("Console Application"); }
    String getDescription() const override  { return TRANS("Creates a command-line application without GUI support."); }
    const char* getIcon() const override    { return BinaryData::wizard_ConsoleApp_svg; }

    StringArray getFileCreationOptions() override
    {
        return { "Create a Main.cpp file",
                 "Don't create any files" };
    }

    Result processResultsFromSetupItems (WizardComp& setupComp) override
    {
        createMainCpp = false;

        switch (setupComp.getFileCreationComboID())
        {
            case 0:     createMainCpp = true;  break;
            case 1:     break;
            default:    HAssertfalse; break;
        }

        return Result::ok();
    }

    bool initialiseProject (Project& project) override
    {
        createSourceFolder();

        project.setProjectType (ProjectType_ConsoleApp::getTypeName());

        Project::Item sourceGroup (createSourceGroup (project));

        setExecutableNameForAllTargets (project, File::createLegalFileName (appTitle));

        if (createMainCpp)
        {
            File mainCppFile = getSourceFilesFolder().getChildFile ("Main.cpp");
            String appHeaders (CodeHelpers::createIncludeStatement (project.getAppIncludeFile(), mainCppFile));

            String mainCpp = project.getFileTemplate ("MainConsoleAppTemplate_cpp")
                                .replace ("%%app_headers%%", appHeaders, false);

            if (! FileHelpers::overwriteFileWithNewDataIfDifferent (mainCppFile, mainCpp))
                failedFiles.add (mainCppFile.getFullPathName());

            sourceGroup.addFileAtIndex (mainCppFile, -1, true);
        }

        return true;
    }

private:
    bool createMainCpp;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConsoleAppWizard)
};
