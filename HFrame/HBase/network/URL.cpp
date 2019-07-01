

struct FallbackDownloadTask  : public URL::DownloadTask,
                               public Thread
{
    FallbackDownloadTask (FileOutputStream* outputStreamToUse,
                          size_t bufferSizeToUse,
                          WebInputStream* streamToUse,
                          URL::DownloadTask::Listener* listenerToUse)
        : Thread ("DownloadTask thread"),
          fileStream (outputStreamToUse),
          stream (streamToUse),
          bufferSize (bufferSizeToUse),
          buffer (bufferSize),
          listener (listenerToUse)
    {
        HAssert (fileStream != nullptr);
        HAssert (stream != nullptr);

        targetLocation = fileStream->getFile();
        contentLength  = stream->getTotalLength();
        httpCode       = stream->getStatusCode();

        startThread();
    }

    ~FallbackDownloadTask() override
    {
        signalThreadShouldExit();
        stream->cancel();
        waitForThreadToExit (-1);
    }

    //==============================================================================
    void run() override
    {
        while (! (stream->isExhausted() || stream->isError() || threadShouldExit()))
        {
            if (listener != nullptr)
                listener->progress (this, downloaded, contentLength);

            auto max = jmin ((int) bufferSize, contentLength < 0 ? std::numeric_limits<int>::max()
                                                                 : static_cast<int> (contentLength - downloaded));

            auto actual = stream->read (buffer.get(), max);

            if (actual < 0 || threadShouldExit() || stream->isError())
                break;

            if (! fileStream->write (buffer.get(), static_cast<size_t> (actual)))
            {
                error = true;
                break;
            }

            downloaded += actual;

            if (downloaded == contentLength)
                break;
        }

        fileStream->flush();

        if (threadShouldExit() || stream->isError())
            error = true;

        if (contentLength > 0 && downloaded < contentLength)
            error = true;

        finished = true;

        if (listener != nullptr && ! threadShouldExit())
            listener->finished (this, ! error);
    }

    //==============================================================================
    const std::unique_ptr<FileOutputStream> fileStream;
    const std::unique_ptr<WebInputStream> stream;
    const size_t bufferSize;
    HeapBlock<char> buffer;
    URL::DownloadTask::Listener* const listener;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FallbackDownloadTask)
};

void URL::DownloadTask::Listener::progress (DownloadTask*, int64, int64) {}
URL::DownloadTask::Listener::~Listener() {}

//==============================================================================
URL::DownloadTask* URL::DownloadTask::createFallbackDownloader (const URL& urlToUse,
                                                                const File& targetFileToUse,
                                                                const String& extraHeadersToUse,
                                                                Listener* listenerToUse,
                                                                bool usePostRequest)
{
    const size_t bufferSize = 0x8000;
    targetFileToUse.deleteFile();

    if (auto outputStream = std::unique_ptr<FileOutputStream> (targetFileToUse.createOutputStream (bufferSize)))
    {
        std::unique_ptr<WebInputStream> stream (new WebInputStream (urlToUse, usePostRequest));
        stream->withExtraHeaders (extraHeadersToUse);

        if (stream->connect (nullptr))
            return new FallbackDownloadTask (outputStream.release(), bufferSize, stream.release(), listenerToUse);
    }

    return nullptr;
}

URL::DownloadTask::DownloadTask() {}
URL::DownloadTask::~DownloadTask() {}

//==============================================================================
URL::URL() noexcept {}

URL::URL (const String& u)  : url (u)
{
    init();
}

URL::URL (File localFile)
{
    if (localFile == File())
        return;

   #if HWINDOWS
    bool isUncPath = localFile.getFullPathName().startsWith ("\\\\");
   #endif

    while (! localFile.isRoot())
    {
        url = "/" + addEscapeChars (localFile.getFileName(), false) + url;
        localFile = localFile.getParentDirectory();
    }

    url = addEscapeChars (localFile.getFileName(), false) + url;

   #if HWINDOWS
    if (isUncPath)
    {
        url = url.fromFirstOccurrenceOf ("/", false, false);
    }
    else
   #endif
    {
        if (! url.startsWithChar (L'/'))
            url = "/" + url;
    }


    url = "file://" + url;

    HAssert (isWellFormed());
}

