

//==============================================================================
bool CharacterFunctions::isWhitespace (const char character) noexcept
{
    return character == ' ' || (character <= 13 && character >= 9);
}

bool CharacterFunctions::isWhitespace (const wchar_t character) noexcept
{
    return iswspace ((wint_t) character) != 0;
}

int CharacterFunctions::getHexDigitValue (const char digit) noexcept
{
    auto d = (unsigned int) (digit - '0');

    if (d < (unsigned int) 10)
        return (int) d;

    d += (unsigned int) ('0' - 'a');

    if (d < (unsigned int) 6)
        return (int) d + 10;

    d += (unsigned int) ('a' - 'A');

    if (d < (unsigned int) 6)
        return (int) d + 10;

    return -1;
}

String CharacterFunctions::trim(const std::string& text) noexcept
{
	std::string::const_iterator it = text.begin();
	while (it != text.end() && std::isspace(*it))
		it++;

	std::string::const_reverse_iterator rit = text.rbegin();
	while (rit.base() != it && std::isspace(*rit))
		rit++;

	return std::string(it, rit.base());
}

String CharacterFunctions::trim(const std::string& text, char c, bool isfront /*= true*/, bool isback /*= true*/) noexcept
{
	std::string::const_iterator it = text.begin();
	if (isfront)
	{
		while (it != text.end() && (*it == c))
			it++;
	}

	std::string::const_reverse_iterator rit = text.rbegin();
	if (isback)
	{
		while (rit.base() != it && (*rit == c))
			rit++;
	}

	return std::string(it, rit.base());
}

String CharacterFunctions::trim(const std::string& text, bool isfront, bool isback /*= true*/) noexcept
{
	std::string::const_iterator it = text.begin();
	if (isfront)
	{
		while (it != text.end() && std::isspace(*it))
			it++;
	}

	std::string::const_reverse_iterator rit = text.rbegin();
	if (isback)
	{
		while (rit.base() != it && std::isspace(*rit))
			rit++;
	}

	return std::string(it, rit.base());
}

String CharacterFunctions::replace(const String& text, const char* stringToReplace, const char* stringToInsert, const bool ignoreCase /*= false*/) noexcept
{
	if (text.empty() || stringToReplace == nullptr || stringToInsert == nullptr)
	{
		return text;
	}

	std::size_t stringToReplaceSize = std::strlen(stringToReplace);
	std::size_t stringToInsertSize = std::strlen(stringToInsert);

	if (stringToInsertSize <= 0 || stringToInsert <= 0)
	{
		return text;
	}

	std::string dest(text);
	std::string from(stringToReplace);

	if (ignoreCase)
	{
		std::transform(dest.begin(), dest.end(), dest.begin(), std::tolower);
		std::transform(from.begin(), from.end(), from.begin(), std::tolower);
	}

	for (std::string::size_type pos = 0;
		(pos = dest.find(from, pos)) != std::string::npos;
		pos += stringToInsertSize)
	{
		dest.replace(pos, stringToReplaceSize, stringToInsert);
	}

	return dest;
}

bool CharacterFunctions::startsWith(const String& text, const char* character, bool ignoreCase /*= false*/) noexcept
{
	return 0 == compare(text.c_str(), character, std::strlen(character));
}

bool CharacterFunctions::endsWith(const String& text, const char* character, bool ignoreCase /*= false*/) noexcept
{
	auto end = text.end();
	auto otherEnd = character + std::strlen(character);

	while (end != text.begin() && otherEnd != character)
	{
		--end;
		--otherEnd;

		if (ignoreCase ? (std::tolower(*end) != std::tolower(*otherEnd)) : ((*end != *otherEnd)))
			return false;
	}

	return otherEnd == character;
}

int CharacterFunctions::compare(char char1, char char2, bool ignoreCase /*= false*/) noexcept
{
	return char1 != char2 ? (ignoreCase && compare(std::toupper(char1), std::toupper(char2))) : 0;
}

int CharacterFunctions::compare(const char* s1, const char* s2, int maxChars /*= 0*/, bool ignoreCase /*= false*/) noexcept
{
	while (--maxChars >= 0)
	{
		auto c1 = *s1++;

		if (auto diff = compare(c1, *s2++, ignoreCase))
			return diff;

		if (c1 == 0)
			break;
	}

	return 0;
}

String CharacterFunctions::remove(const String& text, const String& charactersToRemove) noexcept
{
	if (text.empty())
		return text;

	String builder(text);

	for (auto i = text.rbegin(); i != text.rend(); ++i)
	{
		if (charactersToRemove.find(*i) < 0)
			builder += *i;
	}

	return std::move(builder);
}

String CharacterFunctions::repeat(const String& stringToRepeat, size_t numberOfTimesToRepeat) noexcept
{
	if (numberOfTimesToRepeat <= 0)
		return {};

	String result(stringToRepeat.size() * numberOfTimesToRepeat, 0);
	auto n = (char*)result.data();

	while (--numberOfTimesToRepeat >= 0)
		std::strcpy(n, stringToRepeat.c_str());

	return result;
}

bool CharacterFunctions::contains(const String& text, const String& chars, bool only /*= false*/) noexcept
{
	if (only)
	{
		for (auto t = text.begin(); t != text.end(); ++t)
			if (chars.find(*t) < 0)
				return false;
	}
	else
	{
		for (auto t = text.begin(); t != text.end(); ++t)
			if (chars.find(*t) >= 0)
				return true;
	}

	return only;
}

String CharacterFunctions::hexToString(const void* const d, const int size, const int groupSize) noexcept
{
	const char* hexDigits = "0123456789abcdef";

	if (size <= 0)
		return {};

	int numChars = (size * 2) + 2;
	if (groupSize > 0)
		numChars += size / groupSize;

	String s;

	auto* data = static_cast<const unsigned char*> (d);

	for (int i = 0; i < size; ++i)
	{
		const unsigned char nextByte = *data++;
		dest += ((char)hexDigits[nextByte >> 4]);
		dest += ((char)hexDigits[nextByte & 0xf]);

		if (groupSize > 0 && (i % groupSize) == (groupSize - 1) && i < (size - 1))
			dest += ' ';
	}

	return s;
}