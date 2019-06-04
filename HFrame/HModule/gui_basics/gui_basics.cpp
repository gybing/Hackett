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

#ifdef HGUI_BASICS_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of H cpp file"
#endif

#define NS_FORMAT_FUNCTION(F,A) // To avoid spurious warnings from GCC

#define HCORE_INCLUDE_OBJC_HELPERS 1
#define HCORE_INCLUDE_COM_SMART_PTR 1
#define HCORE_INCLUDE_JNI_HELPERS 1
#define HCORE_INCLUDE_NATIVE_HEADERS 1
#define HEVENTS_INCLUDE_WIN32_MESSAGE_WINDOW 1
#define HGRAPHICS_INCLUDE_COREGRAPHICS_HELPERS 1

#include "gui_basics.h"

//==============================================================================
#if HMAC
 #import <WebKit/WebKit.h>
 #import <IOKit/pwr_mgt/IOPMLib.h>

 #if HSUPPORT_CARBON
  #import <Carbon/Carbon.h> // still needed for SetSystemUIMode()
 #endif

#elif HIOS
 #if HPUSH_NOTIFICATIONS && defined (__IPHONE_10_0) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
  #import <UserNotifications/UserNotifications.h>
 #endif

 #import <UIKit/UIActivityViewController.h>

//==============================================================================
#elif HWINDOWS
 #include <windowsx.h>
 #include <vfw.h>
 #include <commdlg.h>

 #if HWEB_BROWSER
  #include <exdisp.h>
  #include <exdispid.h>
 #endif

 #if HMSVC && ! HDONT_AUTOLINK_TO_WIN32_LIBRARIES
  #pragma comment(lib, "vfw32.lib")
  #pragma comment(lib, "imm32.lib")
 #endif

 #if HOPENGL
  #if HMSVC && ! HDONT_AUTOLINK_TO_WIN32_LIBRARIES
   #pragma comment(lib, "OpenGL32.Lib")
   #pragma comment(lib, "GlU32.Lib")
  #endif
 #endif

 #if HDIRECT2D && HMSVC && ! HDONT_AUTOLINK_TO_WIN32_LIBRARIES
  #pragma comment (lib, "Dwrite.lib")
  #pragma comment (lib, "D2d1.lib")
 #endif

 #if HMINGW
  #include <imm.h>
 #endif

//==============================================================================
#elif HLINUX
 #include <X11/Xlib.h>
 #include <X11/Xatom.h>
 #include <X11/Xresource.h>
 #include <X11/Xutil.h>
 #include <X11/Xmd.h>
 #include <X11/keysym.h>
 #include <X11/XKBlib.h>
 #include <X11/cursorfont.h>
 #include <unistd.h>

 #if HUSE_XRANDR
  /* If you're trying to use Xrandr, you'll need to install the "libxrandr-dev" package..  */
  #include <X11/extensions/Xrandr.h>
 #endif

 #if HUSE_XINERAMA
  /* If you're trying to use Xinerama, you'll need to install the "libxinerama-dev" package..  */
  #include <X11/extensions/Xinerama.h>
 #endif

 #if HUSE_XSHM
  #include <X11/extensions/XShm.h>
  #include <sys/shm.h>
  #include <sys/ipc.h>
 #endif

 #if HUSE_XRENDER
  // If you're missing these headers, try installing the libxrender-dev and libxcomposite-dev
  #include <X11/extensions/Xrender.h>
  #include <X11/extensions/Xcomposite.h>
 #endif

 #if HUSE_XCURSOR
  // If you're missing this header, try installing the libxcursor-dev package
  #include <X11/Xcursor/Xcursor.h>
 #endif

 #undef SIZEOF
 #undef KeyPress
#endif

#include <set>

//==============================================================================
#define HASSERT_MESSAGE_MANAGER_IS_LOCKED_OR_OFFSCREEN \
    HAssert ((MessageManager::getInstanceWithoutCreating() != nullptr \
               && MessageManager::getInstanceWithoutCreating()->currentThreadHasLockedMessageManager()) \
              || getPeer() == nullptr);