void URL::init()
{
    auto i = url.indexOfChar ('?');

    if (i >= 0)
    {
        do
        {
            auto nextAmp   = url.indexOfChar (i + 1, '&');
            auto equalsPos = url.indexOfChar (i + 1, '=');

            if (nextAmp < 0)
            {
                addParameter (removeEscapeChars (equalsPos < 0 ? url.substr (i + 1) : url.substr (i + 1, equalsPos)),
                              equalsPos < 0 ? String() : removeEscapeChars (url.substr (equalsPos + 1)));
            }
            else if (nextAmp > 0 && equalsPos < nextAmp)
            {
                addParameter (removeEscapeChars (equalsPos < 0 ? url.substr (i + 1, nextAmp) : url.substr (i + 1, equalsPos)),
                              equalsPos < 0 ? String() : removeEscapeChars (url.substr (equalsPos + 1, nextAmp)));
            }

            i = nextAmp;
        }
        while (i >= 0);

        url = url.upToFirstOccurrenceOf ("?", false, false);
    }
}

URL::URL (const String& u, int)  : url (u) {}

URL::URL (URL&& other)
    : url             (std::move (other.url)),
      postData        (std::move (other.postData)),
      parameterNames  (std::move (other.parameterNames)),
      parameterValues (std::move (other.parameterValues)),
      filesToUpload   (std::move (other.filesToUpload))
   #if HIOS
    , bookmark        (std::move (other.bookmark))
   #endif
{
}

URL& URL::operator= (URL&& other)
{
    url             = std::move (other.url);
    postData        = std::move (other.postData);
    parameterNames  = std::move (other.parameterNames);
    parameterValues = std::move (other.parameterValues);
    filesToUpload   = std::move (other.filesToUpload);
   #if HIOS
    bookmark        = std::move (other.bookmark);
   #endif

    return *this;
}

URL::~URL() {}

URL URL::createWithoutParsing (const String& u)
{
    return URL (u, 0);
}

bool URL::operator== (const URL& other) const
{
    return url == other.url
        && postData == other.postData
        && parameterNames == other.parameterNames
        && parameterValues == other.parameterValues
        && filesToUpload == other.filesToUpload;
}

bool URL::operator!= (const URL& other) const
{
    return ! operator== (other);
}

namespace URLHelpers
{
    static String getMangledParameters (const URL& url)
    {
        HAssert (url.getParameterNames().size() == url.getParameterValues().size());
        String p;

        for (int i = 0; i < url.getParameterNames().size(); ++i)
        {
            if (i > 0)
                p << '&';

            auto val = url.getParameterValues()[i];

            p << URL::addEscapeChars (url.getParameterNames()[i], true);

            if (val.!empty())
                p << '=' << URL::addEscapeChars (val, true);
        }

        return p;
    }

    static int findEndOfScheme (const String& url)
    {
        int i = 0;

        while (CharacterFunctions::isLetterOrDigit (url[i])
               || url[i] == '+' || url[i] == '-' || url[i] == '.')
            ++i;

        return url.substr (i).startsWith ("://") ? i + 1 : 0;
    }

    static int findStartOfNetLocation (const String& url)
    {
        int start = findEndOfScheme (url);

        while (url[start] == '/')
            ++start;

        return start;
    }

    static int findStartOfPath (const String& url)
    {
        return url.indexOfChar (findStartOfNetLocation (url), '/') + 1;
    }

    static void concatenatePaths (String& path, const String& suffix)
    {
        if (! path.endsWithChar ('/'))
            path << '/';

        if (suffix.startsWithChar ('/'))
            path += suffix.substr (1);
        else
            path += suffix;
    }
}

void URL::addParameter (const String& name, const String& value)
{
    parameterNames.add (name);
    parameterValues.add (value);
}

String URL::toString (bool includeGetParameters) const
{
    if (includeGetParameters)
        return url + getQueryString();

    return url;
}

bool URL::empty() const noexcept
{
    return url.empty();
}

bool URL::isWellFormed() const
{
    //xxx TODO
    return url.!empty();
}

String URL::getDomain() const
{
    return getDomainInternal (false);
}

