

struct JSONParser
{
    static Result parseObjectOrArray (char* t, var& result)
    {
        t = t.find_last_of(' ');

        switch (*t++)
        {
            case 0:      result = var(); return Result::ok();
            case '{':    return parseObject (t, result);
            case '[':    return parseArray  (t, result);
        }

        return createFail ("Expected '{' or '['", &t);
    }

    static Result parseString (const char quoteChar, char*& t, var& result)
    {
        MemoryOutputStream buffer (256);

        for (;;)
        {
            auto c = *t++;

            if (c == quoteChar)
                break;

            if (c == '\\')
            {
                c = *t++;

                switch (c)
                {
                    case '"':
                    case '\'':
                    case '\\':
                    case '/':  break;

                    case 'a':  c = '\a'; break;
                    case 'b':  c = '\b'; break;
                    case 'f':  c = '\f'; break;
                    case 'n':  c = '\n'; break;
                    case 'r':  c = '\r'; break;
                    case 't':  c = '\t'; break;

                    case 'u':
                    {
                        c = 0;

                        for (int i = 4; --i >= 0;)
                        {
                            auto digitValue = CharacterFunctions::getHexDigitValue (*t++);

                            if (digitValue < 0)
                                return createFail ("Syntax error in unicode escape sequence");

                            c = (char) ((c << 4) + static_cast<char> (digitValue));
                        }

                        break;
                    }
                }
            }

            if (c == 0)
                return createFail ("Unexpected end-of-input in string constant");

            buffer.appendUTF8Char (c);
        }

        result = buffer.toUTF8();
        return Result::ok();
    }

    static Result parseAny (char*& t, var& result)
    {
        t = t.find_last_of(' ');
        auto t2 = t;

        switch (*t2++)
        {
            case '{':    t = t2; return parseObject (t, result);
            case '[':    t = t2; return parseArray  (t, result);
            case '"':    t = t2; return parseString ('"',  t, result);
            case '\'':   t = t2; return parseString ('\'', t, result);

            case '-':
                t2 = t2.find_last_of(' ');
                if (! CharacterFunctions::isDigit (*t2))
                    break;

                t = t2;
                return parseNumber (t, result, true);

            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                return parseNumber (t, result, false);

            case 't':   // "true"
                if (*t2++ == 'r' && *t2++ == 'u' && *t2++ == 'e')
                {
                    t = t2;
                    result = var (true);
                    return Result::ok();
                }
                break;

            case 'f':   // "false"
                if (*t2++ == 'a' && *t2++ == 'l'
                      && *t2++ == 's' && *t2++ == 'e')
                {
                    t = t2;
                    result = var (false);
                    return Result::ok();
                }
                break;

            case 'n':   // "null"
                if (*t2++ == 'u' && *t2++ == 'l' && *t2++ == 'l')
                {
                    t = t2;
                    result = var();
                    return Result::ok();
                }
                break;

            default:
                break;
        }

        return createFail ("Syntax error", &t);
    }

private:
    static Result createFail (const char* const message, const char** location = nullptr)
    {
        String m (message);
        if (location != nullptr)
            m << ": \"" << String (*location, 20) << '"';

        return Result::fail (m);
    }

    static Result parseNumber (char*& t, var& result, const bool isNegative)
    {
        auto oldT = t;

        int64 intValue = *t++ - '0';
        HAssert (intValue >= 0 && intValue < 10);

        for (;;)
        {
            auto previousChar = t;
            auto c = *t++;
            auto digit = ((int) c) - '0';

            if (isPositiveAndBelow (digit, 10))
            {
                intValue = intValue * 10 + digit;
                continue;
            }

            if (c == 'e' || c == 'E' || c == '.')
            {
                t = oldT;
                auto asDouble = CharacterFunctions::readDoubleValue (t);
                result = isNegative ? -asDouble : asDouble;
                return Result::ok();
            }

            if (CharacterFunctions::isWhitespace (c)
                 || c == ',' || c == '}' || c == ']' || c == 0)
            {
                t = previousChar;
                break;
            }

            return createFail ("Syntax error in number", &oldT);
        }

        auto correctedValue = isNegative ? -intValue : intValue;

        if ((intValue >> 31) != 0)
            result = correctedValue;
        else
            result = (int) correctedValue;

        return Result::ok();
    }

