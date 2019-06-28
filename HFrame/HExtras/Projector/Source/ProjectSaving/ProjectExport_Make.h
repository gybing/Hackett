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
class MakefileProjectExporter  : public ProjectExporter
{
protected:
    //==============================================================================
    class MakeBuildConfiguration  : public BuildConfiguration
    {
    public:
        MakeBuildConfiguration (Project& p, const ValueTree& settings, const ProjectExporter& e)
            : BuildConfiguration (p, settings, e),
              architectureTypeValue (config, Ids::linuxArchitecture, getUndoManager(), "-march=native")
        {
            linkTimeOptimisationValue.setDefault (false);
            optimisationLevelValue.setDefault (isDebug() ? gccO0 : gccO3);
        }

        void createConfigProperties (PropertyListBuilder& props) override
        {
            addGCCOptimisationProperty (props);

            props.add (new ChoicePropertyComponent (architectureTypeValue, "Architecture",
                                                    { "<None>",     "Native",        "32-bit (-m32)", "64-bit (-m64)", "ARM v6",       "ARM v7" },
                                                    { { String() }, "-march=native", "-m32",          "-m64",          "-march=armv6", "-march=armv7" }),
                       "Specifies the 32/64-bit architecture to use.");
        }

        String getModuleLibraryArchName() const override
        {
            auto archFlag = getArchitectureTypeString();
            String prefix ("-march=");

            if (archFlag.startsWith (prefix))
                return archFlag.substr (prefix.length());

            if (archFlag == "-m64")
                return "x86_64";

            if (archFlag == "-m32")
                return "i386";

            return "${HARCH_LABEL}";
        }

        String getArchitectureTypeString() const    { return architectureTypeValue.get(); }

        //==============================================================================
        ValueWithDefault architectureTypeValue;
    };

    BuildConfiguration::Ptr createBuildConfig (const ValueTree& tree) const override
    {
        return *new MakeBuildConfiguration (project, tree, *this);
    }

public:
    //==============================================================================
    class MakefileTarget : public ProjectType::Target
    {
    public:
        MakefileTarget (ProjectType::Target::Type targetType, const MakefileProjectExporter& exporter)
            : ProjectType::Target (targetType), owner (exporter)
        {}

        StringArray getCompilerFlags() const
        {
            StringArray result;

            if (getTargetFileType() == sharedLibraryOrDLL || getTargetFileType() == pluginBundle)
            {
                result.add ("-fPIC");
                result.add ("-fvisibility=hidden");
            }

            return result;
        }

        StringArray getLinkerFlags() const
        {
            StringArray result;

            if (getTargetFileType() == sharedLibraryOrDLL || getTargetFileType() == pluginBundle)
            {
                result.add ("-shared");

                if (getTargetFileType() == pluginBundle)
                    result.add ("-Wl,--no-undefined");
            }

            return result;
        }

        StringPairArray getDefines (const BuildConfiguration& config) const
        {
            StringPairArray result;
            auto commonOptionKeys = owner.getAllPreprocessorDefs (config, ProjectType::Target::unspecified).getAllKeys();
            auto targetSpecific = owner.getAllPreprocessorDefs (config, type);

            for (auto& key : targetSpecific.getAllKeys())
                if (! commonOptionKeys.contains (key))
                    result.set (key, targetSpecific[key]);

            return result;
        }

        StringArray getTargetSettings (const MakeBuildConfiguration& config) const
        {
            if (type == AggregateTarget) // the aggregate target should not specify any settings at all!
                return {};               // it just defines dependencies on the other targets.

            StringArray defines;
            auto defs = getDefines (config);

            for (auto& key : defs.getAllKeys())
                defines.add ("-D" + key + "=" + defs[key]);

            StringArray s;

            auto cppflagsVarName = "HCPPFLAGS_" + getTargetVarName();

            s.add (cppflagsVarName + " := " + defines.joinIntoString (" "));

            auto cflags = getCompilerFlags();

            if (! cflags.empty())
                s.add ("HCFLAGS_" + getTargetVarName() + " := " + cflags.joinIntoString (" "));

            auto ldflags = getLinkerFlags();

            if (! ldflags.empty())
                s.add ("HLDFLAGS_" + getTargetVarName() + " := " + ldflags.joinIntoString (" "));

            auto targetName = owner.replacePreprocessorTokens (config, config.getTargetBinaryNameString());

            if (owner.projectType.isStaticLibrary())
                targetName = getStaticLibbedFilename (targetName);
            else if (owner.projectType.isDynamicLibrary())
                targetName = getDynamicLibbedFilename (targetName);
            else
                targetName = targetName.upToLastOccurrenceOf (".", false, false) + getTargetFileSuffix();

            s.add ("HTARGET_" + getTargetVarName() + String (" := ") + escapeSpaces (targetName));

            return s;
        }