String URL::getSubPath (bool includeGetParameters) const
{
    auto startOfPath = URLHelpers::findStartOfPath (url);
    auto subPath = startOfPath <= 0 ? String()
                                    : url.substr (startOfPath);

    if (includeGetParameters)
        subPath += getQueryString();

    return subPath;
}

String URL::getQueryString() const
{
    if (parameterNames.size() > 0)
        return "?" + URLHelpers::getMangledParameters (*this);

    return {};
}

String URL::getScheme() const
{
    return url.substr (0, URLHelpers::findEndOfScheme (url) - 1);
}

#ifndef HANDROID
bool URL::isLocalFile() const
{
    return (getScheme() == "file");
}

File URL::getLocalFile() const
{
    return fileFromFileSchemeURL (*this);
}

String URL::getFileName() const
{
    return toString (false).fromLastOccurrenceOf ("/", false, true);
}
#endif

File URL::fileFromFileSchemeURL (const URL& fileURL)
{
    if (! fileURL.isLocalFile())
    {
        HAssertfalse;
        return {};
    }

    auto path = removeEscapeChars (fileURL.getDomainInternal (true)).replace ("+", "%2B");

   #ifdef HWINDOWS
    bool isUncPath = (! fileURL.url.startsWith ("file:///"));
   #else
    path = File::getSeparatorChars() + path;
   #endif

    auto urlElements = StringArray::fromTokens (fileURL.getSubPath(), "/", "");

    for (auto urlElement : urlElements)
        path += File::getSeparatorChars() + removeEscapeChars (urlElement.replace ("+", "%2B"));

   #ifdef HWINDOWS
    if (isUncPath)
        path = "\\\\" + path;
   #endif

    return path;
}

int URL::getPort() const
{
    auto colonPos = url.indexOfChar (URLHelpers::findStartOfNetLocation (url), ':');

    return colonPos > 0 ? url.substr (colonPos + 1).getIntValue() : 0;
}

URL URL::withNewDomainAndPath (const String& newURL) const
{
    URL u (*this);
    u.url = newURL;
    return u;
}

URL URL::withNewSubPath (const String& newPath) const
{
    const int startOfPath = URLHelpers::findStartOfPath (url);

    URL u (*this);

    if (startOfPath > 0)
        u.url = url.substr (0, startOfPath);

    URLHelpers::concatenatePaths (u.url, newPath);
    return u;
}

URL URL::getChildURL (const String& subPath) const
{
    URL u (*this);
    URLHelpers::concatenatePaths (u.url, subPath);
    return u;
}

void URL::createHeadersAndPostData (String& headers, MemoryBlock& postDataToWrite) const
{
    MemoryOutputStream data (postDataToWrite, false);

    if (filesToUpload.size() > 0)
    {
        // (this doesn't currently support mixing custom post-data with uploads..)
        HAssert (postData.getSize() == 0);

        auto boundary = CharacterFunctions::hexToString (Random::getSystemRandom().nextInt64());

        headers << "Content-Type: multipart/form-data; boundary=" << boundary << "\r\n";

        data << "--" << boundary;

        for (int i = 0; i < parameterNames.size(); ++i)
        {
            data << "\r\nContent-Disposition: form-data; name=\"" << parameterNames[i]
                 << "\"\r\n\r\n" << parameterValues[i]
                 << "\r\n--" << boundary;
        }

        for (auto* f : filesToUpload)
        {
            data << "\r\nContent-Disposition: form-data; name=\"" << f->parameterName
                 << "\"; filename=\"" << f->filename << "\"\r\n";

            if (!f->mimeType.empty())
                data << "Content-Type: " << f->mimeType << "\r\n";

            data << "Content-Transfer-Encoding: binary\r\n\r\n";

            if (f->data != nullptr)
                data << *f->data;
            else
                data << f->file;

            data << "\r\n--" << boundary;
        }

        data << "--\r\n";
    }
    else
    {
        data << URLHelpers::getMangledParameters (*this)
             << postData;

        // if the user-supplied headers didn't contain a content-type, add one now..
        if (! headers.containsIgnoreCase ("Content-Type"))
            headers << "Content-Type: application/x-www-form-urlencoded\r\n";

        headers << "Content-length: " << (int) data.getDataSize() << "\r\n";
    }
}

