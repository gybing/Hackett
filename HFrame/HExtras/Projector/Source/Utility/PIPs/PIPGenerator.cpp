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

#include "../../Application/Headers.h"
#include "../../ProjectSaving/ProjectExporter.h"
#include "PIPGenerator.h"
#include "../../Project/Module.h"

//==============================================================================
static String removeEnclosed (const String& input, const String& start, const String& end)
{
    auto startIndex = input.indexOf (start);
    auto endIndex   = input.indexOf (end) + end.length();

    if (startIndex != -1 && endIndex != -1)
        return input.replaceSection (startIndex, endIndex - startIndex, {});

    return input;
}

static void ensureSingleNewLineAfterIncludes (StringArray& lines)
{
    int lastIncludeIndex = -1;

    for (int i = 0; i < lines.size(); ++i)
    {
        if (lines[i].contains ("#include"))
            lastIncludeIndex = i;
    }

    if (lastIncludeIndex != -1)
    {
        auto index = lastIncludeIndex;
        int numNewLines = 0;

        while (++index < lines.size() && lines[index].empty())
            ++numNewLines;

        if (numNewLines > 1)
            lines.removeRange (lastIncludeIndex + 1, numNewLines - 1);
    }
}

static String ensureCorrectWhitespace (const String& input)
{
    auto lines = StringArray::fromLines (input);
    ensureSingleNewLineAfterIncludes (lines);
    return joinLinesIntoSourceFile (lines);
}

static bool isHExample (const File& pipFile)
{
    int numLinesToTest = 10; // license should be at the top of the file so no need to
                             // check all lines

    for (auto line : StringArray::fromLines (pipFile.loadFileAsString()))
    {
        if (line.contains ("This file is part of the H examples."))
            return true;

        --numLinesToTest;
    }

    return false;
}

static bool isValidExporterName (const String& exporterName)
{
    return ProjectExporter::getExporterValueTreeNames().contains (exporterName, true);
}

static bool exporterRequiresExampleAssets (const String& exporterName, const String& projectName)
{
    return (exporterName == "XCODE_IPHONE" || exporterName == "ANDROIDSTUDIO")
            || (exporterName == "XCODE_MAC" && projectName == "AUv3SynthPlugin");
}

//==============================================================================
PIPGenerator::PIPGenerator (const File& pip, const File& output, const File& HPath, const File& userPath)
    : pipFile (pip),
      HModulesPath (HPath),
      userModulesPath (userPath),
      metadata (parseHHeaderMetadata (pipFile))
{
    if (output != File())
    {
        outputDirectory = output;
        isTemp = false;
    }
    else
    {
        outputDirectory = File::getSpecialLocation (File::SpecialLocationType::tempDirectory).getChildFile ("PIPs");
        isTemp = true;
    }

    auto isClipboard = (pip.getParentDirectory().getFileName() == "Clipboard"
                        && pip.getParentDirectory().getParentDirectory().getFileName() == "PIPs");

    outputDirectory = outputDirectory.getChildFile (metadata[Ids::name].toString());
    useLocalCopy = metadata[Ids::useLocalCopy].toString().trim().getIntValue() == 1 || isClipboard;

    if (userModulesPath != File())
    {
        availableUserModules.reset (new AvailableModuleList());
        availableUserModules->scanPaths ({ userModulesPath });
    }
}

//==============================================================================
Result PIPGenerator::createJucerFile()
{
    ValueTree root (Ids::HRPROJECT);

    auto result = setProjectSettings (root);

    if (result != Result::ok())
        return result;

    addModules     (root);
    addExporters   (root);
    createFiles    (root);
    setModuleFlags (root);

    auto outputFile = outputDirectory.getChildFile (metadata[Ids::name].toString() + ".Hr");

    if (auto xml = root.createXml())
        if (xml->writeTo (outputFile, {}))
            return Result::ok();

    return Result::fail ("Failed to create .Hr file in " + outputDirectory.getFullPathName());
}