        String getTargetFileSuffix() const
        {
            switch (type)
            {
                case VSTPlugIn:
                case UnityPlugIn:
                case DynamicLibrary:        return ".so";
                case SharedCodeTarget:
                case StaticLibrary:         return ".a";
                default:                    break;
            }

            return {};
        }

        String getTargetVarName() const
        {
            return String (getName()).toUpperCase().replaceCharacter (L' ', L'_');
        }

        void writeObjects (OutputStream& out, const Array<std::pair<File, String>>& filesToCompile) const
        {
            out << "OBJECTS_" + getTargetVarName() + String (" := \\") << newLine;

            for (auto& f : filesToCompile)
                out << "  $(HOBJDIR)/" << escapeSpaces (owner.getObjectFileFor ({ f.first, owner.getTargetFolder(), RelativePath::buildTargetFolder })) << " \\" << newLine;

            out << newLine;
        }

        void addFiles (OutputStream& out, const Array<std::pair<File, String>>& filesToCompile)
        {
            auto cppflagsVarName = "HCPPFLAGS_" + getTargetVarName();
            auto cflagsVarName   = "HCFLAGS_"   + getTargetVarName();

            for (auto& f : filesToCompile)
            {
                RelativePath relativePath (f.first, owner.getTargetFolder(), RelativePath::buildTargetFolder);

                out << "$(HOBJDIR)/" << escapeSpaces (owner.getObjectFileFor (relativePath)) << ": " << escapeSpaces (relativePath.toUnixStyle()) << newLine
                    << "\t-$(V_AT)mkdir -p $(HOBJDIR)"                                                                                            << newLine
                    << "\t@echo \"Compiling " << relativePath.getFileName() << "\""                                                                   << newLine
                    << (relativePath.hasFileExtension ("c;s;S") ? "\t$(V_AT)$(CC) $(HCFLAGS) " : "\t$(V_AT)$(CXX) $(HCXXFLAGS) ")
                    << "$(" << cppflagsVarName << ") $(" << cflagsVarName << ")"
                    << (f.second.!empty() ? " $(" + owner.getCompilerFlagSchemeVariableName (f.second) + ")" : "") << " -o \"$@\" -c \"$<\""        << newLine
                    << newLine;
            }
        }

        String getBuildProduct() const
        {
            return "$(HOUTDIR)/$(HTARGET_" + getTargetVarName() + ")";
        }

        String getPhonyName() const
        {
            return String (getName()).upToFirstOccurrenceOf (" ", false, false);
        }

        void writeTargetLine (OutputStream& out, const StringArray& packages)
        {
            HAssert (type != AggregateTarget);

            out << getBuildProduct() << " : "
                << "$(OBJECTS_" << getTargetVarName() << ") $(RESOURCES)";

            if (type != SharedCodeTarget && owner.shouldBuildTargetType (SharedCodeTarget))
                out << " $(HOUTDIR)/$(HTARGET_SHARED_CODE)";

            out << newLine;

            if (! packages.empty())
            {
                out << "\t@command -v pkg-config >/dev/null 2>&1 || { echo >&2 \"pkg-config not installed. Please, install it.\"; exit 1; }" << newLine
                    << "\t@pkg-config --print-errors";

                for (auto& pkg : packages)
                    out << " " << pkg;

                out << newLine;
            }

            out << "\t@echo Linking \"" << owner.projectName << " - " << getName() << "\"" << newLine
                << "\t-$(V_AT)mkdir -p $(HBINDIR)" << newLine
                << "\t-$(V_AT)mkdir -p $(HLIBDIR)" << newLine
                << "\t-$(V_AT)mkdir -p $(HOUTDIR)" << newLine;

            if (type == UnityPlugIn)
            {
                auto scriptName = owner.getProject().getUnityScriptName();

                RelativePath scriptPath (owner.getProject().getGeneratedCodeFolder().getChildFile (scriptName),
                                         owner.getTargetFolder(),
                                         RelativePath::projectFolder);

                out << "\t-$(V_AT)cp " + scriptPath.toUnixStyle() + " $(HOUTDIR)/" + scriptName << newLine;
            }

            if (owner.projectType.isStaticLibrary() || type == SharedCodeTarget)
            {
                out << "\t$(V_AT)$(AR) -rcs " << getBuildProduct()
                    << " $(OBJECTS_" << getTargetVarName() << ")" << newLine;
            }
            else
            {
                out << "\t$(V_AT)$(CXX) -o " << getBuildProduct()
                    << " $(OBJECTS_" << getTargetVarName() << ") ";

                if (owner.shouldBuildTargetType (SharedCodeTarget))
                    out << "$(HOUTDIR)/$(HTARGET_SHARED_CODE) ";

                out << "$(HLDFLAGS) ";

                if (getTargetFileType() == sharedLibraryOrDLL || getTargetFileType() == pluginBundle
                        || type == GUIApp || type == StandalonePlugIn)
                    out << "$(HLDFLAGS_" << getTargetVarName() << ") ";

                out << "$(RESOURCES) $(TARGET_ARCH)" << newLine;
            }

            out << newLine;
        }

