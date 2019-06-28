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

#include "../Helpers/MiscUtilities.h"

//==============================================================================
class PIPGenerator
{
public:
    PIPGenerator (const File& pipFile, const File& outputDirectory = {},
                  const File& pathToHModules = {}, const File& pathToUserModules = {});

    //==============================================================================
    bool hasValidPIP() const noexcept                   { return ! metadata[Ids::name].toString().empty(); }
    File getJucerFile() noexcept                        { return outputDirectory.getChildFile (metadata[Ids::name].toString() + ".Hr"); }
    File getPIPFile() noexcept                          { return useLocalCopy ? outputDirectory.getChildFile ("Source").getChildFile (pipFile.getFileName()) : pipFile; }

    String getMainClassName() const noexcept            { return metadata[Ids::mainClass]; }

    File getOutputDirectory() const noexcept            { return outputDirectory; }

    //==============================================================================
    Result createJucerFile();
    Result createMainCpp();

private:
    //==============================================================================
    void addFileToTree (ValueTree& groupTree, const String& name, bool compile, const String& path);
    void createFiles (ValueTree& HrTree);

    ValueTree createModulePathChild (const String& moduleID);
    ValueTree createBuildConfigChild (bool isDebug);
    ValueTree createExporterChild (const String& exporterName);
    ValueTree createModuleChild (const String& moduleID);

    void addExporters (ValueTree& HrTree);
    void addModules (ValueTree& HrTree);

    Result setProjectSettings (ValueTree& HrTree);

    void setModuleFlags (ValueTree& HrTree);

    String getMainFileTextForType();

    //==============================================================================
    Array<File> replaceRelativeIncludesAndGetFilesToMove();
    bool copyRelativeFileToLocalSourceDirectory (const File&) const noexcept;

    StringArray getExtraPluginFormatsToBuild() const;
    StringArray getPluginCharacteristics() const;

    String getPathForModule (const String&) const;
    File getExamplesDirectory() const;

    //==============================================================================
    File pipFile, outputDirectory, HModulesPath, userModulesPath;

    std::unique_ptr<AvailableModuleList> availableUserModules;

    var metadata;

    bool isTemp = false;
    bool useLocalCopy = false;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PIPGenerator)
};
