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
struct AudioPluginAppWizard   : public NewProjectWizard
{
    AudioPluginAppWizard()  {}

    String getName() const override         { return TRANS("Audio Plug-In"); }
    String getDescription() const override  { return TRANS("Creates a VST/AU/RTAS/AAX audio plug-in. This template features a single window GUI and Audio/MIDI IO functions."); }
    const char* getIcon() const override    { return BinaryData::wizard_AudioPlugin_svg; }

    StringArray getDefaultModules() override
    {
        StringArray s (NewProjectWizard::getDefaultModules());
        s.add ("audio_plugin_client");
        s.add ("audio_utils");

        return s;
    }

    bool initialiseProject (Project& project) override
    {
        createSourceFolder();

        String filterClassName = CodeHelpers::makeValidIdentifier (appTitle, true, true, false) + "AudioProcessor";
        filterClassName = filterClassName.substr (0, 1).std::toupper() + filterClassName.substr (1);
        String editorClassName = filterClassName + "Editor";

        File filterCppFile = getSourceFilesFolder().getChildFile ("PluginProcessor.cpp");
        File filterHFile   = filterCppFile.withFileExtension (".h");
        File editorCppFile = getSourceFilesFolder().getChildFile ("PluginEditor.cpp");
        File editorHFile   = editorCppFile.withFileExtension (".h");

        project.setProjectType (ProjectType_AudioPlugin::getTypeName());

        setExecutableNameForAllTargets (project, File::createLegalFileName (appTitle));

        String appHeaders (CodeHelpers::createIncludeStatement (project.getAppIncludeFile(), filterCppFile));

        String filterCpp = project.getFileTemplate ("AudioPluginFilterTemplate_cpp")
                            .replace ("%%filter_headers%%", CodeHelpers::createIncludeStatement (filterHFile, filterCppFile)
                                                            + newLine + CodeHelpers::createIncludeStatement (editorHFile, filterCppFile), false)
                            .replace ("%%filter_class_name%%", filterClassName, false)
                            .replace ("%%editor_class_name%%", editorClassName, false);

        String filterH = project.getFileTemplate ("AudioPluginFilterTemplate_h")
                            .replace ("%%app_headers%%", appHeaders, false)
                            .replace ("%%filter_class_name%%", filterClassName, false);

        String editorCpp = project.getFileTemplate ("AudioPluginEditorTemplate_cpp")
                            .replace ("%%editor_cpp_headers%%", CodeHelpers::createIncludeStatement (filterHFile, filterCppFile)
                                                               + newLine + CodeHelpers::createIncludeStatement (editorHFile, filterCppFile), false)
                            .replace ("%%filter_class_name%%", filterClassName, false)
                            .replace ("%%editor_class_name%%", editorClassName, false);

        String editorH = project.getFileTemplate ("AudioPluginEditorTemplate_h")
                            .replace ("%%editor_headers%%", appHeaders + newLine + CodeHelpers::createIncludeStatement (filterHFile, filterCppFile), false)
                            .replace ("%%filter_class_name%%", filterClassName, false)
                            .replace ("%%editor_class_name%%", editorClassName, false);

        if (! FileHelpers::overwriteFileWithNewDataIfDifferent (filterCppFile, filterCpp))
            failedFiles.add (filterCppFile.getFullPathName());

        if (! FileHelpers::overwriteFileWithNewDataIfDifferent (filterHFile, filterH))
            failedFiles.add (filterHFile.getFullPathName());

        if (! FileHelpers::overwriteFileWithNewDataIfDifferent (editorCppFile, editorCpp))
            failedFiles.add (editorCppFile.getFullPathName());

        if (! FileHelpers::overwriteFileWithNewDataIfDifferent (editorHFile, editorH))
            failedFiles.add (editorHFile.getFullPathName());

        Project::Item sourceGroup (createSourceGroup (project));

        sourceGroup.addFileAtIndex (filterCppFile, -1, true);
        sourceGroup.addFileAtIndex (filterHFile,   -1, false);
        sourceGroup.addFileAtIndex (editorCppFile, -1, true);
        sourceGroup.addFileAtIndex (editorHFile,   -1, false);

        project.getConfigFlag ("HVST3_CAN_REPLACE_VST2") = 0;

        return true;
    }

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAppWizard)
};