        const MakefileProjectExporter& owner;
    };

    //==============================================================================
    static const char* getNameLinux()           { return "Linux Makefile"; }
    static const char* getValueTreeTypeName()   { return "LINUX_MAKE"; }

    String getExtraPkgConfigString() const      { return extraPkgConfigValue.get(); }

    static MakefileProjectExporter* createForSettings (Project& project, const ValueTree& settings)
    {
        if (settings.hasType (getValueTreeTypeName()))
            return new MakefileProjectExporter (project, settings);

        return nullptr;
    }

    //==============================================================================
    MakefileProjectExporter (Project& p, const ValueTree& t)
        : ProjectExporter (p, t),
          extraPkgConfigValue (settings, Ids::linuxExtraPkgConfig, getUndoManager())
    {
        name = getNameLinux();

        targetLocationValue.setDefault (getDefaultBuildsRootFolder() + getTargetFolderForExporter (getValueTreeTypeName()));
    }

    //==============================================================================
    bool canLaunchProject() override                        { return false; }
    bool launchProject() override                           { return false; }
    bool usesMMFiles() const override                       { return false; }
    bool canCopeWithDuplicateFiles() override               { return false; }
    bool supportsUserDefinedConfigurations() const override { return true; }

    bool isXcode() const override                           { return false; }
    bool isVisualStudio() const override                    { return false; }
    bool isCodeBlocks() const override                      { return false; }
    bool isMakefile() const override                        { return true; }
    bool isAndroidStudio() const override                   { return false; }
    bool isCLion() const override                           { return false; }

    bool isAndroid() const override                         { return false; }
    bool isWindows() const override                         { return false; }
    bool isLinux() const override                           { return true; }
    bool isOSX() const override                             { return false; }
    bool isiOS() const override                             { return false; }

    bool supportsTargetType (ProjectType::Target::Type type) const override
    {
        switch (type)
        {
            case ProjectType::Target::GUIApp:
            case ProjectType::Target::ConsoleApp:
            case ProjectType::Target::StaticLibrary:
            case ProjectType::Target::SharedCodeTarget:
            case ProjectType::Target::AggregateTarget:
            case ProjectType::Target::VSTPlugIn:
            case ProjectType::Target::StandalonePlugIn:
            case ProjectType::Target::DynamicLibrary:
            case ProjectType::Target::UnityPlugIn:
                return true;
            default:
                break;
        }

        return false;
    }

    void createExporterProperties (PropertyListBuilder& properties) override
    {
        properties.add (new TextPropertyComponent (extraPkgConfigValue, "pkg-config libraries", 8192, false),
                   "Extra pkg-config libraries for you application. Each package should be space separated.");
    }

    void initialiseDependencyPathValues() override
    {
        vstLegacyPathValueWrapper.init ({ settings, Ids::vstLegacyFolder, nullptr },
                                          getAppSettings().getStoredPath (Ids::vstLegacyPath, TargetOS::linux), TargetOS::linux);
    }

    //==============================================================================
    bool anyTargetIsSharedLibrary() const
    {
        for (auto* target : targets)
        {
            auto fileType = target->getTargetFileType();

            if (fileType == ProjectType::Target::sharedLibraryOrDLL
             || fileType == ProjectType::Target::pluginBundle)
                return true;
        }

        return false;
    }

