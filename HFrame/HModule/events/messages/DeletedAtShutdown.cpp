

static SpinLock deletedAtShutdownLock; // use a spin lock because it can be statically initialised

static Array<DeletedAtShutdown*>& getDeletedAtShutdownObjects()
{
    static Array<DeletedAtShutdown*> objects;
    return objects;
}

DeletedAtShutdown::DeletedAtShutdown()
{
    const SpinLock::ScopedLockType sl (deletedAtShutdownLock);
    getDeletedAtShutdownObjects().add (this);
}

DeletedAtShutdown::~DeletedAtShutdown()
{
    const SpinLock::ScopedLockType sl (deletedAtShutdownLock);
    getDeletedAtShutdownObjects().removeFirstMatchingValue (this);
}

#if HMSVC
 // Disable unreachable code warning, in case the compiler manages to figure out that
 // you have no classes of DeletedAtShutdown that could throw an exception in their destructor.
 #pragma warning (push)
 #pragma warning (disable: 4702)
#endif

void DeletedAtShutdown::deleteAll()
{
    // make a local copy of the array, so it can't get into a loop if something
    // creates another DeletedAtShutdown object during its destructor.
    Array<DeletedAtShutdown*> localCopy;

    {
        const SpinLock::ScopedLockType sl (deletedAtShutdownLock);
        localCopy = getDeletedAtShutdownObjects();
    }

    for (int i = localCopy.size(); --i >= 0;)
    {
        HTRY
        {
            auto* deletee = localCopy.getUnchecked(i);

            // double-check that it's not already been deleted during another object's destructor.
            {
                const SpinLock::ScopedLockType sl (deletedAtShutdownLock);

                if (! getDeletedAtShutdownObjects().contains (deletee))
                    deletee = nullptr;
            }

            delete deletee;
        }
        HCATCH_EXCEPTION
    }

    // if this fails, then it's likely that some new DeletedAtShutdown objects were
    // created while executing the destructors of the other ones.
    HAssert (getDeletedAtShutdownObjects().isEmpty());

    getDeletedAtShutdownObjects().clear(); // just to make sure the array doesn't have any memory still allocated
}

#if HMSVC
 #pragma warning (pop)
#endif


