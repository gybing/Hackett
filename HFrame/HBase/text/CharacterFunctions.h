

//==============================================================================

#if ! DOXYGEN

//==============================================================================
// GNU libstdc++ does not have std::make_unsigned
namespace internal
{
    template <typename Type> struct make_unsigned               { using type = Type; };
    template <> struct make_unsigned<signed char>               { using type = unsigned char; };
    template <> struct make_unsigned<char>                      { using type = unsigned char; };
    template <> struct make_unsigned<short>                     { using type = unsigned short; };
    template <> struct make_unsigned<int>                       { using type = unsigned int; };
    template <> struct make_unsigned<long>                      { using type = unsigned long; };
    template <> struct make_unsigned<long long>                 { using type = unsigned long long; };
}

#endif

//==============================================================================
/**
	�ַ���������������.

    @see String, char*

    @tags{Core}
*/
class API CharacterFunctions
{
public:
    //==============================================================================
    /** ����ַ��Ƿ��ǿո�. */
    static bool isWhitespace (char character) noexcept;
    /** ����ַ��Ƿ��ǿո�. */
    static bool isWhitespace (wchar_t character) noexcept;

    /** ����16�����ַ���ֵ. ��: F => 16. �������16�����ַ�����-1. */
    static int getHexDigitValue (char digit) noexcept;

	/** ����ַ�����β�ո�. */
	static String trim(const std::string& text) noexcept;

	/** ����ַ�����β�ո�. */
	static String trim(const std::string& text, bool isfront, bool isback = true) noexcept;

	/** ����ַ�����β�ո�. */
	static String trim(const std::string& text, char c, bool isfront = true, bool isback = true) noexcept;

	/** �ַ����滻. */
	static String replace(const String& text, const char* stringToReplace, const char* stringToInsert, const bool ignoreCase = false) noexcept;
	
	/** ����ַ���ǰ׺. */
	static bool startsWith(const String& text, const char* character, bool ignoreCase = false) noexcept;

	/** ����ַ�����׺. */
	static bool endsWith(const String& text, const char* character, bool ignoreCase = false) noexcept;

    /** �ַ��Ƚ�. */
	static inline int compare(char char1, char char2, bool ignoreCase = false) noexcept;

    /** �ַ����Ƚ�. */
	static int compare(const char* s1, const char* s2, int maxChars = 0, bool ignoreCase = false) noexcept;

	static String remove(const String& text, const String& charactersToRemove) noexcept;

	static String repeat(const String& stringToRepeat, size_t numberOfTimesToRepeat) noexcept;

	static bool contains(const String& text, const String& chars, bool only = false) noexcept;

	template <typename Type>
	static String hexToString(Type v)
	{
		const char* hexDigits = "0123456789abcdef";

		char buffer[32];
		auto* end = buffer + numElementsInArray(buffer) - 1;
		auto* t = end;
		*t = 0;

		do
		{
			*--t = hexDigits[(int)(v & 15)];
			v >>= 4;

		} while (v != 0);

		return String(t, end);
	}

	static String hexToString(const void* const d, const int size, const int groupSize) noexcept;
	
	
};

//==============================================================================
	/** ����16�����ַ���. */
template <typename ResultType>
struct HexParser
{
	template <typename CharPointerType>
	static ResultType parse(CharPointerType t) noexcept
	{
		ResultType result = 0;

		while (!t.empty())
		{
			auto hexValue = CharacterFunctions::getHexDigitValue(*t++);

			if (hexValue >= 0)
				result = (result << 4) | hexValue;
		}

		return result;
	}
};

// ǰ׺ƥ��
template <typename CharPointer>
struct WildCardMatcher
{
	static bool matches(CharPointer wildcard, CharPointer test, const bool ignoreCase) noexcept
	{
		for (;;)
		{
			auto wc = *wildcard++;

			if (wc == '*')
				return wildcard.empty() || matchesAnywhere(wildcard, test, ignoreCase);

			if (!characterMatches(wc, *test++, ignoreCase))
				return false;

			if (wc == 0)
				return true;
		}
	}

	static bool characterMatches(const char wc, const char tc, const bool ignoreCase) noexcept
	{
		return (wc == tc) || (wc == '?' && tc != 0)
			|| (ignoreCase && CharacterFunctions::std::tolower(wc) == CharacterFunctions::std::tolower(tc));
	}

	static bool matchesAnywhere(const CharPointer wildcard, CharPointer test, const bool ignoreCase) noexcept
	{
		for (; !test.empty(); ++test)
			if (matches(wildcard, test, ignoreCase))
				return true;

		return false;
	}
};

