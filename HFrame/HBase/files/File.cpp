

File::File (const String& fullPathName)
    : fullPath (parseAbsolutePath (fullPathName))
{
}

File File::createFileWithoutCheckingPath (const String& path) noexcept
{
    File f;
    f.fullPath = path;
    return f;
}

File::File (const File& other)
    : fullPath (other.fullPath)
{
}

File& File::operator= (const String& newPath)
{
    fullPath = parseAbsolutePath (newPath);
    return *this;
}

File& File::operator= (const File& other)
{
    fullPath = other.fullPath;
    return *this;
}

File::File (File&& other) noexcept
    : fullPath (std::move (other.fullPath))
{
}

File& File::operator= (File&& other) noexcept
{
    fullPath = std::move (other.fullPath);
    return *this;
}

HDECLARE_DEPRECATED_STATIC (const File File::nonexistent{};)

//==============================================================================
static String removeEllipsis (const String& path)
{
    // This will quickly find both /../ and /./ at the expense of a minor
    // false-positive performance hit when path elements end in a dot.
   #if HWINDOWS
    if (std::string::npos != path.find (".\\"))
   #else
    if (path.contains ("./"))
   #endif
    {
        StringArray toks;
        toks.addTokens (path, File::getSeparatorChars(), {});
        bool anythingChanged = false;

        for (int i = 1; i < toks.size(); ++i)
        {
            auto& t = toks[i];

            if (t == ".." && toks[i - 1] != "..")
            {
                anythingChanged = true;
                toks.removeRange (i - 1, 2);
                i = jmax (0, i - 2);
            }
            else if (t == ".")
            {
                anythingChanged = true;
                toks.remove (i--);
            }
        }

        if (anythingChanged)
            return toks.joinIntoString (File::getSeparatorChars());
    }

    return path;
}

bool File::isRoot() const
{
    return !fullPath.empty() && *this == getParentDirectory();
}

String File::parseAbsolutePath (const String& p)
{
    if (p.empty())
        return {};

#if HWINDOWS
	auto path = p;

	while (true) {
		std::string::size_type   pos(0);
		if ((pos = path.find('/')) != std::string::npos)
			path.replace(pos, 1, "\\");
		else   break;
	}

    path = removeEllipsis (path);

    if (path[0] == getSeparatorChar())
    {
        if (path[1] != getSeparatorChar())
        {
            /*  When you supply a raw string to the File object constructor, it must be an absolute path.
                If you're trying to parse a string that may be either a relative path or an absolute path,
                you MUST provide a context against which the partial path can be evaluated - you can do
                this by simply using File::getChildFile() instead of the File constructor. E.g. saying
                "File::getCurrentWorkingDirectory().getChildFile (myUnknownPath)" would return an absolute
                path if that's what was supplied, or would evaluate a partial path relative to the CWD.
            */
            HAssertfalse;

            path = File::getCurrentWorkingDirectory().getFullPathName().substr (0, 2) + path;
        }
    }
    else if (std::string::npos == path.find (':'))
    {
        /*  When you supply a raw string to the File object constructor, it must be an absolute path.
            If you're trying to parse a string that may be either a relative path or an absolute path,
            you MUST provide a context against which the partial path can be evaluated - you can do
            this by simply using File::getChildFile() instead of the File constructor. E.g. saying
            "File::getCurrentWorkingDirectory().getChildFile (myUnknownPath)" would return an absolute
            path if that's what was supplied, or would evaluate a partial path relative to the CWD.
        */
        HAssertfalse;

        return File::getCurrentWorkingDirectory().getChildFile (path).getFullPathName();
    }
#else
    // Mac or Linux..

    // Yes, I know it's legal for a unix pathname to contain a backslash, but this assertion is here
    // to catch anyone who's trying to run code that was written on Windows with hard-coded path names.
    // If that's why you've ended up here, use File::getChildFile() to build your paths instead.
    HAssert ((! p.containsChar ('\\')) || (p.indexOfChar ('/') >= 0 && p.indexOfChar ('/') < p.indexOfChar ('\\')));

    auto path = removeEllipsis (p);

    if (path.startsWithChar ('~'))
    {
        if (path[1] == getSeparatorChar() || path[1] == 0)
        {
            // expand a name of the form "~/abc"
            path = File::getSpecialLocation (File::userHomeDirectory).getFullPathName()
                    + path.substr (1);
        }
        else
        {
            // expand a name of type "~dave/abc"
            auto userName = path.substr (1).upToFirstOccurrenceOf ("/", false, false);

            if (auto* pw = getpwnam (userName.toUTF8()))
                path = addTrailingSeparator (pw->pw_dir) + path.fromFirstOccurrenceOf ("/", false, false);
        }
    }
    else if (! path.startsWithChar (getSeparatorChar()))
    {
       #if HDEBUG || HLOG_ASSERTIONS
        if (! (path.startsWith ("./") || path.startsWith ("../")))
        {
            /*  When you supply a raw string to the File object constructor, it must be an absolute path.
                If you're trying to parse a string that may be either a relative path or an absolute path,
                you MUST provide a context against which the partial path can be evaluated - you can do
                this by simply using File::getChildFile() instead of the File constructor. E.g. saying
                "File::getCurrentWorkingDirectory().getChildFile (myUnknownPath)" would return an absolute
                path if that's what was supplied, or would evaluate a partial path relative to the CWD.
            */
            HAssertfalse;

           #if HLOG_ASSERTIONS
            Logger::writeToLog ("Illegal absolute path: " + path);
           #endif
        }
       #endif

        return File::getCurrentWorkingDirectory().getChildFile (path).getFullPathName();
    }
#endif

    while (path.back() == getSeparatorChar() && path != getSeparatorChars()) // careful not to turn a single "/" into an empty string.
        path = path.substr (0, path.length() - 1);

    return path;
}