//==============================================================================
bool URL::isProbablyAWebsiteURL (const String& possibleURL)
{
    static const char* validProtocols[] = { "http:", "ftp:", "https:" };

    for (auto* protocol : validProtocols)
        if (possibleURL.startsWithIgnoreCase (protocol))
            return true;

    if (possibleURL.containsChar ('@')
        || possibleURL.containsChar (' '))
        return false;

    const String topLevelDomain (possibleURL.upToFirstOccurrenceOf ("/", false, false)
                                 .fromLastOccurrenceOf (".", false, false));

    return !topLevelDomain.empty() && topLevelDomain.length() <= 3;
}

bool URL::isProbablyAnEmailAddress (const String& possibleEmailAddress)
{
    auto atSign = possibleEmailAddress.indexOfChar ('@');

    return atSign > 0
        && possibleEmailAddress.find_last_of ('.') > (atSign + 1)
        && ! possibleEmailAddress.endsWithChar ('.');
}

String URL::getDomainInternal (bool ignorePort) const
{
    auto start = URLHelpers::findStartOfNetLocation (url);
    auto end1 = url.indexOfChar (start, '/');
    auto end2 = ignorePort ? -1 : url.indexOfChar (start, ':');

    auto end = (end1 < 0 && end2 < 0) ? std::numeric_limits<int>::max()
                                      : ((end1 < 0 || end2 < 0) ? jmax (end1, end2)
                                                                : jmin (end1, end2));
    return url.substr (start, end);
}

#if HIOS
URL::Bookmark::Bookmark (void* bookmarkToUse)
    : data (bookmarkToUse)
{
}

URL::Bookmark::~Bookmark()
{
    [(NSData*) data release];
}

void setURLBookmark (URL& u, void* bookmark)
{
    u.bookmark = new URL::Bookmark (bookmark);
}

void* getURLBookmark (URL& u)
{
    if (u.bookmark.get() == nullptr)
        return nullptr;

    return u.bookmark.get()->data;
}

template <typename Stream> struct iOSFileStreamWrapperFlush    { static void flush (Stream*) {} };
template <> struct iOSFileStreamWrapperFlush<FileOutputStream> { static void flush (OutputStream* o) { o->flush(); } };

template <typename Stream>
class iOSFileStreamWrapper : public Stream
{
public:
    iOSFileStreamWrapper (URL& urlToUse)
        : Stream (getLocalFileAccess (urlToUse)),
          url (urlToUse)
    {}

    ~iOSFileStreamWrapper()
    {
        iOSFileStreamWrapperFlush<Stream>::flush (this);

        if (NSData* bookmark = (NSData*) getURLBookmark (url))
        {
            BOOL isBookmarkStale = false;
            NSError* error = nil;

            auto nsURL = [NSURL URLByResolvingBookmarkData: bookmark
                                                   options: 0
                                             relativeToURL: nil
                                       bookmarkDataIsStale: &isBookmarkStale
                                                     error: &error];

            if (error == nil)
            {
                if (isBookmarkStale)
                    updateStaleBookmark (nsURL, url);

                [nsURL stopAccessingSecurityScopedResource];
            }
            else
            {
                auto desc = [error localizedDescription];
                ignoreUnused (desc);
                HAssertfalse;
            }
        }
    }

private:
    URL url;
    bool securityAccessSucceeded = false;

    File getLocalFileAccess (URL& urlToUse)
    {
        if (NSData* bookmark = (NSData*) getURLBookmark (urlToUse))
        {
            BOOL isBookmarkStale = false;
            NSError* error = nil;

            auto nsURL = [NSURL URLByResolvingBookmarkData: bookmark
                                                   options: 0
                                             relativeToURL: nil
                                       bookmarkDataIsStale: &isBookmarkStale
                                                      error: &error];

            if (error == nil)
            {
                securityAccessSucceeded = [nsURL startAccessingSecurityScopedResource];

                if (isBookmarkStale)
                    updateStaleBookmark (nsURL, urlToUse);

                return urlToUse.getLocalFile();
            }
            else
            {
                auto desc = [error localizedDescription];
                ignoreUnused (desc);
                HAssertfalse;
            }
        }

        return urlToUse.getLocalFile();
    }