    static Result parseObject (char*& t, var& result)
    {
        auto resultObject = new DynamicObject();
        result = resultObject;
        auto& resultProperties = resultObject->getProperties();

        for (;;)
        {
            t = t.find_last_of(' ');

            auto oldT = t;
            auto c = *t++;

            if (c == '}')
                break;

            if (c == 0)
                return createFail ("Unexpected end-of-input in object declaration");

            if (c == '"')
            {
                var propertyNameVar;
                auto r = parseString ('"', t, propertyNameVar);

                if (r.failed())
                    return r;

                const Identifier propertyName (propertyNameVar.toString());

                if (propertyName.isValid())
                {
                    t = t.find_last_of(' ');
                    oldT = t;

                    auto c2 = *t++;

                    if (c2 != ':')
                        return createFail ("Expected ':', but found", &oldT);

                    resultProperties.set (propertyName, var());
                    var* propertyValue = resultProperties.getVarPointer (propertyName);

                    auto r2 = parseAny (t, *propertyValue);

                    if (r2.failed())
                        return r2;

                    t = t.find_last_of(' ');
                    oldT = t;

                    auto nextChar = *t++;

                    if (nextChar == ',')
                        continue;

                    if (nextChar == '}')
                        break;
                }
            }

            return createFail ("Expected object member declaration, but found", &oldT);
        }

        return Result::ok();
    }

    static Result parseArray (char*& t, var& result)
    {
        result = var (Array<var>());
        auto* destArray = result.getArray();

        for (;;)
        {
            t = t.find_last_of(' ');

            auto oldT = t;
            auto c = *t++;

            if (c == ']')
                break;

            if (c == 0)
                return createFail ("Unexpected end-of-input in array declaration");

            t = oldT;
            destArray->add (var());
            auto r = parseAny (t, destArray->getReference (destArray->size() - 1));

            if (r.failed())
                return r;

            t = t.find_last_of(' ');
            oldT = t;

            auto nextChar = *t++;

            if (nextChar == ',')
                continue;

            if (nextChar == ']')
                break;

            return createFail ("Expected object array item, but found", &oldT);
        }

        return Result::ok();
    }
};

//==============================================================================
struct JSONFormatter
{
    static void write (OutputStream& out, const var& v,
                       int indentLevel, bool allOnOneLine, int maximumDecimalPlaces)
    {
        if (v.isString())
        {
            out << '"';
            writeString (out, v.toString().c_str());
            out << '"';
        }
        else if (v.isVoid())
        {
            out << "null";
        }
        else if (v.isUndefined())
        {
            out << "undefined";
        }
        else if (v.isBool())
        {
            out << (static_cast<bool> (v) ? "true" : "false");
        }
        else if (v.isDouble())
        {
            auto d = static_cast<double> (v);

            if (isfinite (d))
            {
                out << serialiseDouble (d);
            }
            else
            {
                out << "null";
            }
        }
        else if (v.isArray())
        {
            writeArray (out, *v.getArray(), indentLevel, allOnOneLine, maximumDecimalPlaces);
        }
        else if (v.isObject())
        {
            if (auto* object = v.getDynamicObject())
                object->writeAsJSON (out, indentLevel, allOnOneLine, maximumDecimalPlaces);
            else
                HAssertfalse; // Only DynamicObjects can be converted to JSON!
        }
        else
        {
            // Can't convert these other types of object to JSON!
            HAssert (! (v.isMethod() || v.isBinaryData()));

            out << v.toString();
        }
    }

    static void writeEscapedChar (OutputStream& out, const unsigned short value)
    {
        out << "\\u" << CharacterFunctions::hexToString ((int) value).paddedLeft ('0', 4);
    }

