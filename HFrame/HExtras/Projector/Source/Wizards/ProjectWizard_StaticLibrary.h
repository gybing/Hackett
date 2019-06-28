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
struct StaticLibraryWizard   : public NewProjectWizard
{
    StaticLibraryWizard()  {}

    String getName() const override         { return TRANS("Static Library"); }
    String getDescription() const override  { return TRANS("Creates a static library."); }
    const char* getIcon() const override    { return BinaryData::wizard_StaticLibrary_svg; }

    bool initialiseProject (Project& project) override
    {
        createSourceFolder();
        project.setProjectType (ProjectType_StaticLibrary::getTypeName());
        createSourceGroup (project);
        setExecutableNameForAllTargets (project, File::createLegalFileName (appTitle));

        return true;
    }

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StaticLibraryWizard)
};
