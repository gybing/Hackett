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
    A button that uses the standard lozenge-shaped background with a line of
    text on it.

    @see Button, DrawableButton

    @tags{GUI}
*/
class API TextButton  : public Button
{
public:
    //==============================================================================
    /** Creates a TextButton. */
    TextButton();

    /** Creates a TextButton.
        @param buttonName           the text to put in the button (the component's name is also
                                    initially set to this string, but these can be changed later
                                    using the setName() and setButtonText() methods)
    */
    explicit TextButton (const String& buttonName);

    /** Creates a TextButton.
        @param buttonName           the text to put in the button (the component's name is also
                                    initially set to this string, but these can be changed later
                                    using the setName() and setButtonText() methods)
        @param toolTip              an optional string to use as a toolip
    */
    TextButton (const String& buttonName, const String& toolTip);

    /** Destructor. */
    ~TextButton() override;

    //==============================================================================
    /** A set of colour IDs to use to change the colour of various aspects of the button.

        These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
        methods.

        @see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
    */
    enum ColourIds
    {
        buttonColourId                  = 0x1000100,  /**< The colour used to fill the button shape (when the button is toggled
                                                           'off'). The look-and-feel class might re-interpret this to add
                                                           effects, etc. */
        buttonOnColourId                = 0x1000101,  /**< The colour used to fill the button shape (when the button is toggled
                                                           'on'). The look-and-feel class might re-interpret this to add
                                                           effects, etc. */
        textColourOffId                 = 0x1000102,  /**< The colour to use for the button's text when the button's toggle state is "off". */
        textColourOnId                  = 0x1000103   /**< The colour to use for the button's text.when the button's toggle state is "on". */
    };

    //==============================================================================
    /** Changes this button's width to fit neatly around its current text, without
        changing its height.
    */
    void changeWidthToFitText();

    /** Resizes the button's width to fit neatly around its current text, and gives it
        the specified height.
    */
    void changeWidthToFitText (int newHeight);

    /** Returns the width that the LookAndFeel suggests would be best for this button if it
        had the given height.
    */
    int getBestWidthForHeight (int buttonHeight);

    //==============================================================================
    /** @internal */
    void paintButton (Graphics&, bool, bool) override;
    /** @internal */
    void colourChanged() override;

private:
   #if HCATCH_DEPRECATED_CODE_MISUSE
    // Note that this method has been removed - instead, see LookAndFeel::getTextButtonFont()
    virtual int getFont() { return 0; }
   #endif

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextButton)
};