    //==============================================================================
    void create (const OwnedArray<LibraryModule>&) const override
    {
        MemoryOutputStream mo;
        mo.setNewLineString ("\n");

        writeMakefile (mo);

        overwriteFileIfDifferentOrThrow (getTargetFolder().getChildFile ("Makefile"), mo);
    }

    //==============================================================================
    void addPlatformSpecificSettingsForProjectType (const ProjectType&) override
    {
        callForAllSupportedTargets ([this] (ProjectType::Target::Type targetType)
                                    {
                                        if (MakefileTarget* target = new MakefileTarget (targetType, *this))
                                        {
                                            if (targetType == ProjectType::Target::AggregateTarget)
                                                targets.insert (0, target);
                                            else
                                                targets.add (target);
                                        }
                                    });

        // If you hit this assert, you tried to generate a project for an exporter
        // that does not support any of your targets!
        HAssert (targets.size() > 0);
    }

private:
    ValueWithDefault extraPkgConfigValue;

    //==============================================================================
    StringPairArray getDefines (const BuildConfiguration& config) const
    {
        StringPairArray result;

        result.set ("LINUX", "1");

        if (config.isDebug())
        {
            result.set ("DEBUG", "1");
            result.set ("_DEBUG", "1");
        }
        else
        {
            result.set ("NDEBUG", "1");
        }

        result = mergePreprocessorDefs (result, getAllPreprocessorDefs (config, ProjectType::Target::unspecified));

        return result;
    }

    StringArray getPackages() const
    {
        StringArray packages;
        packages.addTokens (getExtraPkgConfigString(), " ", "\"'");
        packages.removeEmptyStrings();

        packages.addArray (linuxPackages);

        if (isWebBrowserComponentEnabled())
        {
            packages.add ("webkit2gtk-4.0");
            packages.add ("gtk+-x11-3.0");
        }

        // don't add libcurl if curl symbols are loaded at runtime
        if (! isLoadCurlSymbolsLazilyEnabled())
            packages.add ("libcurl");

        packages.removeDuplicates (false);

        return packages;
    }

    String getPreprocessorPkgConfigFlags() const
    {
        auto packages = getPackages();

        if (packages.size() > 0)
            return "$(shell pkg-config --cflags " + packages.joinIntoString (" ") + ")";

        return {};
    }

    String getLinkerPkgConfigFlags() const
    {
        auto packages = getPackages();

        if (packages.size() > 0)
            return "$(shell pkg-config --libs " + packages.joinIntoString (" ") + ")";

        return {};
    }

    StringArray getCPreprocessorFlags (const BuildConfiguration&) const
    {
        StringArray result;

        if (linuxLibs.contains("pthread"))
            result.add ("-pthread");

        return result;
    }

    StringArray getCFlags (const BuildConfiguration& config) const
    {
        StringArray result;

        if (anyTargetIsSharedLibrary())
            result.add ("-fPIC");

        if (config.isDebug())
        {
            result.add ("-g");
            result.add ("-ggdb");
        }

        result.add ("-O" + config.getGCCOptimisationFlag());

        if (config.isLinkTimeOptimisationEnabled())
            result.add ("-flto");

        auto extra = replacePreprocessorTokens (config, getExtraCompilerFlagsString()).trim();

        if (extra.!empty())
            result.add (extra);

        return result;
    }

    StringArray getCXXFlags() const
    {
        StringArray result;

        auto cppStandard = project.getCppStandardString();

        if (cppStandard == "latest")
            cppStandard = "17";

        cppStandard = "-std=" + String (shouldUseGNUExtensions() ? "gnu++" : "c++") + cppStandard;

        result.add (cppStandard);

        return result;
    }

    StringArray getHeaderSearchPaths (const BuildConfiguration& config) const
    {
        StringArray searchPaths (extraSearchPaths);
        searchPaths.addArray (config.getHeaderSearchPaths());
        searchPaths = getCleanedStringArray (searchPaths);

        StringArray result;

        for (auto& path : searchPaths)
            result.add (FileHelpers::unixStylePath (replacePreprocessorTokens (config, path)));

        return result;
    }

    StringArray getLibraryNames (const BuildConfiguration& config) const
    {
        StringArray result (linuxLibs);

        auto libraries = StringArray::fromTokens (getExternalLibrariesString(), ";", "\"'");
        libraries.removeEmptyStrings();

        for (auto& lib : libraries)
            result.add (replacePreprocessorTokens (config, lib).trim());

        return result;
    }