namespace H
{
    extern bool areThereAnyAlwaysOnTopWindows();
}

#include "components/Component.cpp"
#include "components/ComponentListener.cpp"
#include "mouse/MouseInputSource.cpp"
#include "desktop/Displays.cpp"
#include "desktop/Desktop.cpp"
#include "components/ModalComponentManager.cpp"
#include "mouse/ComponentDragger.cpp"
#include "mouse/DragAndDropContainer.cpp"
#include "mouse/MouseCursor.cpp"
#include "mouse/MouseEvent.cpp"
#include "mouse/MouseInactivityDetector.cpp"
#include "mouse/MouseListener.cpp"
#include "keyboard/CaretComponent.cpp"
#include "keyboard/KeyboardFocusTraverser.cpp"
#include "keyboard/KeyListener.cpp"
#include "keyboard/KeyPress.cpp"
#include "keyboard/ModifierKeys.cpp"
#include "buttons/ArrowButton.cpp"
#include "buttons/Button.cpp"
#include "buttons/DrawableButton.cpp"
#include "buttons/HyperlinkButton.cpp"
#include "buttons/ImageButton.cpp"
#include "buttons/ShapeButton.cpp"
#include "buttons/TextButton.cpp"
#include "buttons/ToggleButton.cpp"
#include "buttons/ToolbarButton.cpp"
#include "drawables/Drawable.cpp"
#include "drawables/DrawableComposite.cpp"
#include "drawables/DrawableImage.cpp"
#include "drawables/DrawablePath.cpp"
#include "drawables/DrawableRectangle.cpp"
#include "drawables/DrawableShape.cpp"
#include "drawables/DrawableText.cpp"
#include "drawables/SVGParser.cpp"
#include "filebrowser/DirectoryContentsDisplayComponent.cpp"
#include "filebrowser/DirectoryContentsList.cpp"
#include "filebrowser/FileBrowserComponent.cpp"
#include "filebrowser/FileChooser.cpp"
#include "filebrowser/FileChooserDialogBox.cpp"
#include "filebrowser/FileListComponent.cpp"
#include "filebrowser/FilenameComponent.cpp"
#include "filebrowser/FileSearchPathListComponent.cpp"
#include "filebrowser/FileTreeComponent.cpp"
#include "filebrowser/ImagePreviewComponent.cpp"
#include "filebrowser/ContentSharer.cpp"
#include "layout/ComponentAnimator.cpp"
#include "layout/ComponentBoundsConstrainer.cpp"
#include "layout/ComponentBuilder.cpp"
#include "layout/ComponentMovementWatcher.cpp"
#include "layout/ConcertinaPanel.cpp"
#include "layout/GroupComponent.cpp"
#include "layout/MultiDocumentPanel.cpp"
#include "layout/ResizableBorderComponent.cpp"
#include "layout/ResizableCornerComponent.cpp"
#include "layout/ResizableEdgeComponent.cpp"
#include "layout/ScrollBar.cpp"
#include "layout/SidePanel.cpp"
#include "layout/StretchableLayoutManager.cpp"
#include "layout/StretchableLayoutResizerBar.cpp"
#include "layout/StretchableObjectResizer.cpp"
#include "layout/TabbedButtonBar.cpp"
#include "layout/TabbedComponent.cpp"
#include "layout/Viewport.cpp"
#include "lookandfeel/LookAndFeel.cpp"
#include "lookandfeel/LookAndFeel_V2.cpp"
#include "lookandfeel/LookAndFeel_V1.cpp"
#include "lookandfeel/LookAndFeel_V3.cpp"
#include "lookandfeel/LookAndFeel_V4.cpp"
#include "menus/MenuBarComponent.cpp"
#include "menus/BurgerMenuComponent.cpp"
#include "menus/MenuBarModel.cpp"
#include "menus/PopupMenu.cpp"
#include "positioning/MarkerList.cpp"
#include "positioning/RelativeCoordinate.cpp"
#include "positioning/RelativeCoordinatePositioner.cpp"
#include "positioning/RelativeParallelogram.cpp"
#include "positioning/RelativePoint.cpp"
#include "positioning/RelativePointPath.cpp"
#include "positioning/RelativeRectangle.cpp"
#include "properties/BooleanPropertyComponent.cpp"
#include "properties/ButtonPropertyComponent.cpp"
#include "properties/ChoicePropertyComponent.cpp"
#include "properties/PropertyComponent.cpp"
#include "properties/PropertyPanel.cpp"
#include "properties/SliderPropertyComponent.cpp"
#include "properties/TextPropertyComponent.cpp"
#include "properties/MultiChoicePropertyComponent.cpp"
#include "widgets/ComboBox.cpp"
#include "widgets/ImageComponent.cpp"
#include "widgets/Label.cpp"
#include "widgets/ListBox.cpp"
#include "widgets/ProgressBar.cpp"
#include "widgets/Slider.cpp"
#include "widgets/TableHeaderComponent.cpp"
#include "widgets/TableListBox.cpp"
#include "widgets/TextEditor.cpp"
#include "widgets/ToolbarItemComponent.cpp"
#include "widgets/Toolbar.cpp"
#include "widgets/ToolbarItemPalette.cpp"
#include "widgets/TreeView.cpp"
#include "windows/AlertWindow.cpp"
#include "windows/CallOutBox.cpp"
#include "windows/ComponentPeer.cpp"
#include "windows/DialogWindow.cpp"
#include "windows/DocumentWindow.cpp"
#include "windows/ResizableWindow.cpp"
#include "windows/ThreadWithProgressWindow.cpp"
#include "windows/TooltipWindow.cpp"
#include "windows/TopLevelWindow.cpp"
#include "commands/ApplicationCommandInfo.cpp"
#include "commands/ApplicationCommandManager.cpp"
#include "commands/ApplicationCommandTarget.cpp"
#include "commands/KeyPressMappingSet.cpp"
#include "application/Application.cpp"
#include "misc/BubbleComponent.cpp"
#include "misc/DropShadower.cpp"
#include "misc/HSplashScreen.cpp"