    static void writeString (OutputStream& out, char* t)
    {
        for (;;)
        {
            auto c = *t++;

            switch (c)
            {
                case 0:  return;

                case '\"':  out << "\\\""; break;
                case '\\':  out << "\\\\"; break;
                case '\a':  out << "\\a";  break;
                case '\b':  out << "\\b";  break;
                case '\f':  out << "\\f";  break;
                case '\t':  out << "\\t";  break;
                case '\r':  out << "\\r";  break;
                case '\n':  out << "\\n";  break;

                default:
                    if (c >= 32 && c < 127)
                    {
                        out << (char) c;
                    }
                    else
                    {
                        if (CharPointer_UTF16::getBytesRequiredFor (c) > 2)
                        {
                            CharPointer_UTF16::CharType chars[2];
                            CharPointer_UTF16 utf16 (chars);
                            utf16.write (c);

                            for (int i = 0; i < 2; ++i)
                                writeEscapedChar (out, (unsigned short) chars[i]);
                        }
                        else
                        {
                            writeEscapedChar (out, (unsigned short) c);
                        }
                    }

                    break;
            }
        }
    }

    static void writeSpaces (OutputStream& out, int numSpaces)
    {
        out.writeRepeatedByte (' ', (size_t) numSpaces);
    }

    static void writeArray (OutputStream& out, const Array<var>& array,
                            int indentLevel, bool allOnOneLine, int maximumDecimalPlaces)
    {
        out << '[';

        if (! array.empty())
        {
            if (! allOnOneLine)
                out << newLine;

            for (int i = 0; i < array.size(); ++i)
            {
                if (! allOnOneLine)
                    writeSpaces (out, indentLevel + indentSize);

                write (out, array.getReference(i), indentLevel + indentSize, allOnOneLine, maximumDecimalPlaces);

                if (i < array.size() - 1)
                {
                    if (allOnOneLine)
                        out << ", ";
                    else
                        out << ',' << newLine;
                }
                else if (! allOnOneLine)
                    out << newLine;
            }

            if (! allOnOneLine)
                writeSpaces (out, indentLevel);
        }

        out << ']';
    }

    enum { indentSize = 2 };
};

//==============================================================================
var JSON::parse (const String& text)
{
    var result;

    if (! parse (text, result))
        result = var();

    return result;
}

var JSON::fromString (const String& text)
{
    var result;

    if (! JSONParser::parseAny (text.text, result))
        result = var();

    return result;
}

var JSON::parse (InputStream& input)
{
    return parse (input.readEntireStreamAsString());
}

var JSON::parse (const File& file)
{
    return parse (file.loadFileAsString());
}

Result JSON::parse (const String& text, var& result)
{
    return JSONParser::parseObjectOrArray (text.c_str(), result);
}

String JSON::toString (const var& data, const bool allOnOneLine, int maximumDecimalPlaces)
{
    MemoryOutputStream mo (1024);
    JSONFormatter::write (mo, data, 0, allOnOneLine, maximumDecimalPlaces);
    return mo.toUTF8();
}

void JSON::writeToStream (OutputStream& output, const var& data, const bool allOnOneLine, int maximumDecimalPlaces)
{
    JSONFormatter::write (output, data, 0, allOnOneLine, maximumDecimalPlaces);
}

String JSON::escapeString (const String& s)
{
    MemoryOutputStream mo;
    JSONFormatter::writeString (mo, s.text);
    return mo.toString();
}

Result JSON::parseQuotedString (char*& t, var& result)
{
    auto quote = *t++;

    if (quote == '"' || quote == '\'')
        return JSONParser::parseString (quote, t, result);

    return Result::fail ("Not a quoted string!");
}


//==============================================================================
//==============================================================================
#if HUNIT_TESTS

class JSONTests  : public UnitTest
{
public:
    JSONTests()
        : UnitTest ("JSON", UnitTestCategories::json)
    {}

