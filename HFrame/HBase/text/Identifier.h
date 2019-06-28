

//==============================================================================
/**
    Represents a string identifier, designed for accessing properties by name.

    Comparing two Identifier objects is very fast (an O(1) operation), but creating
    them can be slower than just using a String directly, so the optimal way to use them
    is to keep some static Identifier objects for the things you use often.

    @see NamedValueSet, ValueTree

    @tags{Core}
*/
class API Identifier  final
{
public:
    /** Creates a null identifier. */
    Identifier() noexcept;

    /** Creates an identifier with a specified name.
        Because this name may need to be used in contexts such as script variables or XML
        tags, it must only contain ascii letters and digits, or the underscore character.
    */
    Identifier (const char* name);

    /** Creates an identifier with a specified name.
        Because this name may need to be used in contexts such as script variables or XML
        tags, it must only contain ascii letters and digits, or the underscore character.
    */
    Identifier (const String& name);

    /** Creates an identifier with a specified name.
        Because this name may need to be used in contexts such as script variables or XML
        tags, it must only contain ascii letters and digits, or the underscore character.
    */
    Identifier (char* nameStart, char* nameEnd);

    /** Creates a copy of another identifier. */
    Identifier (const Identifier& other) noexcept;

    /** Creates a copy of another identifier. */
    Identifier& operator= (const Identifier& other) noexcept;

    /** Creates a copy of another identifier. */
    Identifier (Identifier&& other) noexcept;

    /** Creates a copy of another identifier. */
    Identifier& operator= (Identifier&& other) noexcept;

    /** Destructor */
    ~Identifier() noexcept;

    /** Compares two identifiers. This is a very fast operation. */
    inline bool operator== (const Identifier& other) const noexcept     { return name.c_str() == other.name.c_str(); }

    /** Compares two identifiers. This is a very fast operation. */
    inline bool operator!= (const Identifier& other) const noexcept     { return name.c_str() != other.name.c_str(); }

    /** Compares the identifier with a string. */
    inline bool operator== (StringRef other) const noexcept             { return name == other; }

    /** Compares the identifier with a string. */
    inline bool operator!= (StringRef other) const noexcept             { return name != other; }

    /** Compares the identifier with a string. */
    inline bool operator<  (StringRef other) const noexcept             { return name <  other; }

    /** Compares the identifier with a string. */
    inline bool operator<= (StringRef other) const noexcept             { return name <= other; }

    /** Compares the identifier with a string. */
    inline bool operator>  (StringRef other) const noexcept             { return name >  other; }

    /** Compares the identifier with a string. */
    inline bool operator>= (StringRef other) const noexcept             { return name >= other; }

    /** Returns this identifier as a string. */
    const String& toString() const noexcept                             { return name; }

    /** Returns this identifier's raw string pointer. */
    operator char*() const noexcept                   { return (char*)name.c_str(); }

    /** Returns this identifier's raw string pointer. */
    char* c_str() const noexcept             { return (char*)name.c_str(); }

    /** Returns this identifier as a StringRef. */
    operator StringRef() const noexcept                                 { return name; }

    /** Returns true if this Identifier is not null */
    bool isValid() const noexcept                                       { return !name.empty(); }

    /** Returns true if this Identifier is null */
    bool isNull() const noexcept                                        { return name.empty(); }

    /** A null identifier. */
    static Identifier null;

    /** Checks a given string for characters that might not be valid in an Identifier.
        Since Identifiers are used as a script variables and XML attributes, they should only contain
        alphanumeric characters, underscores, or the '-' and ':' characters.
    */
    static bool isValidIdentifier (const String& possibleIdentifier) noexcept;

private:
    String name;
};


