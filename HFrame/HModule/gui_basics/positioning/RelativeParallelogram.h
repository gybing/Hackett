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
    A parallelogram defined by three RelativePoint positions.

    @see RelativePoint, RelativeCoordinate

    @tags{GUI}
*/
class API RelativeParallelogram
{
public:
    //==============================================================================
    RelativeParallelogram();
    RelativeParallelogram (const Rectangle<float>& simpleRectangle);
    RelativeParallelogram (const RelativePoint& topLeft, const RelativePoint& topRight, const RelativePoint& bottomLeft);
    RelativeParallelogram (const String& topLeft, const String& topRight, const String& bottomLeft);
    ~RelativeParallelogram();

    //==============================================================================
    void resolveThreePoints (Point<float>* points, Expression::Scope* scope) const;
    void resolveFourCorners (Point<float>* points, Expression::Scope* scope) const;
    const Rectangle<float> getBounds (Expression::Scope* scope) const;
    void getPath (Path& path, Expression::Scope* scope) const;
    AffineTransform resetToPerpendicular (Expression::Scope* scope);
    bool isDynamic() const;

    bool operator== (const RelativeParallelogram&) const noexcept;
    bool operator!= (const RelativeParallelogram&) const noexcept;

    static Point<float> getInternalCoordForPoint (const Point<float>* parallelogramCorners, Point<float> point) noexcept;
    static Point<float> getPointForInternalCoord (const Point<float>* parallelogramCorners, Point<float> internalPoint) noexcept;
    static Rectangle<float> getBoundingBox (const Point<float>* parallelogramCorners) noexcept;

    //==============================================================================
    RelativePoint topLeft, topRight, bottomLeft;
};


