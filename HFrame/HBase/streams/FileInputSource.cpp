

FileInputSource::FileInputSource (const File& f, bool useFileTimeInHash)
    : file (f), useFileTimeInHashGeneration (useFileTimeInHash)
{
}

FileInputSource::~FileInputSource()
{
}

InputStream* FileInputSource::createInputStream()
{
    return file.createInputStream();
}

InputStream* FileInputSource::createInputStreamFor (const String& relatedItemPath)
{
    return file.getSiblingFile (relatedItemPath).createInputStream();
}

int64 FileInputSource::hashCode() const
{
    int64 h = file.hashCode();

    if (useFileTimeInHashGeneration)
        h ^= file.getLastModificationTime().toMilliseconds();

    return h;
}