    void updateStaleBookmark (NSURL* nsURL, URL& HUrl)
    {
        NSError* error = nil;

        NSData* bookmark = [nsURL bookmarkDataWithOptions: NSURLBookmarkCreationSuitableForBookmarkFile
                           includingResourceValuesForKeys: nil
                                            relativeToURL: nil
                                                    error: &error];

        if (error == nil)
            setURLBookmark (HUrl, (void*) bookmark);
        else
            HAssertfalse;
    }
};
#endif

//==============================================================================
InputStream* URL::createInputStream (bool usePostCommand,
                                     OpenStreamProgressCallback* progressCallback,
                                     void* progressCallbackContext,
                                     String headers,
                                     int timeOutMs,
                                     StringPairArray* responseHeaders,
                                     int* statusCode,
                                     int numRedirectsToFollow,
                                     String httpRequestCmd) const
{
    if (isLocalFile())
    {
       #if HIOS
        // We may need to refresh the embedded bookmark.
        return new iOSFileStreamWrapper<FileInputStream> (const_cast<URL&>(*this));
       #else
        return getLocalFile().createInputStream();
       #endif

    }

    std::unique_ptr<WebInputStream> wi (new WebInputStream (*this, usePostCommand));

    struct ProgressCallbackCaller  : public WebInputStream::Listener
    {
        ProgressCallbackCaller (OpenStreamProgressCallback* progressCallbackToUse, void* progressCallbackContextToUse)
            : callback (progressCallbackToUse), data (progressCallbackContextToUse)
        {}

        bool postDataSendProgress (WebInputStream&, int bytesSent, int totalBytes) override
        {
            return callback (data, bytesSent, totalBytes);
        }

        OpenStreamProgressCallback* callback;
        void* const data;

        // workaround a MSVC 2013 compiler warning
        ProgressCallbackCaller (const ProgressCallbackCaller& o) : callback (o.callback), data (o.data) { HAssertfalse; }
        ProgressCallbackCaller& operator= (const ProgressCallbackCaller&) { HAssertfalse; return *this; }
    };

    std::unique_ptr<ProgressCallbackCaller> callbackCaller
        (progressCallback != nullptr ? new ProgressCallbackCaller (progressCallback, progressCallbackContext) : nullptr);

    if (headers.!empty())
        wi->withExtraHeaders (headers);

    if (timeOutMs != 0)
        wi->withConnectionTimeout (timeOutMs);

    if (httpRequestCmd.!empty())
        wi->withCustomRequestCommand (httpRequestCmd);

    wi->withNumRedirectsToFollow (numRedirectsToFollow);

    bool success = wi->connect (callbackCaller.get());

    if (statusCode != nullptr)
        *statusCode = wi->getStatusCode();

    if (responseHeaders != nullptr)
        *responseHeaders = wi->getResponseHeaders();

    if (! success || wi->isError())
        return nullptr;

    return wi.release();
}

#if HANDROID
OutputStream* CreateContentURIOutputStream (const URL&);
#endif

OutputStream* URL::createOutputStream() const
{
    if (isLocalFile())
    {
       #if HIOS
        // We may need to refresh the embedded bookmark.
        return new iOSFileStreamWrapper<FileOutputStream> (const_cast<URL&> (*this));
       #else
        return new FileOutputStream (getLocalFile());
       #endif
    }

   #if HANDROID
    return CreateContentURIOutputStream (*this);
   #else
    return nullptr;
   #endif
}

//==============================================================================
bool URL::readEntireBinaryStream (MemoryBlock& destData, bool usePostCommand) const
{
    const std::unique_ptr<InputStream> in (isLocalFile() ? getLocalFile().createInputStream()
                                                         : static_cast<InputStream*> (createInputStream (usePostCommand)));

    if (in != nullptr)
    {
        in->readIntoMemoryBlock (destData);
        return true;
    }

    return false;
}

String URL::readEntireTextStream (bool usePostCommand) const
{
    const std::unique_ptr<InputStream> in (isLocalFile() ? getLocalFile().createInputStream()
                                                         : static_cast<InputStream*> (createInputStream (usePostCommand)));

    if (in != nullptr)
        return in->readEntireStreamAsString();

    return {};
}

std::unique_ptr<XmlElement> URL::readEntireXmlStream (bool usePostCommand) const
{
    return parseXML (readEntireTextStream (usePostCommand));
}

