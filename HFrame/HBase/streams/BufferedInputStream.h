

//==============================================================================
/** Wraps another input stream, and reads from it using an intermediate buffer

    If you're using an input stream such as a file input stream, and making lots of
    small read accesses to it, it's probably sensible to wrap it in one of these,
    so that the source stream gets accessed in larger chunk sizes, meaning less
    work for the underlying stream.

    @tags{Core}
*/
class API BufferedInputStream  : public InputStream
{
public:
    //==============================================================================
    /** Creates a BufferedInputStream from an input source.

        @param sourceStream                 the source stream to read from
        @param bufferSize                   the size of reservoir to use to buffer the source
        @param deleteSourceWhenDestroyed    whether the sourceStream that is passed in should be
                                            deleted by this object when it is itself deleted.
    */
    BufferedInputStream (InputStream* sourceStream,
                         int bufferSize,
                         bool deleteSourceWhenDestroyed);

    /** Creates a BufferedInputStream from an input source.

        @param sourceStream     the source stream to read from - the source stream  must not
                                be deleted until this object has been destroyed.
        @param bufferSize       the size of reservoir to use to buffer the source
    */
    BufferedInputStream (InputStream& sourceStream, int bufferSize);

    /** Destructor.

        This may also delete the source stream, if that option was chosen when the
        buffered stream was created.
    */
    ~BufferedInputStream() override;


    //==============================================================================
    /** Returns the next byte that would be read by a call to readByte() */
    char peekByte();

    int64 getTotalLength() override;
    int64 getPosition() override;
    bool setPosition (int64 newPosition) override;
    int read (void* destBuffer, int maxBytesToRead) override;
    String readString() override;
    bool isExhausted() override;


private:
    //==============================================================================
    OptionalScopedPointer<InputStream> source;
    int bufferSize;
    int64 position, lastReadPos = 0, bufferStart, bufferOverlap = 128;
    HeapBlock<char> buffer;
    bool ensureBuffered();

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BufferedInputStream)
};

