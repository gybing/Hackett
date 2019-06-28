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
/** A simple implementation of the b2Draw class, used to draw a Box2D world.

    To use it, simply create an instance of this class in your paint() method,
    and call its render() method.

    @tags{Box2D}
*/
class Box2DRenderer   : public b2Draw

{
public:
    Box2DRenderer() noexcept;

    /** Renders the world.

        @param g        the context to render into
        @param world    the world to render
        @param box2DWorldLeft   the left coordinate of the area of the world to be drawn
        @param box2DWorldTop    the top coordinate of the area of the world to be drawn
        @param box2DWorldRight  the right coordinate of the area of the world to be drawn
        @param box2DWorldBottom the bottom coordinate of the area of the world to be drawn
        @param targetArea   the area within the target context onto which the source
                            world rectangle should be mapped
    */
    void render (Graphics& g,
                 b2World& world,
                 float box2DWorldLeft, float box2DWorldTop,
                 float box2DWorldRight, float box2DWorldBottom,
                 const Rectangle<float>& targetArea);

    // b2Draw methods:
    void DrawPolygon (const b2Vec2*, int32, const b2Color&) override;
    void DrawSolidPolygon (const b2Vec2*, int32, const b2Color&) override;
    void DrawCircle (const b2Vec2& center, float32 radius, const b2Color&) override;
    void DrawSolidCircle (const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color&) override;
    void DrawSegment (const b2Vec2& p1, const b2Vec2& p2, const b2Color&) override;
    void DrawTransform (const b2Transform& xf) override;

    /** Converts a b2Color to a H Colour. */
    virtual Colour getColour (const b2Color&) const;
    /** Returns the thickness to use for drawing outlines. */
    virtual float getLineThickness() const;

protected:
    Graphics* graphics;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Box2DRenderer)
};


