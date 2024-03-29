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
struct OpenGLAppWizard   : public NewProjectWizard
{
    OpenGLAppWizard()  {}

    String getName() const override         { return TRANS("OpenGL Application"); }
    String getDescription() const override  { return TRANS("Creates a blank H application with a single window component. This component supports openGL drawing features including 3D model import and GLSL shaders."); }
    const char* getIcon() const override    { return BinaryData::wizard_OpenGL_svg; }

    StringArray getFileCreationOptions() override
    {
        return { "Create header and implementation files",
                 "Create header file only" };
    }

    Result processResultsFromSetupItems (WizardComp& setupComp) override
    {
        createCppFile = false;

        switch (setupComp.getFileCreationComboID())
        {
            case 0:     createCppFile = true;  break;
            case 1:     break;
            default:    HAssertfalse; break;
        }

        return Result::ok();
    }

    bool initialiseProject (Project& project) override
    {
        createSourceFolder();

        File mainCppFile    = getSourceFilesFolder().getChildFile ("Main.cpp");
        File contentCompCpp = getSourceFilesFolder().getChildFile ("MainComponent.cpp");
        File contentCompH   = contentCompCpp.withFileExtension (".h");
        String contentCompName = "MainComponent";

        project.setProjectType (ProjectType_GUIApp::getTypeName());

        Project::Item sourceGroup (createSourceGroup (project));

        setExecutableNameForAllTargets (project, File::createLegalFileName (appTitle));

        String appHeaders (CodeHelpers::createIncludeStatement (project.getAppIncludeFile(), mainCppFile));

        // create main window
        appHeaders << newLine << CodeHelpers::createIncludeStatement (contentCompH, mainCppFile);

        String windowH = project.getFileTemplate (createCppFile ? "OpenGLComponentTemplate_h"
                                                                : "OpenGLComponentSimpleTemplate_h")
                            .replace ("%%include_H%%", CodeHelpers::createIncludeStatement (project.getAppIncludeFile(), contentCompH), false)
                            .replace ("%%content_component_class%%", contentCompName, false);

        if (! FileHelpers::overwriteFileWithNewDataIfDifferent (contentCompH, windowH))
            failedFiles.add (contentCompH.getFullPathName());

        sourceGroup.addFileAtIndex (contentCompH, -1, false);

        if (createCppFile)
        {
            String windowCpp = project.getFileTemplate ("OpenGLComponentTemplate_cpp")
                                  .replace ("%%include_H%%", CodeHelpers::createIncludeStatement (project.getAppIncludeFile(), contentCompCpp), false)
                                  .replace ("%%include_corresponding_header%%", CodeHelpers::createIncludeStatement (contentCompH, contentCompCpp), false)
                                  .replace ("%%content_component_class%%", contentCompName, false);

            if (! FileHelpers::overwriteFileWithNewDataIfDifferent (contentCompCpp, windowCpp))
                failedFiles.add (contentCompCpp.getFullPathName());

            sourceGroup.addFileAtIndex (contentCompCpp, -1, true);
        }

        // create main cpp
        String mainCpp = project.getFileTemplate ("MainTemplate_SimpleWindow_cpp")
                            .replace ("%%app_headers%%", appHeaders, false)
                            .replace ("%%app_class_name%%", CodeHelpers::makeValidIdentifier (appTitle + "Application", false, true, false), false)
                            .replace ("%%content_component_class%%", contentCompName, false)
                            .replace ("%%allow_more_than_one_instance%%", "true", false);

        if (! FileHelpers::overwriteFileWithNewDataIfDifferent (mainCppFile, mainCpp))
            failedFiles.add (mainCppFile.getFullPathName());

        sourceGroup.addFileAtIndex (mainCppFile, -1, true);

        return true;
    }

private:
    bool createCppFile;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLAppWizard)
};
