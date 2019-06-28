

//==============================================================================
/**
    Classes derived from this will be automatically deleted when the application exits.

    After HApplicationBase::shutdown() has been called, any objects derived from
    DeletedAtShutdown which are still in existence will be deleted in the reverse
    order to that in which they were created.

    So if you've got a singleton and don't want to have to explicitly delete it, just
    inherit from this and it'll be taken care of.

    @tags{Events}
*/
class API DeletedAtShutdown
{
protected:
    /** Creates a DeletedAtShutdown object. */
    DeletedAtShutdown();

    /** Destructor.

        It's ok to delete these objects explicitly - it's only the ones left
        dangling at the end that will be deleted automatically.
    */
    virtual ~DeletedAtShutdown();


public:
    /** Deletes all extant objects.

        This shouldn't be used by applications, as it's called automatically
        in the shutdown code of the HApplicationBase class.
    */
    static void deleteAll();

private:
    HDECLARE_NON_COPYABLE (DeletedAtShutdown)
};


