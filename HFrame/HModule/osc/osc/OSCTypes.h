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
/** The type used for OSC type tags. */
using OSCType = char;


/** The type used for OSC type tag strings. */
using OSCTypeList = Array<OSCType>;

//==============================================================================
/** The definitions of supported OSC types and their associated OSC type tags,
    as defined in the OpenSoundControl 1.0 specification.

    Note: This implementation does not support any additional type tags that
    are not part of the specification.

    @tags{OSC}
*/
class API OSCTypes
{
public:
    static const OSCType int32;
    static const OSCType float32;
    static const OSCType string;
    static const OSCType blob;
    static const OSCType colour;

    static bool isSupportedType (OSCType type) noexcept
    {
        return type == OSCTypes::int32
            || type == OSCTypes::float32
            || type == OSCTypes::string
            || type == OSCTypes::blob
            || type == OSCTypes::colour;
    }
};


//==============================================================================
/**
    Holds a 32-bit RGBA colour for passing to and from an OSCArgument.
    @see OSCArgument, OSCTypes::colour
    @tags{OSC}
*/
struct OSCColour
{
    uint8 red, green, blue, alpha;

    uint32 toInt32() const;
    static OSCColour fromInt32 (uint32);
};


//==============================================================================
/** Base class for exceptions that can be thrown by methods in the OSC module.

    @tags{OSC}
*/
struct OSCException  : public std::exception
{
    OSCException (const String& desc)
        : description (desc)
    {
       #if ! HUNIT_TESTS
        DBG ("OSCFormatError: " + description);
       #endif
    }

    String description;
};

//==============================================================================
/** Exception type thrown when the OSC module fails to parse something because
    of a data format not compatible with the OpenSoundControl 1.0 specification.

    @tags{OSC}
*/
struct OSCFormatError : public OSCException
{
    OSCFormatError (const String& desc) : OSCException (desc) {}
};

//==============================================================================
/** Exception type thrown in cases of unexpected errors in the OSC module.

    Note: This should never happen, and all the places where this is thrown
    should have a preceding HAssertfalse to facilitate debugging.

    @tags{OSC}
*/
struct OSCInternalError : public OSCException
{
    OSCInternalError (const String& desc) : OSCException (desc) {}
};


