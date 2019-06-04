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

#include "JucerDocument.h"
#include "Components/ComponentUndoableAction.h"
#include "Properties/JustificationProperty.h"
#include "Properties/FontPropertyComponent.h"
#include "Properties/ComponentBooleanProperty.h"
#include "Properties/ComponentChoiceProperty.h"
#include "Properties/ComponentTextProperty.h"
#include "Properties/ComponentColourProperty.h"
#include "Properties/FilePropertyComponent.h"
#include "PaintElements/ImageResourceProperty.h"

#include "ObjectTypes.h"

#include "PaintElements/PaintElementUndoableAction.h"
#include "PaintElements/PaintElementRectangle.h"
#include "PaintElements/PaintElementRoundedRectangle.h"
#include "PaintElements/PaintElementImage.h"
#include "PaintElements/PaintElementEllipse.h"
#include "PaintElements/PaintElementPath.h"
#include "PaintElements/PaintElementText.h"
#include "PaintElements/PaintElementGroup.h"

#include "Components/ButtonHandler.h"
#include "Components/TextButtonHandler.h"
#include "Components/ToggleButtonHandler.h"
#include "Components/SliderHandler.h"
#include "Components/LabelHandler.h"
#include "Components/TextEditorHandler.h"
#include "Components/ComboBoxHandler.h"
#include "Components/GroupComponentHandler.h"
#include "Components/JucerComponentHandler.h"
#include "Components/HyperlinkButtonHandler.h"
#include "Components/ViewportHandler.h"
#include "Components/TabbedComponentHandler.h"
#include "Components/TreeViewHandler.h"
#include "Components/GenericComponentHandler.h"
#include "Components/ImageButtonHandler.h"

namespace ObjectTypes
{

static const char* const elementNames[] =
{
    "Rectangle",
    "Rounded Rectangle",
    "Ellipse",
    "Path",
    "Image",
    "Text",
    nullptr
};

const char* const* const elementTypeNames = (const char* const*) elementNames;
const int numElementTypes = (sizeof (elementNames) / sizeof (elementNames[0])) - 1;

PaintElement* createNewElement (const int index, PaintRoutine* owner)
{
    switch (index)
    {
        case 0:   return new PaintElementRectangle (owner);
        case 1:   return new PaintElementRoundedRectangle (owner);
        case 2:   return new PaintElementEllipse (owner);
        case 3:   return new PaintElementPath (owner);
        case 4:   return new PaintElementImage (owner);
        case 5:   return new PaintElementText (owner);
        default:  HAssertfalse; break;
    }

    return nullptr;
}

PaintElement* createNewImageElement (PaintRoutine* owner)
{
    return new PaintElementImage (owner);
}

PaintElement* createElementForXml (const XmlElement* const e, PaintRoutine* const owner)
{
    HAssert (e != nullptr);

    std::unique_ptr<PaintElement> pe;

    if (e->hasTagName (PaintElementRectangle::getTagName()))                pe.reset (new PaintElementRectangle (owner));
    else if (e->hasTagName (PaintElementRoundedRectangle::getTagName()))    pe.reset (new PaintElementRoundedRectangle (owner));
    else if (e->hasTagName (PaintElementEllipse::getTagName()))             pe.reset (new PaintElementEllipse (owner));
    else if (e->hasTagName (PaintElementImage::getTagName()))               pe.reset (new PaintElementImage (owner));
    else if (e->hasTagName (PaintElementPath::getTagName()))                pe.reset (new PaintElementPath (owner));
    else if (e->hasTagName (PaintElementText::getTagName()))                pe.reset (new PaintElementText (owner));
    else if (e->hasTagName (PaintElementGroup::getTagName()))               pe.reset (new PaintElementGroup (owner));

    if (pe != nullptr && pe->loadFromXml (*e))
        return pe.release();

    HAssertfalse;
    return nullptr;
}

//==============================================================================
static TextButtonHandler textButton;
static ToggleButtonHandler toggleButton;
static SliderHandler slider;
static LabelHandler label;
static TextEditorHandler textEditor;
static ComboBoxHandler comboBox;
static JucerComponentHandler HrCompHandler;
static GroupComponentHandler group;
static HyperlinkButtonHandler hyperlink;
static ViewportHandler viewport;
static TabbedComponentHandler tabbedComp;
static TreeViewHandler treeview;
static GenericComponentHandler genericHandler;
static ImageButtonHandler imageButtonHandler;

static ComponentTypeHandler* const compTypes[] =
{
    &textButton,
    &toggleButton,
    &slider,
    &label,
    &textEditor,
    &comboBox,
    &group,
    &HrCompHandler,
    &hyperlink,
    &viewport,
    &tabbedComp,
    &treeview,
    &genericHandler,
    &imageButtonHandler,
    nullptr
};

ComponentTypeHandler* const* const componentTypeHandlers = (ComponentTypeHandler* const*) compTypes;
const int numComponentTypes = numElementsInArray (compTypes) - 1;

}