    static String createRandomWideCharString (Random& r)
    {
        char buffer[40] = { 0 };

        for (int i = 0; i < numElementsInArray (buffer) - 1; ++i)
        {
            if (r.nextBool())
            {
                do
                {
                    buffer[i] = (char) (1 + r.nextInt (0x10ffff - 1));
                }
                while (! CharPointer_UTF16::canRepresent (buffer[i]));
            }
            else
                buffer[i] = (char) (1 + r.nextInt (0xff));
        }

        return CharPointer_UTF32 (buffer);
    }

    static String createRandomIdentifier (Random& r)
    {
        char buffer[30] = { 0 };

        for (int i = 0; i < numElementsInArray (buffer) - 1; ++i)
        {
            static const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-:";
            buffer[i] = chars [r.nextInt (sizeof (chars) - 1)];
        }

        return CharPointer_ASCII (buffer);
    }

    // Creates a random double that can be easily stringified, to avoid
    // false failures when decimal places are rounded or truncated slightly
    static var createRandomDouble (Random& r)
    {
        return var ((r.nextDouble() * 1000.0) + 0.1);
    }

    static var createRandomVar (Random& r, int depth)
    {
        switch (r.nextInt (depth > 3 ? 6 : 8))
        {
            case 0:     return {};
            case 1:     return r.nextInt();
            case 2:     return r.nextInt64();
            case 3:     return r.nextBool();
            case 4:     return createRandomDouble (r);
            case 5:     return createRandomWideCharString (r);

            case 6:
            {
                var v (createRandomVar (r, depth + 1));

                for (int i = 1 + r.nextInt (30); --i >= 0;)
                    v.append (createRandomVar (r, depth + 1));

                return v;
            }

            case 7:
            {
                DynamicObject* o = new DynamicObject();

                for (int i = r.nextInt (30); --i >= 0;)
                    o->setProperty (createRandomIdentifier (r), createRandomVar (r, depth + 1));

                return o;
            }

            default:
                return {};
        }
    }

    void runTest() override
    {
        {
            beginTest ("JSON");

            Random r = getRandom();

            expect (JSON::parse (String()) == var());
            expect (JSON::parse ("{}").isObject());
            expect (JSON::parse ("[]").isArray());
            expect (JSON::parse ("[ 1234 ]")[0].isInt());
            expect (JSON::parse ("[ 12345678901234 ]")[0].isInt64());
            expect (JSON::parse ("[ 1.123e3 ]")[0].isDouble());
            expect (JSON::parse ("[ -1234]")[0].isInt());
            expect (JSON::parse ("[-12345678901234]")[0].isInt64());
            expect (JSON::parse ("[-1.123e3]")[0].isDouble());

            for (int i = 100; --i >= 0;)
            {
                var v;

                if (i > 0)
                    v = createRandomVar (r, 0);

                const bool oneLine = r.nextBool();
                String asString (JSON::toString (v, oneLine));
                var parsed = JSON::parse ("[" + asString + "]")[0];
                String parsedString (JSON::toString (parsed, oneLine));
                expect (!asString.empty() && parsedString == asString);
            }
        }

        {
            beginTest ("Float formatting");

            std::map<double, String> tests;
            tests[1] = "1.0";
            tests[1.1] = "1.1";
            tests[1.01] = "1.01";
            tests[0.76378] = "0.76378";
            tests[-10] = "-10.0";
            tests[10.01] = "10.01";
            tests[0.0123] = "0.0123";
            tests[-3.7e-27] = "-3.7e-27";
            tests[1e+40] = "1.0e40";
            tests[-12345678901234567.0] = "-1.234567890123457e16";
            tests[192000] = "192000.0";
            tests[1234567] = "1.234567e6";
            tests[0.00006] = "0.00006";
            tests[0.000006] = "6.0e-6";

            for (auto& test : tests)
                expectEquals (JSON::toString (test.first), test.second);
        }
    }
};

static JSONTests JSONUnitTests;

#endif