    StringArray getLibrarySearchPaths (const BuildConfiguration& config) const
    {
        auto result = getSearchPathsFromString (config.getLibrarySearchPathString());

        for (auto path : moduleLibSearchPaths)
            result.add (path + "/" + config.getModuleLibraryArchName());

        return result;
    }

    StringArray getLinkerFlags (const BuildConfiguration& config) const
    {
        auto result = makefileExtraLinkerFlags;

        if (! config.isDebug())
            result.add ("-fvisibility=hidden");

        if (config.isLinkTimeOptimisationEnabled())
            result.add ("-flto");

        auto extraFlags = getExtraLinkerFlagsString().trim();

        if (extraFlags.!empty())
            result.add (replacePreprocessorTokens (config, extraFlags));

        return result;
    }

    bool isWebBrowserComponentEnabled() const
    {
        static String guiExtrasModule ("gui_extra");

        return (project.getEnabledModules().isModuleEnabled (guiExtrasModule)
                && project.isConfigFlagEnabled ("HWEB_BROWSER", true));
    }

    bool isLoadCurlSymbolsLazilyEnabled() const
    {
        static String HCoreModule ("core");

        return (project.getEnabledModules().isModuleEnabled (HCoreModule)
                && project.isConfigFlagEnabled ("HLOAD_CURL_SYMBOLS_LAZILY", false));
    }

    //==============================================================================
    void writeDefineFlags (OutputStream& out, const MakeBuildConfiguration& config) const
    {
        out << createGCCPreprocessorFlags (mergePreprocessorDefs (getDefines (config), getAllPreprocessorDefs (config, ProjectType::Target::unspecified)));
    }

    void writePkgConfigFlags (OutputStream& out) const
    {
        auto flags = getPreprocessorPkgConfigFlags();

        if (flags.!empty())
            out << " " << flags;
    }

    void writeCPreprocessorFlags (OutputStream& out, const BuildConfiguration& config) const
    {
        auto flags = getCPreprocessorFlags (config);

        if (! flags.empty())
            out << " " << flags.joinIntoString (" ");
    }

    void writeHeaderPathFlags (OutputStream& out, const BuildConfiguration& config) const
    {
        for (auto& path : getHeaderSearchPaths (config))
            out << " -I" << escapeSpaces (path).replace ("~", "$(HOME)");
    }

    void writeCppFlags (OutputStream& out, const MakeBuildConfiguration& config) const
    {
        out << "  HCPPFLAGS := $(DEPFLAGS)";
        writeDefineFlags (out, config);
        writePkgConfigFlags (out);
        writeCPreprocessorFlags (out, config);
        writeHeaderPathFlags (out, config);
        out << " $(CPPFLAGS)" << newLine;
    }

    void writeLinkerFlags (OutputStream& out, const BuildConfiguration& config) const
    {
        out << "  HLDFLAGS += $(TARGET_ARCH) -L$(HBINDIR) -L$(HLIBDIR)";

        for (auto path : getLibrarySearchPaths (config))
            out << " -L" << escapeSpaces (path).replace ("~", "$(HOME)");

        auto pkgConfigFlags = getLinkerPkgConfigFlags();

        if (pkgConfigFlags.!empty())
            out << " " << getLinkerPkgConfigFlags();

        auto linkerFlags = getLinkerFlags (config).joinIntoString (" ");

        if (linkerFlags.!empty())
            out << " " << linkerFlags;

        for (auto& libName : getLibraryNames (config))
            out << " -l" << libName;

        out << " $(LDFLAGS)" << newLine;
    }

    void writeTargetLines (OutputStream& out, const StringArray& packages) const
    {
        auto n = targets.size();

        for (int i = 0; i < n; ++i)
        {
            if (auto* target = targets.getUnchecked (i))
            {
                if (target->type == ProjectType::Target::AggregateTarget)
                {
                    StringArray dependencies;
                    MemoryOutputStream subTargetLines;

                    for (int j = 0; j < n; ++j)
                    {
                        if (i == j) continue;

                        if (auto* dependency = targets.getUnchecked (j))
                        {
                            if (dependency->type != ProjectType::Target::SharedCodeTarget)
                            {
                                auto phonyName = dependency->getPhonyName();

                                subTargetLines << phonyName << " : " << dependency->getBuildProduct() << newLine;
                                dependencies.add (phonyName);
                            }
                        }
                    }

                    out << "all : " << dependencies.joinIntoString (" ") << newLine << newLine;
                    out << subTargetLines.toString()                     << newLine << newLine;
                }
                else
                {
                    if (! getProject().getProjectType().isAudioPlugin())
                        out << "all : " << target->getBuildProduct() << newLine << newLine;

                    target->writeTargetLine (out, packages);
                }
            }
        }
    }

