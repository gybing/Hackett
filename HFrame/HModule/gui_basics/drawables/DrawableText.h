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
    A drawable object which renders a line of text.

    @see Drawable

    @tags{GUI}
*/
class API DrawableText  : public Drawable
{
public:
    //==============================================================================
    /** Creates a DrawableText object. */
    DrawableText();
    DrawableText (const DrawableText&);

    /** Destructor. */
    ~DrawableText() override;

    //==============================================================================
    /** Sets the text to display.*/
    void setText (const String& newText);

    /** Returns the currently displayed text */
    const String& getText() const noexcept                              { return text;}

    /** Sets the colour of the text. */
    void setColour (Colour newColour);

    /** Returns the current text colour. */
    Colour getColour() const noexcept                                   { return colour; }

    /** Sets the font to use.
        Note that the font height and horizontal scale are set using setFontHeight() and
        setFontHorizontalScale(). If applySizeAndScale is true, then these height
        and scale values will be changed to match the dimensions of the font supplied;
        if it is false, then the new font object's height and scale are ignored.
    */
    void setFont (const Font& newFont, bool applySizeAndScale);

    /** Returns the current font. */
    const Font& getFont() const noexcept                                { return font; }

    /** Changes the justification of the text within the bounding box. */
    void setJustification (Justification newJustification);

    /** Returns the current justification. */
    Justification getJustification() const noexcept                     { return justification; }

    /** Returns the parallelogram that defines the text bounding box. */
    Parallelogram<float> getBoundingBox() const noexcept                { return bounds; }

    /** Sets the bounding box that contains the text. */
    void setBoundingBox (Parallelogram<float> newBounds);

    float getFontHeight() const noexcept                                { return fontHeight; }
    void setFontHeight (float newHeight);

    float getFontHorizontalScale() const noexcept                       { return fontHScale; }
    void setFontHorizontalScale (float newScale);

    //==============================================================================
    /** @internal */
    void paint (Graphics&) override;
    /** @internal */
    std::unique_ptr<Drawable> createCopy() const override;
    /** @internal */
    Rectangle<float> getDrawableBounds() const override;
    /** @internal */
    Path getOutlineAsPath() const override;

private:
    //==============================================================================
    Parallelogram<float> bounds;
    float fontHeight, fontHScale;
    Font font, scaledFont;
    String text;
    Colour colour;
    Justification justification;

    void refreshBounds();
    Rectangle<int> getTextArea (float width, float height) const;
    AffineTransform getTextTransform (float width, float height) const;

    DrawableText& operator= (const DrawableText&);
    HLEAK_DETECTOR (DrawableText)
};


