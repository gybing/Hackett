

/**
    Contains some static methods for converting between binary and the
    standard base-64 encoding format.

    @tags{Core}
*/
struct API Base64
{
    /** Converts a binary block of data into a base-64 string.
        This will write the resulting string data to the given stream.
        If a write error occurs with the stream, the method will terminate and return false.
    */
    static bool convertToBase64 (OutputStream& base64Result, const void* sourceData, size_t sourceDataSize);

    /** Converts a base-64 string back to its binary representation.
        This will write the decoded binary data to the given stream.
        If the string is not valid base-64, the method will terminate and return false.
    */
    static bool convertFromBase64 (OutputStream& binaryOutput, StringRef base64TextInput);

    /** Converts a block of binary data to a base-64 string. */
    static String toBase64 (const void* sourceData, size_t sourceDataSize);

    /** Converts a string's UTF-8 representation to a base-64 string. */
    static String toBase64 (const String& textToEncode);
};