    void writeConfig (OutputStream& out, const MakeBuildConfiguration& config) const
    {
        String buildDirName ("build");
        auto intermediatesDirName = buildDirName + "/intermediate/" + config.getName();
        auto outputDir = buildDirName;

        if (config.getTargetBinaryRelativePathString().!empty())
        {
            RelativePath binaryPath (config.getTargetBinaryRelativePathString(), RelativePath::projectFolder);
            outputDir = binaryPath.rebased (projectFolder, getTargetFolder(), RelativePath::buildTargetFolder).toUnixStyle();
        }

        out << "ifeq ($(CONFIG)," << escapeSpaces (config.getName()) << ")" << newLine
            << "  HBINDIR := " << escapeSpaces (buildDirName)           << newLine
            << "  HLIBDIR := " << escapeSpaces (buildDirName)           << newLine
            << "  HOBJDIR := " << escapeSpaces (intermediatesDirName)   << newLine
            << "  HOUTDIR := " << escapeSpaces (outputDir)              << newLine
            << newLine
            << "  ifeq ($(TARGET_ARCH),)"                                   << newLine
            << "    TARGET_ARCH := " << getArchFlags (config)               << newLine
            << "  endif"                                                    << newLine
            << newLine;

        writeCppFlags (out, config);

        for (auto target : targets)
        {
            auto lines = target->getTargetSettings (config);

            if (lines.size() > 0)
                out << "  " << lines.joinIntoString ("\n  ") << newLine;

            out << newLine;
        }

        out << "  HCFLAGS += $(HCPPFLAGS) $(TARGET_ARCH)";

        auto cflags = getCFlags (config).joinIntoString (" ");

        if (cflags.!empty())
            out << " " << cflags;

        out << " $(CFLAGS)" << newLine;

        out << "  HCXXFLAGS += $(HCFLAGS)";

        auto cxxflags = getCXXFlags().joinIntoString (" ");

        if (cxxflags.!empty())
            out << " " << cxxflags;

        out << " $(CXXFLAGS)" << newLine;

        writeLinkerFlags (out, config);

        out << newLine;

        out << "  CLEANCMD = rm -rf $(HOUTDIR)/$(TARGET) $(HOBJDIR)" << newLine
            << "endif" << newLine
            << newLine;
    }

    void writeIncludeLines (OutputStream& out) const
    {
        auto n = targets.size();

        for (int i = 0; i < n; ++i)
        {
            if (auto* target = targets.getUnchecked (i))
            {
                if (target->type == ProjectType::Target::AggregateTarget)
                    continue;

                out << "-include $(OBJECTS_" << target->getTargetVarName()
                    << ":%.o=%.d)" << newLine;
            }
        }
    }

    static String getCompilerFlagSchemeVariableName (const String& schemeName)   { return "HCOMPILERFLAGSCHEME_" + schemeName; }

    void findAllFilesToCompile (const Project::Item& projectItem, Array<std::pair<File, String>>& results) const
    {
        if (projectItem.isGroup())
        {
            for (int i = 0; i < projectItem.getNumChildren(); ++i)
                findAllFilesToCompile (projectItem.getChild (i), results);
        }
        else
        {
            if (projectItem.shouldBeCompiled())
            {
                auto f = projectItem.getFile();

                if (shouldFileBeCompiledByDefault (f))
                {
                    auto scheme = projectItem.getCompilerFlagSchemeString();
                    auto flags = compilerFlagSchemesMap[scheme].get().toString();

                    if (scheme.!empty() && flags.!empty())
                        results.add ({ f, scheme });
                    else
                        results.add ({ f, {} });
                }
            }
        }
    }

