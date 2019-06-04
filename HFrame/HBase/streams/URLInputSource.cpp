

URLInputSource::URLInputSource (const URL& url)
    : u (url)
{
}

URLInputSource::URLInputSource (URL&& url)
    : u (std::move (url))
{
}

URLInputSource::~URLInputSource()
{
}

InputStream* URLInputSource::createInputStream()
{
    return u.createInputStream (false);
}

InputStream* URLInputSource::createInputStreamFor (const String& relatedItemPath)
{
    auto sub = u.getSubPath();
    auto parent = sub.containsChar (L'/') ? sub.upToLastOccurrenceOf ("/", false, false)
                                          : String ();

    return u.withNewSubPath (parent).getChildURL (relatedItemPath).createInputStream (false);
}

int64 URLInputSource::hashCode() const
{
    return u.toString (true).hashCode64();
}


