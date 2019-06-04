

//==============================================================================
/**
    A type of InputSource that represents a URL.

    @see InputSource

    @tags{Core}
*/
class API URLInputSource     : public InputSource
{
public:
    //==============================================================================
    /** Creates a URLInputSource for a url. */
    URLInputSource (const URL& url);

    /** Move constructor which will move the URL into the InputSource.

        This is useful when the url carries any security credentials.
    */
    URLInputSource (URL&& url);

    /** Destructor. */
    ~URLInputSource() override;

    InputStream* createInputStream() override;
    InputStream* createInputStreamFor (const String& relatedItemPath) override;
    int64 hashCode() const override;

private:
    //==============================================================================
    const URL u;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (URLInputSource)
};
