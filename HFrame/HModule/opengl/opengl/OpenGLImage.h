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
    A type of ImagePixelData that stores its image data in an OpenGL
    framebuffer, allowing a H Image object to wrap a framebuffer.

    By creating an Image from an instance of an OpenGLFrameBufferImage,
    you can then use a Graphics object to draw into the framebuffer using normal
    H 2D operations.

    @see Image, ImageType, ImagePixelData, OpenGLFrameBuffer

    @tags{OpenGL}
*/
class API OpenGLImageType     : public ImageType
{
public:
    OpenGLImageType();
    ~OpenGLImageType() override;

    ImagePixelData::Ptr create (Image::PixelFormat, int width, int height, bool shouldClearImage) const override;
    int getTypeID() const override;

    static OpenGLFrameBuffer* getFrameBufferFrom (const Image&);
};