String File::addTrailingSeparator (const String& path)
{
    return path.back() == getSeparatorChar() ? path
                                                  : path + getSeparatorChar();
}

//==============================================================================
#if HLINUX
 #define NAMES_ARE_CASE_SENSITIVE 1
#endif

bool File::areFileNamesCaseSensitive()
{
   #if NAMES_ARE_CASE_SENSITIVE
    return true;
   #else
    return false;
   #endif
}

static int compareFilenames (const String& name1, const String& name2) noexcept
{
   #if NAMES_ARE_CASE_SENSITIVE
    return name1.compare (name2);
   #else
    return CharacterFunctions::compare(name1.c_str(), name2.c_str(), true);
   #endif
}

bool File::operator== (const File& other) const     { return compareFilenames (fullPath, other.fullPath) == 0; }
bool File::operator!= (const File& other) const     { return compareFilenames (fullPath, other.fullPath) != 0; }
bool File::operator<  (const File& other) const     { return compareFilenames (fullPath, other.fullPath) <  0; }
bool File::operator>  (const File& other) const     { return compareFilenames (fullPath, other.fullPath) >  0; }

//==============================================================================
bool File::setReadOnly (const bool shouldBeReadOnly,
                        const bool applyRecursively) const
{
    bool worked = true;

    if (applyRecursively && isDirectory())
        for (auto& f : findChildFiles (File::findFilesAndDirectories, false))
            worked = f.setReadOnly (shouldBeReadOnly, true) && worked;

    return setFileReadOnlyInternal (shouldBeReadOnly) && worked;
}

bool File::setExecutePermission (bool shouldBeExecutable) const
{
    return setFileExecutableInternal (shouldBeExecutable);
}

bool File::deleteRecursively (bool followSymlinks) const
{
    bool worked = true;

    if (isDirectory() && (followSymlinks || ! isSymbolicLink()))
        for (auto& f : findChildFiles (File::findFilesAndDirectories, false))
            worked = f.deleteRecursively (followSymlinks) && worked;

    return deleteFile() && worked;
}

bool File::moveFileTo (const File& newFile) const
{
    if (newFile.fullPath == fullPath)
        return true;

    if (! exists())
        return false;

   #if ! NAMES_ARE_CASE_SENSITIVE
    if (*this != newFile)
   #endif
        if (! newFile.deleteFile())
            return false;

    return moveInternal (newFile);
}

bool File::copyFileTo (const File& newFile) const
{
    return (*this == newFile)
            || (exists() && newFile.deleteFile() && copyInternal (newFile));
}

bool File::replaceFileIn (const File& newFile) const
{
    if (newFile.fullPath == fullPath)
        return true;

    if (! newFile.exists())
        return moveFileTo (newFile);

    if (! replaceInternal (newFile))
        return false;

    deleteFile();
    return true;
}

bool File::copyDirectoryTo (const File& newDirectory) const
{
    if (isDirectory() && newDirectory.createDirectory())
    {
        for (auto& f : findChildFiles (File::findFiles, false))
            if (! f.copyFileTo (newDirectory.getChildFile (f.getFileName())))
                return false;

        for (auto& f : findChildFiles (File::findDirectories, false))
            if (! f.copyDirectoryTo (newDirectory.getChildFile (f.getFileName())))
                return false;

        return true;
    }

    return false;
}

