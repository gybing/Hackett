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

#include "../Application/Headers.h"
#include "NewProjectWizardClasses.h"
#include "../ProjectSaving/ProjectExporter.h"
#include "../Utility/UI/SlidingPanelComponent.h"

struct NewProjectWizardClasses
{
    class WizardComp;
    #include "NewProjectWizard.h"

    #include "ProjectWizard_GUIApp.h"
    #include "ProjectWizard_Console.h"
    #include "ProjectWizard_AudioPlugin.h"
    #include "ProjectWizard_StaticLibrary.h"
    #include "ProjectWizard_DLL.h"
    #include "ProjectWizard_openGL.h"
    #include "ProjectWizard_Animated.h"
    #include "ProjectWizard_AudioApp.h"
    #include "ProjectWizard_Blank.h"

    #include "NewProjectWizardComponent.h"
    #include "TemplateThumbnailsComponent.h"
    #include "StartPageComponent.h"

    //==============================================================================
    static int getNumWizards() noexcept
    {
        return 9;
    }

    static std::unique_ptr<NewProjectWizard> createWizardType (int index)
    {
        switch (index)
        {
            case 0:     return std::unique_ptr<NewProjectWizard> (new NewProjectWizardClasses::GUIAppWizard());
            case 1:     return std::unique_ptr<NewProjectWizard> (new NewProjectWizardClasses::AnimatedAppWizard());
            case 2:     return std::unique_ptr<NewProjectWizard> (new NewProjectWizardClasses::OpenGLAppWizard());
            case 3:     return std::unique_ptr<NewProjectWizard> (new NewProjectWizardClasses::ConsoleAppWizard());
            case 4:     return std::unique_ptr<NewProjectWizard> (new NewProjectWizardClasses::AudioAppWizard());
            case 5:     return std::unique_ptr<NewProjectWizard> (new NewProjectWizardClasses::AudioPluginAppWizard());
            case 6:     return std::unique_ptr<NewProjectWizard> (new NewProjectWizardClasses::StaticLibraryWizard());
            case 7:     return std::unique_ptr<NewProjectWizard> (new NewProjectWizardClasses::DynamicLibraryWizard());
            case 8:     return std::unique_ptr<NewProjectWizard> (new NewProjectWizardClasses::BlankAppWizard());
            default:    HAssertfalse; break;
        }

        return {};
    }

    static StringArray getWizardNames()
    {
        StringArray s;

        for (int i = 0; i < getNumWizards(); ++i)
            s.add (createWizardType (i)->getName());

        return s;
    }
};

Component* createNewProjectWizardComponent()
{
    return new NewProjectWizardClasses::StartPageComponent();
}
