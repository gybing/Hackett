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

/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projector to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 H Module Format.txt file.


 BEGIN_HMODULE_DECLARATION

  ID:               gui_basics
  vendor:           H
  version:          5.4.3
  name:             H GUI core classes
  description:      Basic user-interface components and related classes.
  website:          http://www.H.com/H
  license:          GPL/Commercial

  dependencies:     graphics data_structures
  OSXFrameworks:    Cocoa Carbon QuartzCore
  iOSFrameworks:    UIKit MobileCoreServices
  linuxPackages:    x11 xinerama xext

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once
#define HGUI_BASICS_H_INCLUDED

#include <graphics/graphics.h>
#include <data_structures/data_structures.h>

//==============================================================================
/** Config: HENABLE_REPAINT_DEBUGGING
    If this option is turned on, each area of the screen that gets repainted will
    flash in a random colour, so that you can see exactly which bits of your
    components are being drawn.
*/
#ifndef HENABLE_REPAINT_DEBUGGING
 #define HENABLE_REPAINT_DEBUGGING 0
#endif

/** Config: HUSE_XRANDR
    Enables Xrandr multi-monitor support (Linux only).
    Unless you specifically want to disable this, it's best to leave this option turned on.
    Note that your users do not need to have Xrandr installed for your H app to run, as
    the availability of Xrandr is queried during runtime.
*/
#ifndef HUSE_XRANDR
 #define HUSE_XRANDR 1
#endif

/** Config: HUSE_XINERAMA
    Enables Xinerama multi-monitor support (Linux only).
    Unless you specifically want to disable this, it's best to leave this option turned on.
    This will be used as a fallback if HUSE_XRANDR not set or libxrandr cannot be found.
    Note that your users do not need to have Xinerama installed for your H app to run, as
    the availability of Xinerama is queried during runtime.
*/
#ifndef HUSE_XINERAMA
 #define HUSE_XINERAMA 1
#endif

/** Config: HUSE_XSHM
    Enables X shared memory for faster rendering on Linux. This is best left turned on
    unless you have a good reason to disable it.
*/
#ifndef HUSE_XSHM
 #define HUSE_XSHM 1
#endif

/** Config: HUSE_XRENDER
    Enables XRender to allow semi-transparent windowing on Linux.
*/
#ifndef HUSE_XRENDER
 #define HUSE_XRENDER 0
#endif

/** Config: HUSE_XCURSOR
    Uses XCursor to allow ARGB cursor on Linux. This is best left turned on unless you have
    a good reason to disable it.
*/
#ifndef HUSE_XCURSOR
 #define HUSE_XCURSOR 1
#endif

/** Config: HWIN_PER_MONITOR_DPI_AWARE
    Enables per-monitor DPI awareness on Windows 8.1 and above.
*/
#ifndef HWIN_PER_MONITOR_DPI_AWARE
 #define HWIN_PER_MONITOR_DPI_AWARE 1
#endif

//==============================================================================
namespace H
{
    class Component;
    class LookAndFeel;
    class MouseInputSource;
    class MouseInputSourceInternal;
    class ComponentPeer;
    class MouseEvent;
    struct MouseWheelDetails;
    struct PenDetails;
    class ToggleButton;
    class TextButton;
    class AlertWindow;
    class TextLayout;
    class ScrollBar;
    class ComboBox;
    class Button;
    class FilenameComponent;
    class ResizableWindow;
    class MenuBarComponent;
    class GlyphArrangement;
    class TableHeaderComponent;
    class Toolbar;
    class PopupMenu;
    class ProgressBar;
    class FileBrowserComponent;
    class DirectoryContentsDisplayComponent;
    class FilePreviewComponent;
    class CallOutBox;
    class Drawable;
    class DrawablePath;
    class DrawableComposite;
    class CaretComponent;
    class KeyPressMappingSet;
    class ApplicationCommandManagerListener;
    class DrawableButton;
    class Displays;

    class FlexBox;
    #if HHAS_CONSTEXPR
     class Grid;
    #endif
}