//==============================================================================
String File::getPathUpToLastSlash() const
{
    auto lastSlash = fullPath.find_last_of (getSeparatorChar());

    if (lastSlash > 0)
        return fullPath.substr (0, lastSlash);

    if (lastSlash == std::string::npos)
        return getSeparatorChars();

    return fullPath;
}

File File::getParentDirectory() const
{
    return createFileWithoutCheckingPath (getPathUpToLastSlash());
}

//==============================================================================
String File::getFileName() const
{
    return fullPath.substr (fullPath.find_last_of (getSeparatorChar()) + 1);
}

String File::getFileNameWithoutExtension() const
{
    auto lastSlash = fullPath.find_last_of (getSeparatorChar()) + 1;
    auto lastDot   = fullPath.find_last_of ('.');

    if (lastDot > lastSlash)
        return fullPath.substr (lastSlash, lastDot);

    return fullPath.substr (lastSlash);
}

bool File::isAChildOf (const File& potentialParent) const
{
    if (potentialParent.fullPath.empty())
        return false;

    auto ourPath = getPathUpToLastSlash();

    if (compareFilenames (potentialParent.fullPath, ourPath) == 0)
        return true;

    if (potentialParent.fullPath.length() >= ourPath.length())
        return false;

    return getParentDirectory().isAChildOf (potentialParent);
}

size_t   File::hashCode() const { return std::hash<std::string>{}(fullPath); }
int64 File::hashCode64() const  { return std::hash<std::string>{}(fullPath); }

//==============================================================================
bool File::isAbsolutePath (const String& path)
{
    auto firstChar = *(path.c_str());

    return firstChar == getSeparatorChar()
           #if HWINDOWS
            || (firstChar != 0 && path.at(1) == ':');
           #else
            || firstChar == '~';
           #endif
}

File File::getChildFile (const String& relativePath) const
{
    if (isAbsolutePath (relativePath))
        return File (relativePath);

	auto r = relativePath.c_str();

   #if HWINDOWS
	auto index = relativePath.find('/');
	if (index != String::npos)
		return getChildFile(String(relativePath).replace(index, 1, "\\"));
   #endif

    auto path = fullPath;
    auto separatorChar = getSeparatorChar();

    while (*r == '.')
    {
        auto lastPos = r;
        auto secondChar = *++r;

        if (secondChar == '.') // remove "../"
        {
            auto thirdChar = *++r;

            if (thirdChar == separatorChar || thirdChar == 0)
            {
                auto lastSlash = path.find_last_of (separatorChar);

                if (lastSlash >= 0)
                    path = path.substr (0, lastSlash);

                while (*r == separatorChar) // ignore duplicate slashes
                    ++r;
            }
            else
            {
                r = lastPos;
                break;
            }
        }
        else if (secondChar == separatorChar || secondChar == 0)  // remove "./"
        {
            while (*r == separatorChar) // ignore duplicate slashes
                ++r;
        }
        else
        {
            r = lastPos;
            break;
        }
    }

    path = addTrailingSeparator (path);
    path.append (r);
    return File (path);
}

File File::getSiblingFile (const String& fileName) const
{
    return getParentDirectory().getChildFile (fileName);
}

//==============================================================================
String File::descriptionOfSizeInBytes (const int64 bytes)
{
    const char* suffix;
    double divisor = 0;

    if (bytes == 1)                       { suffix = " byte"; }
    else if (bytes < 1024)                { suffix = " bytes"; }
    else if (bytes < 1024 * 1024)         { suffix = " KB"; divisor = 1024.0; }
    else if (bytes < 1024 * 1024 * 1024)  { suffix = " MB"; divisor = 1024.0 * 1024.0; }
    else                                  { suffix = " GB"; divisor = 1024.0 * 1024.0 * 1024.0; }

    return (divisor > 0 ? std::to_string((int64)(bytes * 0.1 / divisor)) : std::to_string(bytes)) + suffix;
}

//==============================================================================
Result File::create() const
{
    if (exists())
        return Result::ok();

    auto parentDir = getParentDirectory();

    if (parentDir == *this)
        return Result::fail ("Cannot create parent directory");

    auto r = parentDir.createDirectory();

    if (r.wasOk())
    {
        FileOutputStream fo (*this, 8);
        r = fo.getStatus();
    }

    return r;
}

Result File::createDirectory() const
{
    if (isDirectory())
        return Result::ok();

    auto parentDir = getParentDirectory();

    if (parentDir == *this)
        return Result::fail ("Cannot create parent directory");

    auto r = parentDir.createDirectory();

    if (r.wasOk())
        r = createDirectoryInternal (CharacterFunctions::trim (fullPath, getSeparatorChar(), false));

    return r;
}

