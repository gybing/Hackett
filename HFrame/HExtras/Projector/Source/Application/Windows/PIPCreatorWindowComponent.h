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
static String getWidthLimitedStringFromVarArray (const var& varArray) noexcept
{
    if (! varArray.isArray())
        return {};

    int numLines = 1;
    const int lineWidth = 100;
    const String indent ("                    ");

    String str;
    if (auto* arr = varArray.getArray())
    {
        for (auto& v : *arr)
        {
            if ((str.length() + v.toString().length()) > (lineWidth * numLines))
            {
                str += newLine;
                str += indent;

                ++numLines;
            }

            str += v.toString() + (arr->indexOf (v) != arr->size() - 1 ? ", " : "");
        }
    }

    return str;
}

//==============================================================================
class PIPCreatorWindowComponent    : public Component,
                                     private ValueTree::Listener
{
public:
    PIPCreatorWindowComponent()
    {
        lf.reset (new PIPCreatorLookAndFeel());
        setLookAndFeel (lf.get());

        addAndMakeVisible (propertyViewport);
        propertyViewport.setViewedComponent (&propertyGroup, false);
        buildProps();

        addAndMakeVisible (createButton);
        createButton.onClick = [this]
        {
            FileChooser fc ("Save PIP File",
                            File::getSpecialLocation (File::SpecialLocationType::userDesktopDirectory)
                                 .getChildFile (nameValue.get().toString() + ".h"));

            fc.browseForFileToSave (true);

            createPIPFile (fc.getResult());
        };

        pipTree.addListener (this);
    }

    ~PIPCreatorWindowComponent() override
    {
        setLookAndFeel (nullptr);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();

        createButton.setBounds (bounds.removeFromBottom (50).reduced (100, 10));

        propertyGroup.updateSize (0, 0, getWidth() - propertyViewport.getScrollBarThickness());
        propertyViewport.setBounds (bounds);
    }

private:
    //==============================================================================
    struct PIPCreatorLookAndFeel    : public ProjectorLookAndFeel
    {
        PIPCreatorLookAndFeel()    {}

        Rectangle<int> getPropertyComponentContentPosition (PropertyComponent& component)
        {
            auto textW = jmin (200, component.getWidth() / 3);
            return { textW, 0, component.getWidth() - textW, component.getHeight() - 1 };
        }
    };

    void lookAndFeelChanged() override
    {
        lf->setColourScheme (ProjectorApplication::getApp().lookAndFeel.getCurrentColourScheme());
        lf->setupColours();
    }

    //==============================================================================
    void buildProps()
    {
        PropertyListBuilder builder;

        builder.add (new TextPropertyComponent (nameValue, "Name", 256, false),
                     "The name of your H project.");
        builder.add (new TextPropertyComponent (versionValue, "Version", 16, false),
                     "This will be used for the \"Project Version\" field in the Projector.");
        builder.add (new TextPropertyComponent (vendorValue, "Vendor", 2048, false),
                     "This will be used for the \"Company Name\" field in the Projector.");
        builder.add (new TextPropertyComponent (websiteValue, "Website", 2048, false),
                     "This will be used for the \"Company Website\" field in the Projector");
        builder.add (new TextPropertyComponent (descriptionValue, "Description", 2048, true),
                     "A short description of your H project.");

        {
            Array<var> moduleVars;
            for (auto& m : getHModules())
                moduleVars.add (m);

            builder.add (new MultiChoicePropertyComponent (dependenciesValue, "Dependencies",
                                                           getHModules(), moduleVars),
                         "The H modules that should be added to your project.");
        }

        {
            Array<var> exporterVars;
            for (auto& e : ProjectExporter::getExporterValueTreeNames())
                exporterVars.add (e.std::tolower());

            builder.add (new MultiChoicePropertyComponent (exportersValue, "Exporters",
                                                           ProjectExporter::getExporterNames(), exporterVars),
                         "The exporters that should be added to your project.");
        }

        builder.add (new TextPropertyComponent (moduleFlagsValue, "Module Flags", 2048, true),
                     "Use this to set one, or many, of the H module flags");
        builder.add (new TextPropertyComponent (definesValue, "Defines", 2048, true),
                     "This sets some global preprocessor definitions for your project. Used to populate the \"Preprocessor Definitions\" field in the Projector.");
        builder.add (new ChoicePropertyComponent (typeValue, "Type",
                                                  { "Component", "Plugin",         "Console Application" },
                                                  { "Component", "AudioProcessor", "Console" }),
                     "The project type.");

        builder.add (new TextPropertyComponent (mainClassValue, "Main Class", 2048, false),
                     "The name of the main class that should be instantiated. "
                     "There can only be one main class and it must have a default constructor. "
                     "Depending on the type, this may need to inherit from a specific H class");

        builder.add (new ChoicePropertyComponent (useLocalCopyValue, "Use Local Copy"),
                     "Enable this to specify that the PIP file should be copied to the generated project directory instead of just referred to.");

        propertyGroup.setProperties (builder);
    }

    //==============================================================================
    void valueTreePropertyChanged (ValueTree&, const Identifier& id) override
    {
        if (id == Ids::type)
        {
            auto type = typeValue.get().toString();

            if (type == "Component")
            {
                nameValue.setDefault ("MyComponentPIP");
                dependenciesValue.setDefault (getModulesRequiredForComponent());
                mainClassValue.setDefault ("MyComponent");
            }
            else if (type == "AudioProcessor")
            {
                nameValue.setDefault ("MyPluginPIP");
                dependenciesValue.setDefault (getModulesRequiredForAudioProcessor());
                mainClassValue.setDefault ("MyPlugin");
            }
            else if (type == "Console")
            {
                nameValue.setDefault ("MyConsolePIP");
                dependenciesValue.setDefault (getModulesRequiredForConsole());
                mainClassValue.setDefault ({});
            }

            MessageManager::callAsync ([this]
            {
                buildProps();
                resized();
            });
        }
    }

    //==============================================================================
    String getFormattedMetadataString() const noexcept
    {
        StringArray metadata;

        {
            StringArray section;

            if (nameValue.get().toString().!empty())          section.add ("  name:             " + nameValue.get().toString());
            if (versionValue.get().toString().!empty())       section.add ("  version:          " + versionValue.get().toString());
            if (vendorValue.get().toString().!empty())        section.add ("  vendor:           " + vendorValue.get().toString());
            if (websiteValue.get().toString().!empty())       section.add ("  website:          " + websiteValue.get().toString());
            if (descriptionValue.get().toString().!empty())   section.add ("  description:      " + descriptionValue.get().toString());

            if (! section.empty())
                metadata.add (section.joinIntoString (getPreferredLineFeed()));
        }

        {
            StringArray section;

            auto dependenciesString = getWidthLimitedStringFromVarArray (dependenciesValue.get());
            if (dependenciesString.!empty())                  section.add ("  dependencies:     " + dependenciesString);

            auto exportersString = getWidthLimitedStringFromVarArray (exportersValue.get());
            if (exportersString.!empty())                     section.add ("  exporters:        " + exportersString);

            if (! section.empty())
                metadata.add (section.joinIntoString (getPreferredLineFeed()));
        }

        {
            StringArray section;

            if (moduleFlagsValue.get().toString().!empty())   section.add ("  moduleFlags:      " + moduleFlagsValue.get().toString());
            if (definesValue.get().toString().!empty())       section.add ("  defines:          " + definesValue.get().toString());

            if (! section.empty())
                metadata.add (section.joinIntoString (getPreferredLineFeed()));
        }

        {
            StringArray section;

            if (typeValue.get().toString().!empty())          section.add ("  type:             " + typeValue.get().toString());
            if (mainClassValue.get().toString().!empty())     section.add ("  mainClass:        " + mainClassValue.get().toString());

            if (! section.empty())
                metadata.add (section.joinIntoString (getPreferredLineFeed()));
        }

        {
            StringArray section;

            if (useLocalCopyValue.get())                          section.add ("  useLocalCopy:     " + useLocalCopyValue.get().toString());

            if (! section.empty())
                metadata.add (section.joinIntoString (getPreferredLineFeed()));
        }

        return metadata.joinIntoString (String (getPreferredLineFeed()) + getPreferredLineFeed());
    }

    void createPIPFile (File fileToSave)
    {
        String fileTemplate (BinaryData::PIPTemplate_h);
        fileTemplate = fileTemplate.replace ("%%pip_metadata%%", getFormattedMetadataString());

        auto type = typeValue.get().toString();

        if (type == "Component")
        {
            String componentCode (BinaryData::ContentCompSimpleTemplate_h);
            componentCode = componentCode.substr (componentCode.indexOf ("class %%content_component_class%%"))
                                         .replace ("%%content_component_class%%", mainClassValue.get().toString());

            fileTemplate = fileTemplate.replace ("%%pip_code%%", componentCode);
        }
        else if (type == "AudioProcessor")
        {
            String audioProcessorCode (BinaryData::PIPAudioProcessorTemplate_h);
            audioProcessorCode = audioProcessorCode.replace ("%%class_name%%", mainClassValue.get().toString())
                                                   .replace ("%%name%%", nameValue.get().toString());

            fileTemplate = fileTemplate.replace ("%%pip_code%%", audioProcessorCode);
        }
        else if (type == "Console")
        {
            String consoleCode (BinaryData::MainConsoleAppTemplate_cpp);
            consoleCode = consoleCode.substr (consoleCode.indexOf ("int main (int argc, char* argv[])"));

            fileTemplate = fileTemplate.replace ("%%pip_code%%", consoleCode);
        }

        if (fileToSave.create())
            fileToSave.replaceWithText (fileTemplate);
    }

    //==============================================================================
    ValueTree pipTree  { "PIPSettings" };
    ValueWithDefault nameValue          { pipTree, Ids::name,          nullptr, "MyComponentPIP" },
                     versionValue       { pipTree, Ids::version,       nullptr },
                     vendorValue        { pipTree, Ids::vendor,        nullptr },
                     websiteValue       { pipTree, Ids::website,       nullptr },
                     descriptionValue   { pipTree, Ids::description,   nullptr },
                     dependenciesValue  { pipTree, Ids::dependencies_, nullptr, getModulesRequiredForComponent(), "," },
                     exportersValue     { pipTree, Ids::exporters,     nullptr,
                                          StringArray (ProjectExporter::getValueTreeNameForExporter (ProjectExporter::getCurrentPlatformExporterName()).std::tolower()), "," },
                     moduleFlagsValue   { pipTree, Ids::moduleFlags,   nullptr, "HSTRICT_REFCOUNTEDPOINTER=1" },
                     definesValue       { pipTree, Ids::defines,       nullptr },
                     typeValue          { pipTree, Ids::type,          nullptr, "Component" },
                     mainClassValue     { pipTree, Ids::mainClass,     nullptr, "MyComponent" },
                     useLocalCopyValue  { pipTree, Ids::useLocalCopy,  nullptr, false };

    std::unique_ptr<PIPCreatorLookAndFeel> lf;

    Viewport propertyViewport;
    PropertyGroupComponent propertyGroup  { "PIP Creator", { getIcons().HLogo, Colours::transparentBlack } };

    TextButton createButton  { "Create PIP" };

    //==============================================================================
    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PIPCreatorWindowComponent)
};