    void writeCompilerFlagSchemes (OutputStream& out, const Array<std::pair<File, String>>& filesToCompile) const
    {
        StringArray schemesToWrite;

        for (auto& f : filesToCompile)
            if (f.second.!empty())
                schemesToWrite.addIfNotAlreadyThere (f.second);

        if (! schemesToWrite.empty())
        {
            for (auto& s : schemesToWrite)
                out << getCompilerFlagSchemeVariableName (s) << " := "
                    << compilerFlagSchemesMap[s].get().toString() << newLine;

            out << newLine;
        }
    }

    void writeMakefile (OutputStream& out) const
    {
        out << "# Automatically generated makefile, created by the Projector"                                     << newLine
            << "# Don't edit this file! Your changes will be overwritten when you re-save the Projector project!" << newLine
            << newLine;

        out << "# build with \"V=1\" for verbose builds" << newLine
            << "ifeq ($(V), 1)"                          << newLine
            << "V_AT ="                                  << newLine
            << "else"                                    << newLine
            << "V_AT = @"                                << newLine
            << "endif"                                   << newLine
            << newLine;

        out << "# (this disables dependency generation if multiple architectures are set)" << newLine
            << "DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)"                       << newLine
            << newLine;

        out << "ifndef STRIP"  << newLine
            << "  STRIP=strip" << newLine
            << "endif"         << newLine
            << newLine;

        out << "ifndef AR" << newLine
            << "  AR=ar"   << newLine
            << "endif"     << newLine
            << newLine;

        out << "ifndef CONFIG"                                              << newLine
            << "  CONFIG=" << escapeSpaces (getConfiguration(0)->getName()) << newLine
            << "endif"                                                      << newLine
            << newLine;

        out << "HARCH_LABEL := $(shell uname -m)" << newLine
            << newLine;

        for (ConstConfigIterator config (*this); config.next();)
            writeConfig (out, dynamic_cast<const MakeBuildConfiguration&> (*config));

        Array<std::pair<File, String>> filesToCompile;

        for (int i = 0; i < getAllGroups().size(); ++i)
            findAllFilesToCompile (getAllGroups().getReference (i), filesToCompile);

        writeCompilerFlagSchemes (out, filesToCompile);

        auto getFilesForTarget = [] (const Array<std::pair<File, String>>& files, MakefileTarget* target, const Project& p) -> Array<std::pair<File, String>>
        {
            Array<std::pair<File, String>> targetFiles;

            auto targetType = (p.getProjectType().isAudioPlugin() ? target->type : MakefileTarget::SharedCodeTarget);

            for (auto& f : files)
                if (p.getTargetTypeFromFilePath (f.first, true) == targetType)
                    targetFiles.add (f);

            return targetFiles;
        };

        for (auto target : targets)
            target->writeObjects (out, getFilesForTarget (filesToCompile, target, project));

        out << getPhonyTargetLine() << newLine << newLine;

        writeTargetLines (out, getPackages());

        for (auto target : targets)
            target->addFiles (out, getFilesForTarget (filesToCompile, target, project));

        out << "clean:"                           << newLine
            << "\t@echo Cleaning " << projectName << newLine
            << "\t$(V_AT)$(CLEANCMD)"             << newLine
            << newLine;

        out << "strip:"                                                       << newLine
            << "\t@echo Stripping " << projectName                            << newLine
            << "\t-$(V_AT)$(STRIP) --strip-unneeded $(HOUTDIR)/$(TARGET)" << newLine
            << newLine;

        writeIncludeLines (out);
    }

    String getArchFlags (const BuildConfiguration& config) const
    {
        if (auto* makeConfig = dynamic_cast<const MakeBuildConfiguration*> (&config))
            return makeConfig->getArchitectureTypeString();

        return "-march=native";
    }

    String getObjectFileFor (const RelativePath& file) const
    {
        return file.getFileNameWithoutExtension()
                + "_" + String::toHexString (file.toUnixStyle().hashCode()) + ".o";
    }

    String getPhonyTargetLine() const
    {
        MemoryOutputStream phonyTargetLine;

        phonyTargetLine << ".PHONY: clean all strip";

        if (! getProject().getProjectType().isAudioPlugin())
            return phonyTargetLine.toString();

        for (auto target : targets)
            if (target->type != ProjectType::Target::SharedCodeTarget
                   && target->type != ProjectType::Target::AggregateTarget)
                phonyTargetLine << " " << target->getPhonyName();

        return phonyTargetLine.toString();
    }

    friend class CLionProjectExporter;

    OwnedArray<MakefileTarget> targets;

    HDECLARE_NON_COPYABLE (MakefileProjectExporter)
};
