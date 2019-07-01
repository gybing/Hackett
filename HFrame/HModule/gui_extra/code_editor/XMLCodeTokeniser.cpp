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

XmlTokeniser::XmlTokeniser() {}
XmlTokeniser::~XmlTokeniser() {}

CodeEditorComponent::ColourScheme XmlTokeniser::getDefaultColourScheme()
{
    struct Type
    {
        const char* name;
        uint32 colour;
    };

    const Type types[] =
    {
        { "Error",              0xffcc0000 },
        { "Comment",            0xff00aa00 },
        { "Keyword",            0xff0000cc },
        { "Operator",           0xff225500 },
        { "Identifier",         0xff000000 },
        { "String",             0xff990099 },
        { "Bracket",            0xff000055 },
        { "Punctuation",        0xff004400 },
        { "Preprocessor Text",  0xff660000 }
    };

    CodeEditorComponent::ColourScheme cs;

    for (auto& t : types)
        cs.set (t.name, Colour (t.colour));

    return cs;
}

template <typename Iterator>
static void skipToEndOfXmlDTD (Iterator& source) noexcept
{
    bool lastWasQuestionMark = false;

    for (;;)
    {
        auto c = source.nextChar();

        if (c == 0 || (c == '>' && lastWasQuestionMark))
            break;

        lastWasQuestionMark = (c == '?');
    }
}

template <typename Iterator>
static void skipToEndOfXmlComment (Iterator& source) noexcept
{
    char last[2] = {};

    for (;;)
    {
        auto c = source.nextChar();

        if (c == 0 || (c == '>' && last[0] == '-' && last[1] == '-'))
            break;

        last[1] = last[0];
        last[0] = c;
    }
}

int XmlTokeniser::readNextToken (CodeDocument::Iterator& source)
{
    source.skipWhitespace();
    auto firstChar = source.peekNextChar();

    switch (firstChar)
    {
        case 0:  break;

        case '"':
        case '\'':
            CppTokeniserFunctions::skipQuotedString (source);
            return tokenType_string;

        case '<':
        {
            source.skip();
            source.skipWhitespace();
            auto nextChar = source.peekNextChar();

            if (nextChar == '?')
            {
                source.skip();
                skipToEndOfXmlDTD (source);
                return tokenType_preprocessor;
            }

            if (nextChar == '!')
            {
                source.skip();

                if (source.peekNextChar() == '-')
                {
                    source.skip();

                    if (source.peekNextChar() == '-')
                    {
                        skipToEndOfXmlComment (source);
                        return tokenType_comment;
                    }
                }
            }

            CppTokeniserFunctions::skipIfNextCharMatches (source, '/');
            CppTokeniserFunctions::parseIdentifier (source);
            source.skipWhitespace();
            CppTokeniserFunctions::skipIfNextCharMatches (source, '/');
            source.skipWhitespace();
            CppTokeniserFunctions::skipIfNextCharMatches (source, '>');
            return tokenType_keyword;
        }

        case '>':
            source.skip();
            return tokenType_keyword;

        case '/':
            source.skip();
            source.skipWhitespace();
            CppTokeniserFunctions::skipIfNextCharMatches (source, '>');
            return tokenType_keyword;

        case '=':
        case ':':
            source.skip();
            return tokenType_operator;

        default:
            if (CppTokeniserFunctions::isIdentifierStart (firstChar))
                CppTokeniserFunctions::parseIdentifier (source);

            source.skip();
            break;
    };

    return tokenType_identifier;
}


