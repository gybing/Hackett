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

#include "../../Utility/UI/PropertyComponents/LabelPropertyComponent.h"

//==============================================================================
class GlobalPathsWindowComponent    : public Component,
                                      private Timer,
                                      private Value::Listener
{
public:
    GlobalPathsWindowComponent()
    {
        addChildComponent (rescanHPathButton);
        rescanHPathButton.onClick = [this]
        {
            ProjectorApplication::getApp().rescanHPathModules();
            lastHModulePath = getAppSettings().getStoredPath (Ids::defaultJuceModulePath, TargetOS::getThisOS()).get();
        };

        addChildComponent (rescanUserPathButton);
        rescanUserPathButton.onClick = [this]
        {
            ProjectorApplication::getApp().rescanUserPathModules();
            lastUserModulePath = getAppSettings().getStoredPath (Ids::defaultUserModulePath, TargetOS::getThisOS()).get();
        };

        addAndMakeVisible (resetToDefaultsButton);
        resetToDefaultsButton.onClick = [this] { resetCurrentOSPathsToDefaults(); };

        addAndMakeVisible (propertyViewport);
        propertyViewport.setViewedComponent (&propertyGroup, false);

        auto os = TargetOS::getThisOS();

        if      (os == TargetOS::osx)     selectedOSValue = "osx";
        else if (os == TargetOS::windows) selectedOSValue = "windows";
        else if (os == TargetOS::linux)   selectedOSValue = "linux";

        selectedOSValue.addListener (this);

        buildProps();

        lastHModulePath = getAppSettings().getStoredPath (Ids::defaultJuceModulePath, TargetOS::getThisOS()).get();
        lastUserModulePath = getAppSettings().getStoredPath (Ids::defaultUserModulePath, TargetOS::getThisOS()).get();
    }

    ~GlobalPathsWindowComponent() override
    {
        auto HValue = getAppSettings().getStoredPath (Ids::defaultJuceModulePath, TargetOS::getThisOS());
        auto userValue = getAppSettings().getStoredPath (Ids::defaultUserModulePath, TargetOS::getThisOS());

        auto HPathNeedsScanning = (! HValue.isUsingDefault() && HValue.get() != lastHModulePath);
        auto userPathNeedsScanning = (! userValue.isUsingDefault() && userValue.get() != lastUserModulePath);

        if (HPathNeedsScanning)
            ProjectorApplication::getApp().rescanHPathModules();

        if (userPathNeedsScanning)
            ProjectorApplication::getApp().rescanUserPathModules();
    }

    void paint (Graphics& g) override
    {
        g.fillAll (findColour (backgroundColourId));
    }

    void paintOverChildren (Graphics& g) override
    {
        g.setColour (findColour (defaultHighlightColourId).withAlpha (flashAlpha));
        g.fillRect (boundsToHighlight);
    }

    void resized() override
    {
        auto b = getLocalBounds().reduced (10);

        auto buttonBounds = b.removeFromBottom (50);

        rescanHPathButton.setBounds (buttonBounds.removeFromLeft (150).reduced (5, 10));
        rescanUserPathButton.setBounds (buttonBounds.removeFromLeft (150).reduced (5, 10));

        resetToDefaultsButton.setBounds (buttonBounds.removeFromRight (150).reduced (5, 10));

        propertyGroup.updateSize (0, 0, getWidth() - 20 - propertyViewport.getScrollBarThickness());
        propertyViewport.setBounds (b);
    }

    void highlightHPath()
    {
        if (isTimerRunning() || ! isSelectedOSThisOS())
            return;

        PropertyComponent* HPathPropertyComponent = nullptr;

        for (auto* prop : propertyGroup.properties)
            if (prop->getName() == "Path to H")
                HPathPropertyComponent = prop;

        if (HPathPropertyComponent != nullptr)
        {
            boundsToHighlight = getLocalArea (&propertyGroup, HPathPropertyComponent->getBounds());
            flashAlpha = 0.0f;
            hasFlashed = false;

            startTimer (25);
        }
    }

private:
    //==============================================================================
    void timerCallback() override
    {
        flashAlpha += (hasFlashed ? -0.05f : 0.05f);

        if (flashAlpha > 0.75f)
        {
            hasFlashed = true;
        }
        else if (flashAlpha < 0.0f)
        {
            flashAlpha = 0.0f;
            boundsToHighlight = {};

            stopTimer();
        }

        repaint();
    }

    void valueChanged (Value&) override
    {
        buildProps();
        resized();
    }

    //==============================================================================
    bool isSelectedOSThisOS()    { return TargetOS::getThisOS() == getSelectedOS(); }

    TargetOS::OS getSelectedOS() const
    {
        auto val = selectedOSValue.getValue();

        if      (val == "osx")      return TargetOS::osx;
        else if (val == "windows")  return TargetOS::windows;
        else if (val == "linux")    return TargetOS::linux;

        HAssertfalse;
        return TargetOS::unknown;
    }

    //==============================================================================
    void buildProps()
    {
        updateValues();

        PropertyListBuilder builder;
        auto isThisOS = isSelectedOSThisOS();

        builder.add (new ChoicePropertyComponent (selectedOSValue, "OS", { "OSX", "Windows", "Linux" }, { "osx", "windows", "linux" }),
                     "Use this dropdown to set the global paths for different OSes. "
                     "\nN.B. These paths are stored locally and will only be used when "
                     "saving a project on this machine. Other machines will have their own "
                     "locally stored paths.");

        builder.add (new LabelPropertyComponent ("H"), {});

        builder.add (new FilePathPropertyComponent (HPathValue, "Path to H", true, isThisOS),
                     "This should be the path to the top-level directory of your H folder. "
                     "This path will be used when searching for the H examples and DemoRunner application.");

        builder.add (new FilePathPropertyComponent (HModulePathValue, "H Modules", true, isThisOS),
                     String ("This should be the path to the folder containing the H modules that you wish to use, typically the \"modules\" directory of your H folder.")
                     + (isThisOS ? " Use the button below to re-scan a new path." : ""));
        builder.add (new FilePathPropertyComponent (userModulePathValue, "User Modules", true, isThisOS, {}, {}, true),
                     String ("A path to a folder containing any custom modules that you wish to use.")
                     + (isThisOS ? " Use the button below to re-scan new paths." : ""));

        builder.add (new LabelPropertyComponent ("SDKs"), {});

        builder.add (new FilePathPropertyComponent (vstPathValue,  "VST (Legacy) SDK", true, isThisOS),
                     "If you are building a legacy VST plug-in then this path should point to a VST2 SDK. "
                     "The VST2 SDK can be obtained from the vstsdk3610_11_06_2018_build_37 (or older) VST3 SDK or H version 5.3.2. "
                     "You also need a VST2 license from Steinberg to distribute VST2 plug-ins.");
        builder.add (new FilePathPropertyComponent (vst3PathValue, "VST3 SDK", true, isThisOS),
                     "This path can be set to use a custom VST3 SDK instead of the one which is embedded in H.");

        if (getSelectedOS() != TargetOS::linux)
        {
            builder.add (new FilePathPropertyComponent (aaxPathValue, "AAX SDK", true, isThisOS),
                         "If you are building AAX plug-ins, this should be the path to the AAX SDK folder.");
            builder.add (new FilePathPropertyComponent (rtasPathValue, "RTAS SDK (deprecated)", true, isThisOS),
                         "If you are building RTAS plug-ins, this should be the path to the RTAS SDK folder.");
        }

        builder.add (new FilePathPropertyComponent (androidSDKPathValue, "Android SDK", true, isThisOS),
                     "This path will be used when writing the local.properties file of an Android project and should point to the Android SDK folder.");
        builder.add (new FilePathPropertyComponent (androidNDKPathValue, "Android NDK", true, isThisOS),
                     "This path will be used when writing the local.properties file of an Android project and should point to the Android NDK folder.");

        if (isThisOS)
        {
            builder.add (new LabelPropertyComponent ("Other"), {});

           #if HMAC
            String exeLabel ("app");
           #elif HWINDOWS
            String exeLabel ("executable");
           #else
            String exeLabel ("startup script");
           #endif

            builder.add (new FilePathPropertyComponent (clionExePathValue, "CLion " + exeLabel,          false, isThisOS),
                         "This path will be used for the \"Save Project and Open in IDE...\" option of the CLion exporter.");
            builder.add (new FilePathPropertyComponent (androidStudioExePathValue, "Android Studio " + exeLabel, false, isThisOS),
                         "This path will be used for the \"Save Project and Open in IDE...\" option of the Android Studio exporter.");

            rescanHPathButton.setVisible (true);
            rescanUserPathButton.setVisible (true);
        }
        else
        {
            rescanHPathButton.setVisible (false);
            rescanUserPathButton.setVisible (false);
        }

        propertyGroup.setProperties (builder);
    }

    void updateValues()
    {
        auto& settings = getAppSettings();
        auto os = getSelectedOS();

        HPathValue             = settings.getStoredPath (Ids::HPath, os);
        HModulePathValue       = settings.getStoredPath (Ids::defaultJuceModulePath, os);
        userModulePathValue       = settings.getStoredPath (Ids::defaultUserModulePath, os);
        vstPathValue              = settings.getStoredPath (Ids::vstLegacyPath, os);
        vst3PathValue             = settings.getStoredPath (Ids::vst3Path, os);
        rtasPathValue             = settings.getStoredPath (Ids::rtasPath, os);
        aaxPathValue              = settings.getStoredPath (Ids::aaxPath, os);
        androidSDKPathValue       = settings.getStoredPath (Ids::androidSDKPath, os);
        androidNDKPathValue       = settings.getStoredPath (Ids::androidNDKPath, os);
        clionExePathValue         = settings.getStoredPath (Ids::clionExePath, os);
        androidStudioExePathValue = settings.getStoredPath (Ids::androidStudioExePath, os);
    }

    void resetCurrentOSPathsToDefaults()
    {
        HPathValue            .resetToDefault();
        HModulePathValue      .resetToDefault();
        userModulePathValue      .resetToDefault();
        vstPathValue             .resetToDefault();
        vst3PathValue            .resetToDefault();
        rtasPathValue            .resetToDefault();
        aaxPathValue             .resetToDefault();
        androidSDKPathValue      .resetToDefault();
        androidNDKPathValue      .resetToDefault();
        clionExePathValue        .resetToDefault();
        androidStudioExePathValue.resetToDefault();

        repaint();
    }

    //==============================================================================
    Value selectedOSValue;

    ValueWithDefault HPathValue, HModulePathValue, userModulePathValue,
                     vst3PathValue, vstPathValue, rtasPathValue, aaxPathValue,
                     androidSDKPathValue, androidNDKPathValue,
                     clionExePathValue, androidStudioExePathValue;

    Viewport propertyViewport;
    PropertyGroupComponent propertyGroup  { "Global Paths", { getIcons().openFolder, Colours::transparentBlack } };

    TextButton rescanHPathButton  { "Re-scan H Modules" },
               rescanUserPathButton  { "Re-scan User Modules" },
               resetToDefaultsButton { "Reset to Defaults" };

    Rectangle<int> boundsToHighlight;
    float flashAlpha = 0.0f;
    bool hasFlashed = false;

    var lastHModulePath, lastUserModulePath;

    //==============================================================================
    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlobalPathsWindowComponent)
};
