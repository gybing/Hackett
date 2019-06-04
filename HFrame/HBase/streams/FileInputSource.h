

//==============================================================================
/**
    A type of InputSource that represents a normal file.

    @see InputSource

    @tags{Core}
*/
class API FileInputSource     : public InputSource
{
public:
    //==============================================================================
    /** Creates a FileInputSource for a file.
        If the useFileTimeInHashGeneration parameter is true, then this object's
        hashCode() method will incorporate the file time into its hash code; if
        false, only the file name will be used for the hash.
    */
    FileInputSource (const File& file, bool useFileTimeInHashGeneration = false);

    /** Destructor. */
    ~FileInputSource() override;

    InputStream* createInputStream() override;
    InputStream* createInputStreamFor (const String& relatedItemPath) override;
    int64 hashCode() const override;

private:
    //==============================================================================
    const File file;
    bool useFileTimeInHashGeneration;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileInputSource)
};