//==============================================================================
Time File::getLastModificationTime() const           { int64 m, a, c; getFileTimesInternal (m, a, c); return Time (m); }
Time File::getLastAccessTime() const                 { int64 m, a, c; getFileTimesInternal (m, a, c); return Time (a); }
Time File::getCreationTime() const                   { int64 m, a, c; getFileTimesInternal (m, a, c); return Time (c); }

bool File::setLastModificationTime (Time t) const    { return setFileTimesInternal (t.toMilliseconds(), 0, 0); }
bool File::setLastAccessTime (Time t) const          { return setFileTimesInternal (0, t.toMilliseconds(), 0); }
bool File::setCreationTime (Time t) const            { return setFileTimesInternal (0, 0, t.toMilliseconds()); }

//==============================================================================
bool File::loadFileAsData (MemoryBlock& destBlock) const
{
    if (! existsAsFile())
        return false;

    FileInputStream in (*this);
    return in.openedOk() && getSize() == (int64) in.readIntoMemoryBlock (destBlock);
}

String File::loadFileAsString() const
{
    if (! existsAsFile())
        return {};

    FileInputStream in (*this);
    return in.openedOk() ? in.readEntireStreamAsString()
                         : String();
}

void File::readLines (StringArray& destLines) const
{
    destLines.addLines (loadFileAsString());
}

//==============================================================================
Array<File> File::findChildFiles (int whatToLookFor, bool searchRecursively, const String& wildcard) const
{
    Array<File> results;
    findChildFiles (results, whatToLookFor, searchRecursively, wildcard);
    return results;
}

int File::findChildFiles (Array<File>& results, int whatToLookFor, bool searchRecursively, const String& wildcard) const
{
    int total = 0;

    for (DirectoryIterator di (*this, searchRecursively, wildcard, whatToLookFor); di.next();)
    {
        results.add (di.getFile());
        ++total;
    }

    return total;
}

int File::getNumberOfChildFiles (const int whatToLookFor, const String& wildCardPattern) const
{
    int total = 0;

    for (DirectoryIterator di (*this, false, wildCardPattern, whatToLookFor); di.next();)
        ++total;

    return total;
}

bool File::containsSubDirectories() const
{
    if (! isDirectory())
        return false;

    DirectoryIterator di (*this, false, "*", findDirectories);
    return di.next();
}

//==============================================================================
File File::getNonexistentChildFile (const String& suggestedPrefix,
                                    const String& suffix,
                                    bool putNumbersInBrackets) const
{
    auto f = getChildFile (suggestedPrefix + suffix);

    if (f.exists())
    {
        int number = 1;
        auto prefix = suggestedPrefix;

        // remove any bracketed numbers that may already be on the end..
        if (CharacterFunctions::endsWith (CharacterFunctions::trim(prefix), ")"))
        {
            putNumbersInBrackets = true;

            auto openBracks  = prefix.find_last_of ('(');
            auto closeBracks = prefix.find_last_of (')');

			auto text = prefix.substr(openBracks + 1, closeBracks);

            if (openBracks > 0
                 && closeBracks > openBracks
                 && !std::any_of(text.begin(), text.end(), [](const char& c) { return std::isdigit(c); }))
            {
				number = std::atoi(text.c_str());
                prefix = prefix.substr (0, openBracks);
            }
        }

        do
        {
            auto newName = prefix;

            if (putNumbersInBrackets)
            {
                newName += '(' + ++number + ')';
            }
            else
            {
                if (std::isdigit (prefix.back()))
                    newName += '_'; // pad with an underscore if the name already ends in a digit

                newName += ++number;
            }

            f = getChildFile (newName + suffix);

        } while (f.exists());
    }

    return f;
}

File File::getNonexistentSibling (const bool putNumbersInBrackets) const
{
    if (! exists())
        return *this;

    return getParentDirectory().getNonexistentChildFile (getFileNameWithoutExtension(),
                                                         getFileExtension(),
                                                         putNumbersInBrackets);
}

//==============================================================================
String File::getFileExtension() const
{
    auto indexOfDot = fullPath.find_last_of ('.');

    if (indexOfDot > fullPath.find_last_of (getSeparatorChar()))
        return fullPath.substr (indexOfDot);

    return {};
}