Result PIPGenerator::createMainCpp()
{
    auto outputFile = outputDirectory.getChildFile ("Source").getChildFile ("Main.cpp");

    if (! outputFile.existsAsFile() && (outputFile.create() != Result::ok()))
        return Result::fail ("Failed to create Main.cpp - " + outputFile.getFullPathName());

    outputFile.replaceWithText (getMainFileTextForType());

    return Result::ok();
}

//==============================================================================
void PIPGenerator::addFileToTree (ValueTree& groupTree, const String& name, bool compile, const String& path)
{
    ValueTree file (Ids::FILE);
    file.setProperty (Ids::ID, createAlphaNumericUID(), nullptr);
    file.setProperty (Ids::name, name, nullptr);
    file.setProperty (Ids::compile, compile, nullptr);
    file.setProperty (Ids::resource, 0, nullptr);
    file.setProperty (Ids::file, path, nullptr);

    groupTree.addChild (file, -1, nullptr);
}

void PIPGenerator::createFiles (ValueTree& HrTree)
{
    auto sourceDir = outputDirectory.getChildFile ("Source");

    if (! sourceDir.exists())
        sourceDir.createDirectory();

    if (useLocalCopy)
        pipFile.copyFileTo (sourceDir.getChildFile (pipFile.getFileName()));

    ValueTree mainGroup (Ids::MAINGROUP);
    mainGroup.setProperty (Ids::ID, createAlphaNumericUID(), nullptr);
    mainGroup.setProperty (Ids::name, metadata[Ids::name], nullptr);

    ValueTree group (Ids::GROUP);
    group.setProperty (Ids::ID, createGUID (sourceDir.getFullPathName() + "_guidpathsaltxhsdf"), nullptr);
    group.setProperty (Ids::name, "Source", nullptr);

    addFileToTree (group, "Main.cpp", true, "Source/Main.cpp");
    addFileToTree (group, pipFile.getFileName(), false, useLocalCopy ? "Source/" + pipFile.getFileName()
                                                                     : pipFile.getFullPathName());

    mainGroup.addChild (group, -1, nullptr);

    if (useLocalCopy)
    {
        auto relativeFiles = replaceRelativeIncludesAndGetFilesToMove();

        if (relativeFiles.size() > 0)
        {
            ValueTree assets (Ids::GROUP);
            assets.setProperty (Ids::ID, createAlphaNumericUID(), nullptr);
            assets.setProperty (Ids::name, "Assets", nullptr);

            for (auto& f : relativeFiles)
                if (copyRelativeFileToLocalSourceDirectory (f))
                    addFileToTree (assets, f.getFileName(), f.getFileExtension() == ".cpp", "Source/" + f.getFileName());

            mainGroup.addChild (assets, -1, nullptr);
        }
    }

    HrTree.addChild (mainGroup, 0, nullptr);
}

ValueTree PIPGenerator::createModulePathChild (const String& moduleID)
{
    ValueTree modulePath (Ids::MODULEPATH);

    modulePath.setProperty (Ids::ID, moduleID, nullptr);
    modulePath.setProperty (Ids::path, getPathForModule (moduleID), nullptr);

    return modulePath;
}

ValueTree PIPGenerator::createBuildConfigChild (bool isDebug)
{
    ValueTree child (Ids::CONFIGURATIONS);

    child.setProperty (Ids::name, isDebug ? "Debug" : "Release", nullptr);
    child.setProperty (Ids::isDebug, isDebug ? 1 : 0, nullptr);
    child.setProperty (Ids::optimisation, isDebug ? 1 : 3, nullptr);
    child.setProperty (Ids::targetName, metadata[Ids::name], nullptr);

    return child;
}

