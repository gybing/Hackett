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
    A component that draws an outline around itself and has an optional title at
    the top, for drawing an outline around a group of controls.


    @tags{GUI}
*/
class API GroupComponent    : public Component
{
public:
    //==============================================================================
    /** Creates a GroupComponent.

        @param componentName    the name to give the component
        @param labelText        the text to show at the top of the outline
    */
    GroupComponent (const String& componentName = String(),
                    const String& labelText = String());

    /** Destructor. */
    ~GroupComponent() override;

    //==============================================================================
    /** Changes the text that's shown at the top of the component. */
    void setText (const String& newText);

    /** Returns the currently displayed text label. */
    String getText() const;

    /** Sets the positioning of the text label.

        (The default is Justification::left)

        @see getTextLabelPosition
    */
    void setTextLabelPosition (Justification justification);

    /** Returns the current text label position.

        @see setTextLabelPosition
    */
    Justification getTextLabelPosition() const noexcept           { return justification; }

    //==============================================================================
    /** A set of colour IDs to use to change the colour of various aspects of the component.

        These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
        methods.

        @see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
    */
    enum ColourIds
    {
        outlineColourId     = 0x1005400,    /**< The colour to use for drawing the line around the edge. */
        textColourId        = 0x1005410     /**< The colour to use to draw the text label. */
    };

    //==============================================================================
    /** This abstract base class is implemented by LookAndFeel classes. */
    struct API  LookAndFeelMethods
    {
        virtual ~LookAndFeelMethods() = default;

        virtual void drawGroupComponentOutline (Graphics&, int w, int h, const String& text,
                                                const Justification&, GroupComponent&) = 0;
    };

    //==============================================================================
    /** @internal */
    void paint (Graphics&) override;
    /** @internal */
    void enablementChanged() override;
    /** @internal */
    void colourChanged() override;

private:
    String text;
    Justification justification;

    HDECLARE_NON_COPYABLE (GroupComponent)
};