bool File::hasFileExtension (const String& possibleSuffix) const
{
    if (possibleSuffix.empty())
        return fullPath.find_last_of ('.') <= fullPath.find_last_of (getSeparatorChar());

    auto semicolon = possibleSuffix.find (';');

    if (semicolon >= 0)
        return hasFileExtension (CharacterFunctions::trim(String(possibleSuffix).substr(0, semicolon), false))
                || hasFileExtension (possibleSuffix.substr(semicolon + 1, possibleSuffix.find_last_of(' ') - semicolon - 1));

    if (CharacterFunctions::endsWith (fullPath, possibleSuffix.c_str(), true))
    {
        if (possibleSuffix[0] == '.')
            return true;

        auto dotPos = fullPath.length() - possibleSuffix.length() - 1;

        if (dotPos >= 0)
            return fullPath[dotPos] == '.';
    }

    return false;
}

File File::withFileExtension (const String& newExtension) const
{
    if (fullPath.empty())
        return {};

    auto filePart = getFileName();

    auto lastDot = filePart.find_last_of ('.');

    if (lastDot >= 0)
        filePart = filePart.substr (0, lastDot);

    if (!newExtension.empty() && newExtension[0] != '.')
        filePart += '.';

    return getSiblingFile (filePart + newExtension);
}

//==============================================================================
bool File::startAsProcess (const String& parameters) const
{
    return exists() && Process::openDocument (fullPath, parameters);
}

//==============================================================================
FileInputStream* File::createInputStream() const
{
    std::unique_ptr<FileInputStream> fin (new FileInputStream (*this));

    if (fin->openedOk())
        return fin.release();

    return nullptr;
}

FileOutputStream* File::createOutputStream (size_t bufferSize) const
{
    std::unique_ptr<FileOutputStream> out (new FileOutputStream (*this, bufferSize));

    return out->failedToOpen() ? nullptr
                               : out.release();
}

//==============================================================================
bool File::appendData (const void* const dataToAppend,
                       const size_t numberOfBytes) const
{
    HAssert (((ssize_t) numberOfBytes) >= 0);

    if (numberOfBytes == 0)
        return true;

    FileOutputStream out (*this, 8192);
    return out.openedOk() && out.write (dataToAppend, numberOfBytes);
}

bool File::replaceWithData (const void* const dataToWrite,
                            const size_t numberOfBytes) const
{
    if (numberOfBytes == 0)
        return deleteFile();

    TemporaryFile tempFile (*this, TemporaryFile::useHiddenFile);
    tempFile.getFile().appendData (dataToWrite, numberOfBytes);
    return tempFile.overwriteTargetFileWithTemporary();
}

bool File::appendText (const String& text, bool asUnicode, bool writeHeaderBytes, const char* lineFeed) const
{
    FileOutputStream out (*this);

    if (out.failedToOpen())
        return false;

    return out.writeText (text, asUnicode, writeHeaderBytes, lineFeed);
}

bool File::replaceWithText (const String& textToWrite, bool asUnicode, bool writeHeaderBytes, const char* lineFeed) const
{
    TemporaryFile tempFile (*this, TemporaryFile::useHiddenFile);
    tempFile.getFile().appendText (textToWrite, asUnicode, writeHeaderBytes, lineFeed);
    return tempFile.overwriteTargetFileWithTemporary();
}

bool File::hasIdenticalContentTo (const File& other) const
{
    if (other == *this)
        return true;

    if (getSize() == other.getSize() && existsAsFile() && other.existsAsFile())
    {
        FileInputStream in1 (*this), in2 (other);

        if (in1.openedOk() && in2.openedOk())
        {
            const int bufferSize = 4096;
            HeapBlock<char> buffer1 (bufferSize), buffer2 (bufferSize);

            for (;;)
            {
                auto num1 = in1.read (buffer1, bufferSize);
                auto num2 = in2.read (buffer2, bufferSize);

                if (num1 != num2)
                    break;

                if (num1 <= 0)
                    return true;

                if (memcmp (buffer1, buffer2, (size_t) num1) != 0)
                    break;
            }
        }
    }

    return false;
}

//==============================================================================
String File::createLegalPathName (const String& original)
{
    auto s = original;
    String start;

    if (!s.empty() && s[1] == ':')
    {
        start = s.substr (0, 2);
        s = s.substr (2);
    }

    return start + CharacterFunctions::remove(s, "\"#@,;:<>*^|?").substr (0, 1024);
}

String File::createLegalFileName (const String& original)
{
    auto s = CharacterFunctions::remove (original, "\"#@,;:<>*^|?\\/");

    const int maxLength = 128; // only the length of the filename, not the whole path
    auto len = s.length();

    if (len > maxLength)
    {
        auto lastDot = s.find_last_of ('.');

        if (lastDot > std::max ((size_t)0, len - 12))
        {
            s = s.substr (0, maxLength - (len - lastDot))
                 + s.substr (lastDot);
        }
        else
        {
            s = s.substr (0, maxLength);
        }
    }

    return s;
}

