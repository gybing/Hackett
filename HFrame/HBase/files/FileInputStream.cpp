

int64 fileSetPosition (void* handle, int64 pos);


//==============================================================================
FileInputStream::FileInputStream (const File& f)  : file (f)
{
    openHandle();
}

int64 FileInputStream::getTotalLength()
{
    // You should always check that a stream opened successfully before using it!
    HAssert (openedOk());

    return file.getSize();
}

int FileInputStream::read (void* buffer, int bytesToRead)
{
    // You should always check that a stream opened successfully before using it!
    HAssert (openedOk());

    // The buffer should never be null, and a negative size is probably a
    // sign that something is broken!
    HAssert (buffer != nullptr && bytesToRead >= 0);

    auto num = readInternal (buffer, (size_t) bytesToRead);
    currentPosition += (int64) num;

    return (int) num;
}

bool FileInputStream::isExhausted()
{
    return currentPosition >= getTotalLength();
}

int64 FileInputStream::getPosition()
{
    return currentPosition;
}

bool FileInputStream::setPosition (int64 pos)
{
    // You should always check that a stream opened successfully before using it!
    HAssert (openedOk());

    if (pos != currentPosition)
        currentPosition = fileSetPosition (fileHandle, pos);

    return currentPosition == pos;
}


//==============================================================================
//==============================================================================
#if HUNIT_TESTS

struct FileInputStreamTests   : public UnitTest
{
    FileInputStreamTests()
        : UnitTest ("FileInputStream", UnitTestCategories::streams)
    {}

    void runTest() override
    {
        const MemoryBlock data ("abcdefghijklmnopqrstuvwxyz", 26);
        File f (File::createTempFile (".txt"));
        f.appendData (data.getData(), data.getSize());
        FileInputStream stream (f);

        beginTest ("Read");

        expectEquals (stream.getPosition(), (int64) 0);
        expectEquals (stream.getTotalLength(), (int64) data.getSize());
        expectEquals (stream.getNumBytesRemaining(), stream.getTotalLength());
        expect (! stream.isExhausted());

        size_t numBytesRead = 0;
        MemoryBlock readBuffer (data.getSize());

        while (numBytesRead < data.getSize())
        {
            numBytesRead += (size_t) stream.read (&readBuffer[numBytesRead], 3);

            expectEquals (stream.getPosition(), (int64) numBytesRead);
            expectEquals (stream.getNumBytesRemaining(), (int64) (data.getSize() - numBytesRead));
            expect (stream.isExhausted() == (numBytesRead == data.getSize()));
        }

        expectEquals (stream.getPosition(), (int64) data.getSize());
        expectEquals (stream.getNumBytesRemaining(), (int64) 0);
        expect (stream.isExhausted());

        expect (readBuffer == data);

        beginTest ("Skip");

        stream.setPosition (0);
        expectEquals (stream.getPosition(), (int64) 0);
        expectEquals (stream.getTotalLength(), (int64) data.getSize());
        expectEquals (stream.getNumBytesRemaining(), stream.getTotalLength());
        expect (! stream.isExhausted());

        numBytesRead = 0;
        const int numBytesToSkip = 5;

        while (numBytesRead < data.getSize())
        {
            stream.skipNextBytes (numBytesToSkip);
            numBytesRead += numBytesToSkip;
            numBytesRead = std::min (numBytesRead, data.getSize());

            expectEquals (stream.getPosition(), (int64) numBytesRead);
            expectEquals (stream.getNumBytesRemaining(), (int64) (data.getSize() - numBytesRead));
            expect (stream.isExhausted() == (numBytesRead == data.getSize()));
        }

        expectEquals (stream.getPosition(), (int64) data.getSize());
        expectEquals (stream.getNumBytesRemaining(), (int64) 0);
        expect (stream.isExhausted());

        f.deleteFile();
    }
};

static FileInputStreamTests fileInputStreamTests;

#endif


