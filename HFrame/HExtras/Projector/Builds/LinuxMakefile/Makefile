# Automatically generated makefile, created by the Projector
# Don't edit this file! Your changes will be overwritten when you re-save the Projector project!

# build with "V=1" for verbose builds
ifeq ($(V), 1)
V_AT =
else
V_AT = @
endif

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifndef STRIP
  STRIP=strip
endif

ifndef AR
  AR=ar
endif

ifndef CONFIG
  CONFIG=Debug
endif

HARCH_LABEL := $(shell uname -m)

ifeq ($(CONFIG),Debug)
  HBINDIR := build
  HLIBDIR := build
  HOBJDIR := build/intermediate/Debug
  HOUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  HCPPFLAGS := $(DEPFLAGS) -DLINUX=1 -DDEBUG=1 -D_DEBUG=1 -DHR_LINUX_MAKE_6D53C8B4=1 -DHAPP_VERSION=5.4.3 -DHAPP_VERSION_HEX=0x50403 $(shell pkg-config --cflags x11 xinerama xext freetype2 webkit2gtk-4.0 gtk+-x11-3.0) -pthread -I../../JuceLibraryCode -I../../../../modules $(CPPFLAGS)
  HCPPFLAGS_APP := -DJucePlugin_Build_VST=0 -DJucePlugin_Build_VST3=0 -DJucePlugin_Build_AU=0 -DJucePlugin_Build_AUv3=0 -DJucePlugin_Build_RTAS=0 -DJucePlugin_Build_AAX=0 -DJucePlugin_Build_Standalone=0 -DJucePlugin_Build_Unity=0
  HTARGET_APP := Projector

  HCFLAGS += $(HCPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0 $(CFLAGS)
  HCXXFLAGS += $(HCFLAGS) -std=c++11 $(CXXFLAGS)
  HLDFLAGS += $(TARGET_ARCH) -L$(HBINDIR) -L$(HLIBDIR) $(shell pkg-config --libs x11 xinerama xext freetype2 webkit2gtk-4.0 gtk+-x11-3.0) -lrt -ldl -lpthread $(LDFLAGS)

  CLEANCMD = rm -rf $(HOUTDIR)/$(TARGET) $(HOBJDIR)
endif

ifeq ($(CONFIG),Release)
  HBINDIR := build
  HLIBDIR := build
  HOBJDIR := build/intermediate/Release
  HOUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  HCPPFLAGS := $(DEPFLAGS) -DLINUX=1 -DNDEBUG=1 -DHR_LINUX_MAKE_6D53C8B4=1 -DHAPP_VERSION=5.4.3 -DHAPP_VERSION_HEX=0x50403 $(shell pkg-config --cflags x11 xinerama xext freetype2 webkit2gtk-4.0 gtk+-x11-3.0) -pthread -I../../JuceLibraryCode -I../../../../modules $(CPPFLAGS)
  HCPPFLAGS_APP := -DJucePlugin_Build_VST=0 -DJucePlugin_Build_VST3=0 -DJucePlugin_Build_AU=0 -DJucePlugin_Build_AUv3=0 -DJucePlugin_Build_RTAS=0 -DJucePlugin_Build_AAX=0 -DJucePlugin_Build_Standalone=0 -DJucePlugin_Build_Unity=0
  HTARGET_APP := Projector

  HCFLAGS += $(HCPPFLAGS) $(TARGET_ARCH) -O3 $(CFLAGS)
  HCXXFLAGS += $(HCFLAGS) -std=c++11 $(CXXFLAGS)
  HLDFLAGS += $(TARGET_ARCH) -L$(HBINDIR) -L$(HLIBDIR) $(shell pkg-config --libs x11 xinerama xext freetype2 webkit2gtk-4.0 gtk+-x11-3.0) -fvisibility=hidden -lrt -ldl -lpthread $(LDFLAGS)

  CLEANCMD = rm -rf $(HOUTDIR)/$(TARGET) $(HOBJDIR)
endif

OBJECTS_APP := \
  $(HOBJDIR)/AutoUpdater_ca658dc2.o \
  $(HOBJDIR)/CommandLine_f35de107.o \
  $(HOBJDIR)/Main_f8488f5b.o \
  $(HOBJDIR)/MainWindow_1e163aeb.o \
  $(HOBJDIR)/ProjectorAnalytics_5740f336.o \
  $(HOBJDIR)/DocumentEditorComponent_bc853a2f.o \
  $(HOBJDIR)/OpenDocumentManager_ba866622.o \
  $(HOBJDIR)/SourceCodeEditor_55965985.o \
  $(HOBJDIR)/ComponentTypeHandler_d7e70242.o \
  $(HOBJDIR)/ButtonDocument_b2cabdec.o \
  $(HOBJDIR)/ComponentDocument_1d8c1063.o \
  $(HOBJDIR)/ColouredElement_acb45c39.o \
  $(HOBJDIR)/PaintElement_db652060.o \
  $(HOBJDIR)/PaintElementGroup_256346c9.o \
  $(HOBJDIR)/PaintElementImage_49702ba5.o \
  $(HOBJDIR)/PaintElementPath_d1b280a5.o \
  $(HOBJDIR)/ComponentLayoutEditor_4fc44b8d.o \
  $(HOBJDIR)/ComponentOverlayComponent_5e028963.o \
  $(HOBJDIR)/EditingPanelBase_28115dee.o \
  $(HOBJDIR)/JucerDocumentEditor_89371a26.o \
  $(HOBJDIR)/PaintRoutineEditor_43fd8744.o \
  $(HOBJDIR)/PaintRoutinePanel_83779cd7.o \
  $(HOBJDIR)/ResourceEditorPanel_237eee2.o \
  $(HOBJDIR)/TestComponent_1a4179c4.o \
  $(HOBJDIR)/BinaryResources_e0f99b46.o \
  $(HOBJDIR)/ComponentLayout_6ea00129.o \
  $(HOBJDIR)/GeneratedCode_9ca4ef7e.o \
  $(HOBJDIR)/JucerDocument_ff8afcc2.o \
  $(HOBJDIR)/ObjectTypes_4406f01c.o \
  $(HOBJDIR)/PaintRoutine_e1e891ee.o \
  $(HOBJDIR)/LicenseController_9abff0cf.o \
  $(HOBJDIR)/CompileEngineClient_aee8c99c.o \
  $(HOBJDIR)/CompileEngineServer_5d8914.o \
  $(HOBJDIR)/DownloadCompileEngineThread_19bb4bb3.o \
  $(HOBJDIR)/HeaderComponent_1ebf72ba.o \
  $(HOBJDIR)/Module_3f7666a5.o \
  $(HOBJDIR)/Project_c131864a.o \
  $(HOBJDIR)/ProjectExporter_cf377b25.o \
  $(HOBJDIR)/ProjectSaver_4276639b.o \
  $(HOBJDIR)/ResourceFile_74b61849.o \
  $(HOBJDIR)/AppearanceSettings_fa5d736e.o \
  $(HOBJDIR)/StoredSettings_c7168d2d.o \
  $(HOBJDIR)/CodeHelpers_1e797672.o \
  $(HOBJDIR)/FileHelpers_54f12f83.o \
  $(HOBJDIR)/MiscUtilities_31fc8dd8.o \
  $(HOBJDIR)/PIPGenerator_fd3402c7.o \
  $(HOBJDIR)/Icons_d02d18f1.o \
  $(HOBJDIR)/JucerTreeViewBase_9b9f2ff0.o \
  $(HOBJDIR)/ProjectorLookAndFeel_3b20291d.o \
  $(HOBJDIR)/SlidingPanelComponent_4e7dc07e.o \
  $(HOBJDIR)/NewFileWizard_fac97f47.o \
  $(HOBJDIR)/NewProjectWizardClasses_891f6fa2.o \
  $(HOBJDIR)/BinaryData_ce4232d4.o \
  $(HOBJDIR)/include_analytics_f8e9fa94.o \
  $(HOBJDIR)/include_core_f26d17db.o \
  $(HOBJDIR)/include_cryptography_8cb807a8.o \
  $(HOBJDIR)/include_data_structures_7471b1e3.o \
  $(HOBJDIR)/include_events_fd7d695.o \
  $(HOBJDIR)/include_graphics_f817e147.o \
  $(HOBJDIR)/include_gui_basics_e3f79785.o \
  $(HOBJDIR)/include_gui_extra_6dee1c1a.o \

.PHONY: clean all strip

all : $(HOUTDIR)/$(HTARGET_APP)

$(HOUTDIR)/$(HTARGET_APP) : $(OBJECTS_APP) $(RESOURCES)
	@command -v pkg-config >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@pkg-config --print-errors x11 xinerama xext freetype2 webkit2gtk-4.0 gtk+-x11-3.0
	@echo Linking "Projector - App"
	-$(V_AT)mkdir -p $(HBINDIR)
	-$(V_AT)mkdir -p $(HLIBDIR)
	-$(V_AT)mkdir -p $(HOUTDIR)
	$(V_AT)$(CXX) -o $(HOUTDIR)/$(HTARGET_APP) $(OBJECTS_APP) $(HLDFLAGS) $(HLDFLAGS_APP) $(RESOURCES) $(TARGET_ARCH)

$(HOBJDIR)/AutoUpdater_ca658dc2.o: ../../Source/Application/AutoUpdater.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling AutoUpdater.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/CommandLine_f35de107.o: ../../Source/Application/CommandLine.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling CommandLine.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/Main_f8488f5b.o: ../../Source/Application/Main.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling Main.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/MainWindow_1e163aeb.o: ../../Source/Application/MainWindow.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling MainWindow.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/ProjectorAnalytics_5740f336.o: ../../Source/Application/ProjectorAnalytics.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling ProjectorAnalytics.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/DocumentEditorComponent_bc853a2f.o: ../../Source/CodeEditor/DocumentEditorComponent.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling DocumentEditorComponent.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/OpenDocumentManager_ba866622.o: ../../Source/CodeEditor/OpenDocumentManager.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling OpenDocumentManager.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/SourceCodeEditor_55965985.o: ../../Source/CodeEditor/SourceCodeEditor.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling SourceCodeEditor.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/ComponentTypeHandler_d7e70242.o: ../../Source/ComponentEditor/Components/ComponentTypeHandler.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling ComponentTypeHandler.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/ButtonDocument_b2cabdec.o: ../../Source/ComponentEditor/Documents/ButtonDocument.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling ButtonDocument.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/ComponentDocument_1d8c1063.o: ../../Source/ComponentEditor/Documents/ComponentDocument.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling ComponentDocument.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/ColouredElement_acb45c39.o: ../../Source/ComponentEditor/PaintElements/ColouredElement.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling ColouredElement.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/PaintElement_db652060.o: ../../Source/ComponentEditor/PaintElements/PaintElement.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling PaintElement.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/PaintElementGroup_256346c9.o: ../../Source/ComponentEditor/PaintElements/PaintElementGroup.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling PaintElementGroup.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/PaintElementImage_49702ba5.o: ../../Source/ComponentEditor/PaintElements/PaintElementImage.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling PaintElementImage.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/PaintElementPath_d1b280a5.o: ../../Source/ComponentEditor/PaintElements/PaintElementPath.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling PaintElementPath.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/ComponentLayoutEditor_4fc44b8d.o: ../../Source/ComponentEditor/UI/ComponentLayoutEditor.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling ComponentLayoutEditor.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/ComponentOverlayComponent_5e028963.o: ../../Source/ComponentEditor/UI/ComponentOverlayComponent.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling ComponentOverlayComponent.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/EditingPanelBase_28115dee.o: ../../Source/ComponentEditor/UI/EditingPanelBase.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling EditingPanelBase.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/JucerDocumentEditor_89371a26.o: ../../Source/ComponentEditor/UI/JucerDocumentEditor.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling JucerDocumentEditor.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/PaintRoutineEditor_43fd8744.o: ../../Source/ComponentEditor/UI/PaintRoutineEditor.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling PaintRoutineEditor.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/PaintRoutinePanel_83779cd7.o: ../../Source/ComponentEditor/UI/PaintRoutinePanel.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling PaintRoutinePanel.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/ResourceEditorPanel_237eee2.o: ../../Source/ComponentEditor/UI/ResourceEditorPanel.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling ResourceEditorPanel.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/TestComponent_1a4179c4.o: ../../Source/ComponentEditor/UI/TestComponent.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling TestComponent.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/BinaryResources_e0f99b46.o: ../../Source/ComponentEditor/BinaryResources.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling BinaryResources.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/ComponentLayout_6ea00129.o: ../../Source/ComponentEditor/ComponentLayout.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling ComponentLayout.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/GeneratedCode_9ca4ef7e.o: ../../Source/ComponentEditor/GeneratedCode.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling GeneratedCode.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/JucerDocument_ff8afcc2.o: ../../Source/ComponentEditor/JucerDocument.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling JucerDocument.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/ObjectTypes_4406f01c.o: ../../Source/ComponentEditor/ObjectTypes.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling ObjectTypes.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/PaintRoutine_e1e891ee.o: ../../Source/ComponentEditor/PaintRoutine.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling PaintRoutine.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/LicenseController_9abff0cf.o: ../../Source/Licenses/LicenseController.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling LicenseController.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/CompileEngineClient_aee8c99c.o: ../../Source/LiveBuildEngine/CompileEngineClient.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling CompileEngineClient.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/CompileEngineServer_5d8914.o: ../../Source/LiveBuildEngine/CompileEngineServer.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling CompileEngineServer.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/DownloadCompileEngineThread_19bb4bb3.o: ../../Source/LiveBuildEngine/DownloadCompileEngineThread.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling DownloadCompileEngineThread.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/HeaderComponent_1ebf72ba.o: ../../Source/Project/UI/HeaderComponent.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling HeaderComponent.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/Module_3f7666a5.o: ../../Source/Project/Module.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling Module.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/Project_c131864a.o: ../../Source/Project/Project.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling Project.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/ProjectExporter_cf377b25.o: ../../Source/ProjectSaving/ProjectExporter.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling ProjectExporter.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/ProjectSaver_4276639b.o: ../../Source/ProjectSaving/ProjectSaver.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling ProjectSaver.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/ResourceFile_74b61849.o: ../../Source/ProjectSaving/ResourceFile.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling ResourceFile.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/AppearanceSettings_fa5d736e.o: ../../Source/Settings/AppearanceSettings.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling AppearanceSettings.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/StoredSettings_c7168d2d.o: ../../Source/Settings/StoredSettings.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling StoredSettings.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/CodeHelpers_1e797672.o: ../../Source/Utility/Helpers/CodeHelpers.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling CodeHelpers.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/FileHelpers_54f12f83.o: ../../Source/Utility/Helpers/FileHelpers.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling FileHelpers.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/MiscUtilities_31fc8dd8.o: ../../Source/Utility/Helpers/MiscUtilities.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling MiscUtilities.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/PIPGenerator_fd3402c7.o: ../../Source/Utility/PIPs/PIPGenerator.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling PIPGenerator.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/Icons_d02d18f1.o: ../../Source/Utility/UI/Icons.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling Icons.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/JucerTreeViewBase_9b9f2ff0.o: ../../Source/Utility/UI/JucerTreeViewBase.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling JucerTreeViewBase.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/ProjectorLookAndFeel_3b20291d.o: ../../Source/Utility/UI/ProjectorLookAndFeel.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling ProjectorLookAndFeel.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/SlidingPanelComponent_4e7dc07e.o: ../../Source/Utility/UI/SlidingPanelComponent.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling SlidingPanelComponent.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/NewFileWizard_fac97f47.o: ../../Source/Wizards/NewFileWizard.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling NewFileWizard.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/NewProjectWizardClasses_891f6fa2.o: ../../Source/Wizards/NewProjectWizardClasses.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling NewProjectWizardClasses.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/BinaryData_ce4232d4.o: ../../JuceLibraryCode/BinaryData.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling BinaryData.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/include_analytics_f8e9fa94.o: ../../JuceLibraryCode/include_analytics.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling include_analytics.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/include_core_f26d17db.o: ../../JuceLibraryCode/include_core.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling include_core.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/include_cryptography_8cb807a8.o: ../../JuceLibraryCode/include_cryptography.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling include_cryptography.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/include_data_structures_7471b1e3.o: ../../JuceLibraryCode/include_data_structures.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling include_data_structures.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/include_events_fd7d695.o: ../../JuceLibraryCode/include_events.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling include_events.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/include_graphics_f817e147.o: ../../JuceLibraryCode/include_graphics.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling include_graphics.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/include_gui_basics_e3f79785.o: ../../JuceLibraryCode/include_gui_basics.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling include_gui_basics.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

$(HOBJDIR)/include_gui_extra_6dee1c1a.o: ../../JuceLibraryCode/include_gui_extra.cpp
	-$(V_AT)mkdir -p $(HOBJDIR)
	@echo "Compiling include_gui_extra.cpp"
	$(V_AT)$(CXX) $(HCXXFLAGS) $(HCPPFLAGS_APP) $(HCFLAGS_APP) -o "$@" -c "$<"

clean:
	@echo Cleaning Projector
	$(V_AT)$(CLEANCMD)

strip:
	@echo Stripping Projector
	-$(V_AT)$(STRIP) --strip-unneeded $(HOUTDIR)/$(TARGET)

-include $(OBJECTS_APP:%.o=%.d)