#include "layout/FlexBox.cpp"
#if HHAS_CONSTEXPR
 #include "layout/GridItem.cpp"
 #include "layout/Grid.cpp"
#endif

#if HIOS || HWINDOWS
 #include "native/MultiTouchMapper.h"
#endif

#if HMAC || HIOS

 #if HCLANG
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wundeclared-selector"
 #endif

 #if HIOS
  #include "native/ios_UIViewComponentPeer.mm"
  #include "native/ios_Windowing.mm"
  #include "native/ios_FileChooser.mm"
  #include "native/ios_ContentSharer.cpp"
 #else
  #include "native/mac_NSViewComponentPeer.mm"
  #include "native/mac_Windowing.mm"
  #include "native/mac_MainMenu.mm"
  #include "native/mac_FileChooser.mm"
 #endif

 #if HCLANG
  #pragma clang diagnostic pop
 #endif

 #include "native/mac_MouseCursor.mm"

#elif HWINDOWS
 #include "native/win32_Windowing.cpp"
 #include "native/win32_DragAndDrop.cpp"
 #include "native/win32_FileChooser.cpp"

#elif HLINUX
 #include "native/linux_X11.cpp"
 #include "native/linux_X11_Clipboard.cpp"
 #include "native/linux_X11_Windowing.cpp"
 #include "native/linux_FileChooser.cpp"

#elif HANDROID
 #include "native/android_Windowing.cpp"
 #include "native/common_MimeTypes.cpp"
 #include "native/android_FileChooser.cpp"
 #include "native/android_ContentSharer.cpp"

#endif