#include "mouse/MouseCursor.h"
#include "mouse/MouseListener.h"
#include "keyboard/ModifierKeys.h"
#include "mouse/MouseInputSource.h"
#include "mouse/MouseEvent.h"
#include "keyboard/KeyPress.h"
#include "keyboard/KeyListener.h"
#include "keyboard/KeyboardFocusTraverser.h"
#include "components/ModalComponentManager.h"
#include "components/ComponentListener.h"
#include "components/CachedComponentImage.h"
#include "components/Component.h"
#include "layout/ComponentAnimator.h"
#include "desktop/Desktop.h"
#include "desktop/Displays.h"
#include "layout/ComponentBoundsConstrainer.h"
#include "mouse/ComponentDragger.h"
#include "mouse/DragAndDropTarget.h"
#include "mouse/DragAndDropContainer.h"
#include "mouse/FileDragAndDropTarget.h"
#include "mouse/SelectedItemSet.h"
#include "mouse/MouseInactivityDetector.h"
#include "mouse/TextDragAndDropTarget.h"
#include "mouse/TooltipClient.h"
#include "keyboard/CaretComponent.h"
#include "keyboard/SystemClipboard.h"
#include "keyboard/TextEditorKeyMapper.h"
#include "keyboard/TextInputTarget.h"
#include "commands/ApplicationCommandID.h"
#include "commands/ApplicationCommandInfo.h"
#include "commands/ApplicationCommandTarget.h"
#include "commands/ApplicationCommandManager.h"
#include "commands/KeyPressMappingSet.h"
#include "buttons/Button.h"
#include "buttons/ArrowButton.h"
#include "buttons/DrawableButton.h"
#include "buttons/HyperlinkButton.h"
#include "buttons/ImageButton.h"
#include "buttons/ShapeButton.h"
#include "buttons/TextButton.h"
#include "buttons/ToggleButton.h"
#include "layout/AnimatedPosition.h"
#include "layout/AnimatedPositionBehaviours.h"
#include "layout/ComponentBuilder.h"
#include "layout/ComponentMovementWatcher.h"
#include "layout/ConcertinaPanel.h"
#include "layout/GroupComponent.h"
#include "layout/ResizableBorderComponent.h"
#include "layout/ResizableCornerComponent.h"
#include "layout/ResizableEdgeComponent.h"
#include "layout/ScrollBar.h"
#include "layout/StretchableLayoutManager.h"
#include "layout/StretchableLayoutResizerBar.h"
#include "layout/StretchableObjectResizer.h"
#include "layout/TabbedButtonBar.h"
#include "layout/TabbedComponent.h"
#include "layout/Viewport.h"
#include "menus/PopupMenu.h"
#include "menus/MenuBarModel.h"
#include "menus/MenuBarComponent.h"
#include "positioning/RelativeCoordinate.h"
#include "positioning/MarkerList.h"
#include "positioning/RelativePoint.h"
#include "positioning/RelativeRectangle.h"
#include "positioning/RelativeCoordinatePositioner.h"
#include "positioning/RelativeParallelogram.h"
#include "positioning/RelativePointPath.h"
#include "drawables/Drawable.h"
#include "drawables/DrawableShape.h"
#include "drawables/DrawableComposite.h"
#include "drawables/DrawableImage.h"
#include "drawables/DrawablePath.h"
#include "drawables/DrawableRectangle.h"
#include "drawables/DrawableText.h"
#include "widgets/TextEditor.h"
#include "widgets/Label.h"
#include "widgets/ComboBox.h"
#include "widgets/ImageComponent.h"
#include "widgets/ListBox.h"
#include "widgets/ProgressBar.h"
#include "widgets/Slider.h"
#include "widgets/TableHeaderComponent.h"
#include "widgets/TableListBox.h"
#include "widgets/Toolbar.h"
#include "widgets/ToolbarItemComponent.h"
#include "widgets/ToolbarItemFactory.h"
#include "widgets/ToolbarItemPalette.h"
#include "menus/BurgerMenuComponent.h"
#include "buttons/ToolbarButton.h"
#include "misc/DropShadower.h"
#include "misc/HSplashScreen.h"
#include "widgets/TreeView.h"
#include "windows/TopLevelWindow.h"
#include "windows/AlertWindow.h"
#include "windows/CallOutBox.h"
#include "windows/ComponentPeer.h"
#include "windows/ResizableWindow.h"
#include "windows/DocumentWindow.h"
#include "windows/DialogWindow.h"
#include "windows/NativeMessageBox.h"
#include "windows/ThreadWithProgressWindow.h"
#include "windows/TooltipWindow.h"
#include "layout/MultiDocumentPanel.h"
#include "layout/SidePanel.h"
#include "filebrowser/FileBrowserListener.h"
#include "filebrowser/DirectoryContentsList.h"
#include "filebrowser/DirectoryContentsDisplayComponent.h"
#include "filebrowser/FileBrowserComponent.h"
#include "filebrowser/FileChooser.h"
#include "filebrowser/FileChooserDialogBox.h"
#include "filebrowser/FileListComponent.h"
#include "filebrowser/FilenameComponent.h"
#include "filebrowser/FilePreviewComponent.h"
#include "filebrowser/FileSearchPathListComponent.h"
#include "filebrowser/FileTreeComponent.h"
#include "filebrowser/ImagePreviewComponent.h"
#include "filebrowser/ContentSharer.h"
#include "properties/PropertyComponent.h"
#include "properties/BooleanPropertyComponent.h"
#include "properties/ButtonPropertyComponent.h"
#include "properties/ChoicePropertyComponent.h"
#include "properties/PropertyPanel.h"
#include "properties/SliderPropertyComponent.h"
#include "properties/TextPropertyComponent.h"
#include "properties/MultiChoicePropertyComponent.h"
#include "application/Application.h"
#include "misc/BubbleComponent.h"
#include "lookandfeel/LookAndFeel.h"
#include "lookandfeel/LookAndFeel_V2.h"
#include "lookandfeel/LookAndFeel_V1.h"
#include "lookandfeel/LookAndFeel_V3.h"
#include "lookandfeel/LookAndFeel_V4.h"
#include "mouse/LassoComponent.h"

#if HLINUX
 #include "native/linux_X11.h"
#endif

#include "layout/FlexItem.h"
#include "layout/FlexBox.h"

#if HHAS_CONSTEXPR
 #include "layout/GridItem.h"
 #include "layout/Grid.h"
#endif