ValueTree PIPGenerator::createExporterChild (const String& exporterName)
{
    ValueTree exporter (exporterName);

    exporter.setProperty (Ids::targetFolder, "Builds/" + ProjectExporter::getTargetFolderForExporter (exporterName), nullptr);

    if (isHExample (pipFile) && exporterRequiresExampleAssets (exporterName, metadata[Ids::name]))
    {
        auto examplesDir = getExamplesDirectory();

        if (examplesDir != File())
        {
            auto assetsDirectoryPath = examplesDir.getChildFile ("Assets").getFullPathName();

            exporter.setProperty (exporterName == "ANDROIDSTUDIO" ? Ids::androidExtraAssetsFolder
                                                                  : Ids::customXcodeResourceFolders,
                                  assetsDirectoryPath, nullptr);
        }
        else
        {
            // invalid H path
            HAssertfalse;
        }
    }

    {
        ValueTree configs (Ids::CONFIGURATIONS);

        configs.addChild (createBuildConfigChild (true), -1, nullptr);
        configs.addChild (createBuildConfigChild (false), -1, nullptr);

        exporter.addChild (configs, -1, nullptr);
    }

    {
        ValueTree modulePaths (Ids::MODULEPATHS);

        auto modules = StringArray::fromTokens (metadata[Ids::dependencies_].toString(), ",", {});

        for (auto m : modules)
            modulePaths.addChild (createModulePathChild (m.trim()), -1, nullptr);

        exporter.addChild (modulePaths, -1, nullptr);
    }

    return exporter;
}

ValueTree PIPGenerator::createModuleChild (const String& moduleID)
{
    ValueTree module (Ids::MODULE);

    module.setProperty (Ids::ID, moduleID, nullptr);
    module.setProperty (Ids::showAllCode, 1, nullptr);
    module.setProperty (Ids::useLocalCopy, 0, nullptr);
    module.setProperty (Ids::useGlobalPath, (getPathForModule (moduleID).empty() ? 1 : 0), nullptr);

    return module;
}

void PIPGenerator::addExporters (ValueTree& HrTree)
{
    ValueTree exportersTree (Ids::EXPORTFORMATS);

    auto exporters = StringArray::fromTokens (metadata[Ids::exporters].toString(), ",", {});

    for (auto& e : exporters)
    {
        e = e.trim().std::toupper();

        if (isValidExporterName (e))
            exportersTree.addChild (createExporterChild (e), -1, nullptr);
    }

    HrTree.addChild (exportersTree, -1, nullptr);
}

void PIPGenerator::addModules (ValueTree& HrTree)
{
    ValueTree modulesTree (Ids::MODULES);

    auto modules = StringArray::fromTokens (metadata[Ids::dependencies_].toString(), ",", {});
    modules.trim();

    auto projectType = metadata[Ids::type].toString();

    if (projectType == "Console")
        modules.mergeArray (getModulesRequiredForConsole());
    else if (projectType == "Component")
        modules.mergeArray (getModulesRequiredForComponent());
    else if (projectType == "AudioProcessor")
        modules.mergeArray (getModulesRequiredForAudioProcessor());

    for (auto& m : modules)
        modulesTree.addChild (createModuleChild (m.trim()), -1, nullptr);

    HrTree.addChild (modulesTree, -1, nullptr);
}

