

//==============================================================================
/**
    A stream which uses zlib to compress the data written into it.

    Important note: When you call flush() on a GZIPCompressorOutputStream,
    the gzip data is closed - this means that no more data can be written to
    it, and any subsequent attempts to call write() will cause an assertion.

    @see GZIPDecompressorInputStream

    @tags{Core}
*/
class API GZIPCompressorOutputStream  : public OutputStream
{
public:
    //==============================================================================
    /** Creates a compression stream.
        @param destStream                       the stream into which the compressed data will be written
        @param compressionLevel                 how much to compress the data, between 0 and 9, where
                                                0 is non-compressed storage, 1 is the fastest/lowest compression,
                                                and 9 is the slowest/highest compression. Any value outside this range
                                                indicates that a default compression level should be used.
        @param windowBits                       this is used internally to change the window size used
                                                by zlib - leave it as 0 unless you specifically need to set
                                                its value for some reason
    */
    GZIPCompressorOutputStream (OutputStream& destStream,
                                int compressionLevel = -1,
                                int windowBits = 0);

    /** Creates a compression stream.
        @param destStream                       the stream into which the compressed data will be written.
                                                Ownership of this object depends on the value of deleteDestStreamWhenDestroyed
        @param compressionLevel                 how much to compress the data, between 0 and 9, where
                                                0 is non-compressed storage, 1 is the fastest/lowest compression,
                                                and 9 is the slowest/highest compression. Any value outside this range
                                                indicates that a default compression level should be used.
        @param deleteDestStreamWhenDestroyed    whether or not the GZIPCompressorOutputStream will delete the
                                                destStream object when it is destroyed
        @param windowBits                       this is used internally to change the window size used
                                                by zlib - leave it as 0 unless you specifically need to set
                                                its value for some reason
    */
    GZIPCompressorOutputStream (OutputStream* destStream,
                                int compressionLevel = -1,
                                bool deleteDestStreamWhenDestroyed = false,
                                int windowBits = 0);

    /** Destructor. */
    ~GZIPCompressorOutputStream() override;

    //==============================================================================
    /** Flushes and closes the stream.
        Note that unlike most streams, when you call flush() on a GZIPCompressorOutputStream,
        the stream is closed - this means that no more data can be written to it, and any
        subsequent attempts to call write() will cause an assertion.
    */
    void flush() override;

    int64 getPosition() override;
    bool setPosition (int64) override;
    bool write (const void*, size_t) override;

    /** These are preset values that can be used for the constructor's windowBits parameter.
        For more info about this, see the zlib documentation for its windowBits parameter.
    */
    enum WindowBitsValues
    {
        windowBitsRaw = -15,
        windowBitsGZIP = 15 + 16
    };

private:
    //==============================================================================
    OptionalScopedPointer<OutputStream> destStream;

    class GZIPCompressorHelper;
    std::unique_ptr<GZIPCompressorHelper> helper;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GZIPCompressorOutputStream)
};


