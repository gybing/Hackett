

//==============================================================================
/**
    An input stream that reads from a local file.

    @see InputStream, FileOutputStream, File::createInputStream

    @tags{Core}
*/
class API FileInputStream  : public InputStream
{
public:
    //==============================================================================
    /** Creates a FileInputStream to read from the given file.

        After creating a FileInputStream, you should use openedOk() or failedToOpen()
        to make sure that it's OK before trying to read from it! If it failed, you
        can call getStatus() to get more error information.
    */
    explicit FileInputStream (const File& fileToRead);

    /** Destructor. */
    ~FileInputStream() override;

    //==============================================================================
    /** Returns the file that this stream is reading from. */
    const File& getFile() const noexcept                { return file; }

    /** Returns the status of the file stream.
        The result will be ok if the file opened successfully. If an error occurs while
        opening or reading from the file, this will contain an error message.
    */
    const Result& getStatus() const noexcept            { return status; }

    /** Returns true if the stream couldn't be opened for some reason.
        @see getResult()
    */
    bool failedToOpen() const noexcept                  { return status.failed(); }

    /** Returns true if the stream opened without problems.
        @see getResult()
    */
    bool openedOk() const noexcept                      { return status.wasOk(); }


    //==============================================================================
    int64 getTotalLength() override;
    int read (void*, int) override;
    bool isExhausted() override;
    int64 getPosition() override;
    bool setPosition (int64) override;

private:
    //==============================================================================
    const File file;
    void* fileHandle = nullptr;
    int64 currentPosition = 0;
    Result status { Result::ok() };

    void openHandle();
    size_t readInternal (void*, size_t);

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileInputStream)
};


