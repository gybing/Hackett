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
struct ProjectTreeItemBase  : public JucerTreeViewBase,
                              public ValueTree::Listener
{
    ProjectTreeItemBase() {}

    void showSettingsPage (Component* content)
    {
        content->setComponentID (getUniqueName());

        std::unique_ptr<Component> comp (content);

        if (ProjectContentComponent* pcc = getProjectContentComponent())
            pcc->setEditorComponent (comp.release(), nullptr);
    }

    void closeSettingsPage()
    {
        if (auto* pcc = getProjectContentComponent())
            if (auto* content = pcc->getEditorComponentContent())
                if (content->getComponentID() == getUniqueName())
                    pcc->hideEditor();
    }

    void deleteAllSelectedItems() override
    {
        auto* tree = getOwnerView();
        HAssert (tree->getNumSelectedItems() <= 1); // multi-select should be disabled

        if (auto* s = dynamic_cast<ProjectTreeItemBase*> (tree->getSelectedItem (0)))
            s->deleteItem();
    }

    void itemOpennessChanged (bool isNowOpen) override
    {
        if (isNowOpen)
           refreshSubItems();
    }

    virtual bool isProjectSettings() const          { return false; }
    virtual bool isModulesList() const              { return false; }

    static void updateSize (Component& comp, PropertyGroupComponent& group)
    {
        auto width = jmax (550, comp.getParentWidth() - 12);

        auto y = 0;
        y += group.updateSize (12, y, width - 12);

        y = jmax (comp.getParentHeight(), y);

        comp.setSize (width, y);
    }
};