Result PIPGenerator::setProjectSettings (ValueTree& HrTree)
{
    HrTree.setProperty (Ids::name, metadata[Ids::name], nullptr);
    HrTree.setProperty (Ids::companyName, metadata[Ids::vendor], nullptr);
    HrTree.setProperty (Ids::version, metadata[Ids::version], nullptr);
    HrTree.setProperty (Ids::userNotes, metadata[Ids::description], nullptr);
    HrTree.setProperty (Ids::companyWebsite, metadata[Ids::website], nullptr);

    auto defines = metadata[Ids::defines].toString();

    if (isHExample (pipFile))
    {
        auto examplesDir = getExamplesDirectory();

        if (examplesDir != File())
        {
             defines += ((defines.empty() ? "" : " ") + String ("PIP_HEXAMPLES_DIRECTORY=")
                         + Base64::toBase64 (examplesDir.getFullPathName()));
        }
        else
        {
            return Result::fail (String ("Invalid H path. Set path to H via ") +
                                 (TargetOS::getThisOS() == TargetOS::osx ? "\"Projector->Global Paths...\""
                                                                         : "\"File->Global Paths...\"")
                                 + " menu item.");
        }
    }

    HrTree.setProperty (Ids::defines, defines, nullptr);

    auto type = metadata[Ids::type].toString();

    if (type == "Console")
    {
        HrTree.setProperty (Ids::projectType, "consoleapp", nullptr);
    }
    else if (type == "Component")
    {
        HrTree.setProperty (Ids::projectType, "guiapp", nullptr);
    }
    else if (type == "AudioProcessor")
    {
        HrTree.setProperty (Ids::projectType, "audioplug", nullptr);
        HrTree.setProperty (Ids::pluginManufacturer, metadata[Ids::vendor], nullptr);
        HrTree.setProperty (Ids::pluginAUIsSandboxSafe, "1", nullptr);

        StringArray pluginFormatsToBuild (Ids::buildVST3.toString(), Ids::buildAU.toString(), Ids::buildStandalone.toString());
        pluginFormatsToBuild.addArray (getExtraPluginFormatsToBuild());

        HrTree.setProperty (Ids::pluginFormats, pluginFormatsToBuild.joinIntoString (","), nullptr);

        if (! getPluginCharacteristics().empty())
            HrTree.setProperty (Ids::pluginCharacteristicsValue, getPluginCharacteristics().joinIntoString (","), nullptr);
    }

    return Result::ok();
}

void PIPGenerator::setModuleFlags (ValueTree& HrTree)
{
    ValueTree options ("HOPTIONS");

    for (auto& option : StringArray::fromTokens (metadata[Ids::moduleFlags].toString(), ",", {}))
    {
        auto name  = option.upToFirstOccurrenceOf ("=", false, true).trim();
        auto value = option.fromFirstOccurrenceOf ("=", false, true).trim();

        options.setProperty (name, (value == "1" ? 1 : 0), nullptr);
    }

    if (metadata[Ids::type].toString() == "AudioProcessor"
          && options.getPropertyPointer ("HVST3_CAN_REPLACE_VST2") == nullptr)
        options.setProperty ("HVST3_CAN_REPLACE_VST2", 0, nullptr);

    HrTree.addChild (options, -1, nullptr);
}

String PIPGenerator::getMainFileTextForType()
{
    String mainTemplate (BinaryData::PIPMain_cpp);

    mainTemplate = mainTemplate.replace ("%%filename%%", useLocalCopy ? pipFile.getFileName()
                                                                      : isTemp ? pipFile.getFullPathName()
                                                                               : RelativePath (pipFile, outputDirectory.getChildFile ("Source"),
                                                                                               RelativePath::unknown).toUnixStyle());

    auto type = metadata[Ids::type].toString();

    if (type == "Console")
    {
        mainTemplate = removeEnclosed (mainTemplate, "%%component_begin%%", "%%component_end%%");
        mainTemplate = removeEnclosed (mainTemplate, "%%audioprocessor_begin%%", "%%audioprocessor_end%%");
        mainTemplate = mainTemplate.replace ("%%console_begin%%", {}).replace ("%%console_end%%", {});

        return ensureCorrectWhitespace (mainTemplate);
    }
    else if (type == "Component")
    {
        mainTemplate = removeEnclosed (mainTemplate, "%%audioprocessor_begin%%", "%%audioprocessor_end%%");
        mainTemplate = removeEnclosed (mainTemplate, "%%console_begin%%", "%%console_end%%");
        mainTemplate = mainTemplate.replace ("%%component_begin%%", {}).replace ("%%component_end%%", {});

        mainTemplate = mainTemplate.replace ("%%project_name%%",    metadata[Ids::name].toString());
        mainTemplate = mainTemplate.replace ("%%project_version%%", metadata[Ids::version].toString());

        return ensureCorrectWhitespace (mainTemplate.replace ("%%startup%%", "mainWindow.reset (new MainWindow (" + metadata[Ids::name].toString().quoted()
                                                            + ", new " + metadata[Ids::mainClass].toString() + "(), *this));")
                                                    .replace ("%%shutdown%%", "mainWindow = nullptr;"));
    }
    else if (type == "AudioProcessor")
    {
        mainTemplate = removeEnclosed (mainTemplate, "%%component_begin%%", "%%component_end%%");
        mainTemplate = removeEnclosed (mainTemplate, "%%console_begin%%", "%%console_end%%");
        mainTemplate = mainTemplate.replace ("%%audioprocessor_begin%%", {}).replace ("%%audioprocessor_end%%", {});

        return ensureCorrectWhitespace (mainTemplate.replace ("%%class_name%%", metadata[Ids::mainClass].toString()));
    }

    return {};
}

