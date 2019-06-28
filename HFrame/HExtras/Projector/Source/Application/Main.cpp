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

#include "Headers.h"

#include "Application.h"
#include "../CodeEditor/OpenDocumentManager.h"
#include "../CodeEditor/SourceCodeEditor.h"
#include "../Utility/UI/PropertyComponents/FilePathPropertyComponent.h"
#include "../Project/UI/Sidebar/TreeItemTypes.h"
#include "Windows/UTF8WindowComponent.h"
#include "Windows/SVGPathDataWindowComponent.h"
#include "Windows/AboutWindowComponent.h"
#include "Windows/ApplicationUsageDataWindowComponent.h"
#include "Windows/EditorColourSchemeWindowComponent.h"
#include "Windows/GlobalPathsWindowComponent.h"
#include "Windows/PIPCreatorWindowComponent.h"
#include "Windows/FloatingToolWindow.h"

#include "../LiveBuildEngine/MessageIDs.h"
#include "../LiveBuildEngine/CppHelpers.h"
#include "../LiveBuildEngine/SourceCodeRange.h"
#include "../LiveBuildEngine/ClassDatabase.h"
#include "../LiveBuildEngine/DiagnosticMessage.h"

#include "../LiveBuildEngine/CompileEngineDLL.h"
#include "../LiveBuildEngine/CompileEngineClient.h"
#include "../LiveBuildEngine/UI/ActivityListComponent.h"
#include "../LiveBuildEngine/UI/BuildTabStatusComponent.h"
#include "../LiveBuildEngine/UI/ComponentListComponent.h"
#include "../LiveBuildEngine/CompileEngineServer.h"

HIMPLEMENT_SINGLETON (CompileEngineDLL)

struct ProjectorAppClasses
{
    #include "../CodeEditor/LiveBuildCodeEditor.h"
    #include "../LiveBuildEngine/UI/ErrorListComponent.h"
};

#include "CommandLine.h"

#include "../Project/UI/ProjectContentComponent.cpp"
#include "Application.cpp"


START_HAPPLICATION (ProjectorApplication)
