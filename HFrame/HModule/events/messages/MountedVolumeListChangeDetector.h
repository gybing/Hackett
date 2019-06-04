

#if HMAC || HWINDOWS || defined (DOXYGEN)

//==============================================================================
/**
    An instance of this class will provide callbacks when drives are
    mounted or unmounted on the system.

    Just inherit from this class and implement the pure virtual method
    to get the callbacks, there's no need to do anything else.

    @see File::findFileSystemRoots()

    @tags{Events}
*/
class API MountedVolumeListChangeDetector
{
public:
    MountedVolumeListChangeDetector();
    virtual ~MountedVolumeListChangeDetector();

    /** This method is called when a volume is mounted or unmounted. */
    virtual void mountedVolumeListChanged() = 0;

private:
    HPUBLIC_IN_DLL_BUILD (struct Pimpl)
    std::unique_ptr<Pimpl> pimpl;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MountedVolumeListChangeDetector)
};

#endif