//==============================================================================
Array<File> PIPGenerator::replaceRelativeIncludesAndGetFilesToMove()
{
    StringArray lines;
    pipFile.readLines (lines);
    Array<File> files;

    for (auto& line : lines)
    {
        if (line.contains ("#include") && ! line.contains ("JuceLibraryCode"))
        {
            auto path = line.fromFirstOccurrenceOf ("#include", false, false);
            path = path.removeCharacters ("\"").trim();

            if (path.startsWith ("<") && path.endsWith (">"))
                continue;

            auto file = pipFile.getParentDirectory().getChildFile (path);
            files.add (file);

            line = line.replace (path, file.getFileName());
        }
    }

    outputDirectory.getChildFile ("Source")
                   .getChildFile (pipFile.getFileName())
                   .replaceWithText (joinLinesIntoSourceFile (lines));

    return files;
}

bool PIPGenerator::copyRelativeFileToLocalSourceDirectory (const File& fileToCopy) const noexcept
{
    return fileToCopy.copyFileTo (outputDirectory.getChildFile ("Source")
                                                 .getChildFile (fileToCopy.getFileName()));
}

StringArray PIPGenerator::getExtraPluginFormatsToBuild() const
{
    auto name = metadata[Ids::name].toString();

    if (name == "AUv3SynthPlugin" || name == "AudioPluginDemo")
        return { Ids::buildAUv3.toString() };
    else if (name == "InterAppAudioEffectPlugin")
        return { Ids::enableIAA.toString() };

    return {};
}

StringArray PIPGenerator::getPluginCharacteristics() const
{
    auto name = metadata[Ids::name].toString();

    if (name == "AudioPluginDemo")
        return { Ids::pluginIsSynth.toString(),
                 Ids::pluginWantsMidiIn.toString(),
                 Ids::pluginProducesMidiOut.toString(),
                 Ids::pluginEditorRequiresKeys.toString() };
    else if (name == "AUv3SynthPlugin" || name == "MultiOutSynthPlugin")
        return { Ids::pluginIsSynth.toString(),
                 Ids::pluginWantsMidiIn.toString(),
                 Ids::pluginIsSynth.toString() };
    else if (name == "ArpeggiatorPlugin")
        return { Ids::pluginWantsMidiIn.toString(),
                 Ids::pluginProducesMidiOut.toString(),
                 Ids::pluginIsMidiEffectPlugin.toString() };

    return {};
}

String PIPGenerator::getPathForModule (const String& moduleID) const
{
    if (isHModule (moduleID))
    {
        if (HModulesPath != File())
        {
            if (isTemp)
                return HModulesPath.getFullPathName();

            return RelativePath (HModulesPath, outputDirectory, RelativePath::projectFolder).toUnixStyle();
        }
    }
    else if (availableUserModules != nullptr)
    {
        auto moduleRoot = availableUserModules->getModuleWithID (moduleID).second.getParentDirectory();

        if (isTemp)
            return moduleRoot.getFullPathName();

        return RelativePath (moduleRoot , outputDirectory, RelativePath::projectFolder).toUnixStyle();
    }

    return {};
}

File PIPGenerator::getExamplesDirectory() const
{
    if (HModulesPath != File())
    {
        auto examples = HModulesPath.getSiblingFile ("examples");

        if (isValidHExamplesDirectory (examples))
            return examples;
    }

    auto examples = File (getAppSettings().getStoredPath (Ids::HPath, TargetOS::getThisOS()).get().toString()).getChildFile ("examples");

    if (isValidHExamplesDirectory (examples))
        return examples;

    return {};
}