//==============================================================================
static int countNumberOfSeparators (const char* s)
{
    int num = 0;

    for (;;)
    {
        auto c = *s++;

        if (c == 0)
            break;

        if (c == File::getSeparatorChar())
            ++num;
    }

    return num;
}

String File::getRelativePathFrom (const File& dir) const
{
    if (dir == *this)
        return ".";

    auto thisPath = fullPath;

    while (CharacterFunctions::endsWith (thisPath, getSeparatorChars()))
        thisPath = thisPath.substr (0, thisPath.length() - 1);

    auto dirPath = addTrailingSeparator (dir.existsAsFile() ? dir.getParentDirectory().getFullPathName()
                                                            : dir.fullPath);

    int commonBitLength = 0;
    auto thisPathAfterCommon = thisPath.c_str();
    auto dirPathAfterCommon  = dirPath.c_str();

    {
        auto thisPathIter = thisPath.c_str();
        auto dirPathIter = dirPath.c_str();

        for (int i = 0;;)
        {
            auto c1 = *thisPathIter++;
            auto c2 = *dirPathIter++;

           #if NAMES_ARE_CASE_SENSITIVE
            if (c1 != c2
           #else
            if ((c1 != c2 && std::tolower (c1) != std::tolower (c2))
           #endif
                 || c1 == 0)
                break;

            ++i;

            if (c1 == getSeparatorChar())
            {
                thisPathAfterCommon = thisPathIter;
                dirPathAfterCommon  = dirPathIter;
                commonBitLength = i;
            }
        }
    }

    // if the only common bit is the root, then just return the full path..
    if (commonBitLength == 0 || (commonBitLength == 1 && thisPath[1] == getSeparatorChar()))
        return fullPath;

    auto numUpDirectoriesNeeded = countNumberOfSeparators (dirPathAfterCommon);

    if (numUpDirectoriesNeeded == 0)
        return thisPathAfterCommon;

   #if HWINDOWS
    auto s = CharacterFunctions::repeat ("..\\", numUpDirectoriesNeeded);
   #else
    auto s = CharacterFunctions::repeat ("../",  numUpDirectoriesNeeded);
   #endif
    s += thisPathAfterCommon;
    return s;
}

//==============================================================================
File File::createTempFile (const String& fileNameEnding)
{
    auto tempFile = getSpecialLocation (tempDirectory)
                      .getChildFile ("temp_" + CharacterFunctions::hexToString (Random::getSystemRandom().nextInt()))
                      .withFileExtension (fileNameEnding);

    if (tempFile.exists())
        return createTempFile (fileNameEnding);

    return tempFile;
}

bool File::createSymbolicLink (const File& linkFileToCreate,
                               const String& nativePathOfTarget,
                               bool overwriteExisting)
{
    if (linkFileToCreate.exists())
    {
        if (! linkFileToCreate.isSymbolicLink())
        {
            // user has specified an existing file / directory as the link
            // this is bad! the user could end up unintentionally destroying data
            HAssertfalse;
            return false;
        }

        if (overwriteExisting)
            linkFileToCreate.deleteFile();
    }

   #if HMAC || HLINUX
    // one common reason for getting an error here is that the file already exists
    if (symlink (nativePathOfTarget.toRawUTF8(), linkFileToCreate.getFullPathName().toRawUTF8()) == -1)
    {
        HAssertfalse;
        return false;
    }

    return true;
   #elif HMSVC
    File targetFile (linkFileToCreate.getSiblingFile (nativePathOfTarget));

    return CreateSymbolicLinkA (linkFileToCreate.getFullPathName().c_str(),
                               nativePathOfTarget.c_str(),
                               targetFile.isDirectory() ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0) != FALSE;
   #else
    ignoreUnused (nativePathOfTarget);
    HAssertfalse; // symbolic links not supported on this platform!
    return false;
   #endif
}

bool File::createSymbolicLink (const File& linkFileToCreate, bool overwriteExisting) const
{
    return createSymbolicLink (linkFileToCreate, getFullPathName(), overwriteExisting);
}

#if ! HWINDOWS
File File::getLinkedTarget() const
{
    if (isSymbolicLink())
        return getSiblingFile (getNativeLinkedTarget());

    return *this;
}
#endif

//==============================================================================
MemoryMappedFile::MemoryMappedFile (const File& file, MemoryMappedFile::AccessMode mode, bool exclusive)
    : range (0, file.getSize())
{
    openInternal (file, mode, exclusive);
}

MemoryMappedFile::MemoryMappedFile (const File& file, const Range<int64>& fileRange, AccessMode mode, bool exclusive)
    : range (fileRange.getIntersectionWith (Range<int64> (0, file.getSize())))
{
    openInternal (file, mode, exclusive);
}


//==============================================================================
//==============================================================================
#if HUNIT_TESTS

class FileTests  : public UnitTest
{
public:
    FileTests()
        : UnitTest ("Files", UnitTestCategories::files)
    {}

    void runTest() override
    {
        beginTest ("Reading");

        const File home (File::getSpecialLocation (File::userHomeDirectory));
        const File temp (File::getSpecialLocation (File::tempDirectory));

        expect (! File().exists());
        expect (! File().existsAsFile());
        expect (! File().isDirectory());
       #if ! HWINDOWS
        expect (File("/").isDirectory());
       #endif
        expect (home.isDirectory());
        expect (home.exists());
        expect (! home.existsAsFile());
        expect (File::getSpecialLocation (File::userDocumentsDirectory).isDirectory());
        expect (File::getSpecialLocation (File::userApplicationDataDirectory).isDirectory());
        expect (File::getSpecialLocation (File::currentExecutableFile).exists());
        expect (File::getSpecialLocation (File::currentApplicationFile).exists());
        expect (File::getSpecialLocation (File::invokedExecutableFile).exists());
        expect (home.getVolumeTotalSize() > 1024 * 1024);
        expect (home.getBytesFreeOnVolume() > 0);
        expect (! home.isHidden());
        expect (home.isOnHardDisk());
        expect (! home.isOnCDRomDrive());
        expect (File::getCurrentWorkingDirectory().exists());
        expect (home.setAsCurrentWorkingDirectory());
        expect (File::getCurrentWorkingDirectory() == home);

        {
            Array<File> roots;
            File::findFileSystemRoots (roots);
            expect (roots.size() > 0);

            int numRootsExisting = 0;
            for (int i = 0; i < roots.size(); ++i)
                if (roots[i].exists())
                    ++numRootsExisting;

            // (on windows, some of the drives may not contain media, so as long as at least one is ok..)
            expect (numRootsExisting > 0);
        }

        beginTest ("Writing");

        File demoFolder (temp.getChildFile ("H UnitTests Temp Folder.folder"));
        expect (demoFolder.deleteRecursively());
        expect (demoFolder.createDirectory());
        expect (demoFolder.isDirectory());
        expect (demoFolder.getParentDirectory() == temp);
        expect (temp.isDirectory());
        expect (temp.findChildFiles (File::findFilesAndDirectories, false, "*").contains (demoFolder));
        expect (temp.findChildFiles (File::findDirectories, true, "*.folder").contains (demoFolder));

        File tempFile (demoFolder.getNonexistentChildFile ("test", ".txt", false));

        expect (tempFile.getFileExtension() == ".txt");
        expect (tempFile.hasFileExtension (".txt"));
        expect (tempFile.hasFileExtension ("txt"));
        expect (tempFile.withFileExtension ("xyz").hasFileExtension (".xyz"));
        expect (tempFile.withFileExtension ("xyz").hasFileExtension ("abc;xyz;foo"));
        expect (tempFile.withFileExtension ("xyz").hasFileExtension ("xyz;foo"));
        expect (! tempFile.withFileExtension ("h").hasFileExtension ("bar;foo;xx"));
        expect (tempFile.getSiblingFile ("foo").isAChildOf (temp));
        expect (tempFile.hasWriteAccess());

        expect (home.getChildFile (".") == home);
        expect (home.getChildFile ("..") == home.getParentDirectory());
        expect (home.getChildFile (".xyz").getFileName() == ".xyz");
        expect (home.getChildFile ("..xyz").getFileName() == "..xyz");
        expect (home.getChildFile ("...xyz").getFileName() == "...xyz");
        expect (home.getChildFile ("./xyz") == home.getChildFile ("xyz"));
        expect (home.getChildFile ("././xyz") == home.getChildFile ("xyz"));
        expect (home.getChildFile ("../xyz") == home.getParentDirectory().getChildFile ("xyz"));
        expect (home.getChildFile (".././xyz") == home.getParentDirectory().getChildFile ("xyz"));
        expect (home.getChildFile (".././xyz/./abc") == home.getParentDirectory().getChildFile ("xyz/abc"));
        expect (home.getChildFile ("./../xyz") == home.getParentDirectory().getChildFile ("xyz"));
        expect (home.getChildFile ("a1/a2/a3/./../../a4") == home.getChildFile ("a1/a4"));

        {
            FileOutputStream fo (tempFile);
            fo.write ("0123456789", 10);
        }

        expect (tempFile.exists());
        expect (tempFile.getSize() == 10);
        expect (std::abs ((int) (tempFile.getLastModificationTime().toMilliseconds() - Time::getCurrentTime().toMilliseconds())) < 3000);
        expectEquals (tempFile.loadFileAsString(), String ("0123456789"));
        expect (! demoFolder.containsSubDirectories());

        expectEquals (tempFile.getRelativePathFrom (demoFolder.getParentDirectory()), demoFolder.getFileName() + File::getSeparatorChars() + tempFile.getFileName());
        expectEquals (demoFolder.getParentDirectory().getRelativePathFrom (tempFile), String("..") + File::getSeparatorChars() + ".." + File::getSeparatorChars() + demoFolder.getParentDirectory().getFileName());

        expect (demoFolder.getNumberOfChildFiles (File::findFiles) == 1);
        expect (demoFolder.getNumberOfChildFiles (File::findFilesAndDirectories) == 1);
        expect (demoFolder.getNumberOfChildFiles (File::findDirectories) == 0);
        demoFolder.getNonexistentChildFile ("tempFolder", "", false).createDirectory();
        expect (demoFolder.getNumberOfChildFiles (File::findDirectories) == 1);
        expect (demoFolder.getNumberOfChildFiles (File::findFilesAndDirectories) == 2);
        expect (demoFolder.containsSubDirectories());

        expect (tempFile.hasWriteAccess());
        tempFile.setReadOnly (true);
        expect (! tempFile.hasWriteAccess());
        tempFile.setReadOnly (false);
        expect (tempFile.hasWriteAccess());

        Time t (Time::getCurrentTime());
        tempFile.setLastModificationTime (t);
        Time t2 = tempFile.getLastModificationTime();
        expect (std::abs ((int) (t2.toMilliseconds() - t.toMilliseconds())) <= 1000);

        {
            MemoryBlock mb;
            tempFile.loadFileAsData (mb);
            expect (mb.getSize() == 10);
            expect (mb[0] == '0');
        }

        {
            expect (tempFile.getSize() == 10);
            FileOutputStream fo (tempFile);
            expect (fo.openedOk());

            expect (fo.setPosition  (7));
            expect (fo.truncate().wasOk());
            expect (tempFile.getSize() == 7);
            fo.write ("789", 3);
            fo.flush();
            expect (tempFile.getSize() == 10);
        }

        beginTest ("Memory-mapped files");

        {
            MemoryMappedFile mmf (tempFile, MemoryMappedFile::readOnly);
            expect (mmf.getSize() == 10);
            expect (mmf.getData() != nullptr);
            expect (memcmp (mmf.getData(), "0123456789", 10) == 0);
        }

        {
            const File tempFile2 (tempFile.getNonexistentSibling (false));
            expect (tempFile2.create());
            expect (tempFile2.appendData ("xxxxxxxxxx", 10));

            {
                MemoryMappedFile mmf (tempFile2, MemoryMappedFile::readWrite);
                expect (mmf.getSize() == 10);
                expect (mmf.getData() != nullptr);
                memcpy (mmf.getData(), "abcdefghij", 10);
            }

            {
                MemoryMappedFile mmf (tempFile2, MemoryMappedFile::readWrite);
                expect (mmf.getSize() == 10);
                expect (mmf.getData() != nullptr);
                expect (memcmp (mmf.getData(), "abcdefghij", 10) == 0);
            }

            expect (tempFile2.deleteFile());
        }

        beginTest ("More writing");

        expect (tempFile.appendData ("abcdefghij", 10));
        expect (tempFile.getSize() == 20);
        expect (tempFile.replaceWithData ("abcdefghij", 10));
        expect (tempFile.getSize() == 10);

        File tempFile2 (tempFile.getNonexistentSibling (false));
        expect (tempFile.copyFileTo (tempFile2));
        expect (tempFile2.exists());
        expect (tempFile2.hasIdenticalContentTo (tempFile));
        expect (tempFile.deleteFile());
        expect (! tempFile.exists());
        expect (tempFile2.moveFileTo (tempFile));
        expect (tempFile.exists());
        expect (! tempFile2.exists());

        expect (demoFolder.deleteRecursively());
        expect (! demoFolder.exists());
    }
};

static FileTests fileUnitTests;

#endif


