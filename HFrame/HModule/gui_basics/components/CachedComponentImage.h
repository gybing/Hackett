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
    Base class used internally for structures that can store cached images of
    component state.

    Most people are unlikely to ever need to know about this class - it's really
    only for power-users!

    @see Component::setCachedComponentImage

    @tags{GUI}
*/
class API CachedComponentImage
{
public:
    CachedComponentImage() = default;
    virtual ~CachedComponentImage() = default;

    //==============================================================================
    /** Called as part of the parent component's paint method, this must draw
        the given component into the target graphics context, using the cached
        version where possible.
    */
    virtual void paint (Graphics&) = 0;

    /** Invalidates all cached image data.
        @returns true if the peer should also be repainted, or false if this object
                 handles all repaint work internally.
    */
    virtual bool invalidateAll() = 0;

    /** Invalidates a section of the cached image data.
        @returns true if the peer should also be repainted, or false if this object
                 handles all repaint work internally.
    */
    virtual bool invalidate (const Rectangle<int>& area) = 0;

    /** Called to indicate that the component is no longer active, so
        any cached data should be released if possible.
    */
    virtual void releaseResources() = 0;
};


