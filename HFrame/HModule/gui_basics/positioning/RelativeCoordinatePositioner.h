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

//==============================================================================
/**
    Base class for Component::Positioners that are based upon relative coordinates.

    @tags{GUI}
*/
class API RelativeCoordinatePositionerBase  : public Component::Positioner,
                                                    public ComponentListener,
                                                    public MarkerList::Listener
{
public:
    RelativeCoordinatePositionerBase (Component&);
    ~RelativeCoordinatePositionerBase() override;

    void componentMovedOrResized (Component&, bool, bool) override;
    void componentParentHierarchyChanged (Component&) override;
    void componentChildrenChanged (Component&) override;
    void componentBeingDeleted (Component&) override;
    void markersChanged (MarkerList*) override;
    void markerListBeingDeleted (MarkerList*) override;

    void apply();

    bool addCoordinate (const RelativeCoordinate&);
    bool addPoint (const RelativePoint&);

    //==============================================================================
    /** Used for resolving a RelativeCoordinate expression in the context of a component. */
    class ComponentScope  : public Expression::Scope
    {
    public:
        ComponentScope (Component&);

        // Suppress a VS2013 compiler warning
        ComponentScope& operator= (const ComponentScope&) = delete;

        Expression getSymbolValue (const String& symbol) const override;
        void visitRelativeScope (const String& scopeName, Visitor&) const override;
        String getScopeUID() const override;

    protected:
        Component& component;

        Component* findSiblingComponent (const String& componentID) const;
    };

protected:
    virtual bool registerCoordinates() = 0;
    virtual void applyToComponentBounds() = 0;

private:
    class DependencyFinderScope;
    friend class DependencyFinderScope;
    Array<Component*> sourceComponents;
    Array<MarkerList*> sourceMarkerLists;
    bool registeredOk;

    void registerComponentListener (Component&);
    void registerMarkerListListener (MarkerList*);
    void unregisterListeners();

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RelativeCoordinatePositionerBase)
};