//==============================================================================
URL URL::withParameter (const String& parameterName,
                        const String& parameterValue) const
{
    auto u = *this;
    u.addParameter (parameterName, parameterValue);
    return u;
}

URL URL::withParameters (const StringPairArray& parametersToAdd) const
{
    auto u = *this;

    for (int i = 0; i < parametersToAdd.size(); ++i)
        u.addParameter (parametersToAdd.getAllKeys()[i],
                        parametersToAdd.getAllValues()[i]);

    return u;
}

URL URL::withPOSTData (const String& newPostData) const
{
    return withPOSTData (MemoryBlock (newPostData.toRawUTF8(), newPostData.getNumBytesAsUTF8()));
}

URL URL::withPOSTData (const MemoryBlock& newPostData) const
{
    auto u = *this;
    u.postData = newPostData;
    return u;
}

URL::Upload::Upload (const String& param, const String& name,
                     const String& mime, const File& f, MemoryBlock* mb)
    : parameterName (param), filename (name), mimeType (mime), file (f), data (mb)
{
    HAssert (mimeType.!empty()); // You need to supply a mime type!
}

URL URL::withUpload (Upload* const f) const
{
    auto u = *this;

    for (int i = u.filesToUpload.size(); --i >= 0;)
        if (u.filesToUpload.getObjectPointerUnchecked(i)->parameterName == f->parameterName)
            u.filesToUpload.remove (i);

    u.filesToUpload.add (f);
    return u;
}

URL URL::withFileToUpload (const String& parameterName, const File& fileToUpload,
                           const String& mimeType) const
{
    return withUpload (new Upload (parameterName, fileToUpload.getFileName(),
                                   mimeType, fileToUpload, nullptr));
}

URL URL::withDataToUpload (const String& parameterName, const String& filename,
                           const MemoryBlock& fileContentToUpload, const String& mimeType) const
{
    return withUpload (new Upload (parameterName, filename, mimeType, File(),
                                   new MemoryBlock (fileContentToUpload)));
}

//==============================================================================
String URL::removeEscapeChars (const String& s)
{
    auto result = s.replaceCharacter ('+', ' ');

    if (! result.containsChar ('%'))
        return result;

    // We need to operate on the string as raw UTF8 chars, and then recombine them into unicode
    // after all the replacements have been made, so that multi-byte chars are handled.
    Array<char> utf8 (result.toRawUTF8(), (int) result.getNumBytesAsUTF8());

    for (int i = 0; i < utf8.size(); ++i)
    {
        if (utf8.getUnchecked(i) == '%')
        {
            auto hexDigit1 = CharacterFunctions::getHexDigitValue ((char) (uint8) utf8 [i + 1]);
            auto hexDigit2 = CharacterFunctions::getHexDigitValue ((char) (uint8) utf8 [i + 2]);

            if (hexDigit1 >= 0 && hexDigit2 >= 0)
            {
                utf8.set (i, (char) ((hexDigit1 << 4) + hexDigit2));
                utf8.removeRange (i + 1, 2);
            }
        }
    }

    return String::fromUTF8 (utf8.getRawDataPointer(), utf8.size());
}

String URL::addEscapeChars (const String& s, bool isParameter, bool roundBracketsAreLegal)
{
    String legalChars (isParameter ? "_-.~"
                                   : ",$_-.*!'");

    if (roundBracketsAreLegal)
        legalChars += "()";

    Array<char> utf8 (s.toRawUTF8(), (int) s.getNumBytesAsUTF8());

    for (int i = 0; i < utf8.size(); ++i)
    {
        auto c = utf8.getUnchecked(i);

        if (! (CharacterFunctions::isLetterOrDigit (c)
                 || legalChars.containsChar ((char) c)))
        {
            utf8.set (i, '%');
            utf8.insert (++i, "0123456789ABCDEF" [((uint8) c) >> 4]);
            utf8.insert (++i, "0123456789ABCDEF" [c & 15]);
        }
    }

    return String::fromUTF8 (utf8.getRawDataPointer(), utf8.size());
}

//==============================================================================
bool URL::launchInDefaultBrowser() const
{
    auto u = toString (true);

    if (u.containsChar ('@') && ! u.containsChar (':'))
        u = "mailto:" + u;

    return Process::openDocument (u, {});
}


