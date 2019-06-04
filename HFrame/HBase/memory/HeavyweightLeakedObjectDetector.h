

//==============================================================================
/**
    This class is a useful way of tracking down hard to find memory leaks when the
    regular LeakedObjectDetector isn't enough.

    As well as firing when any instances of the OwnerClass type are leaked, it will
    print out a stack trace showing where the leaked object was created. This is obviously
    quite a heavyweight task so, unlike the LeakedObjectDetector which should be always
    be added to your classes, you should only use this object temporarily when you are
    debugging and remove it when finished.

    To use it, use the HHEAVYWEIGHT_LEAK_DETECTOR macro as a simple way to put
    one in your class declaration.

    @tags{Core}
*/
template <class OwnerClass>
class HeavyweightLeakedObjectDetector
{
public:
    //==============================================================================
    HeavyweightLeakedObjectDetector() noexcept                                           { getBacktraceMap()[this] = SystemStats::getStackBacktrace(); }
    HeavyweightLeakedObjectDetector (const HeavyweightLeakedObjectDetector&) noexcept    { getBacktraceMap()[this] = SystemStats::getStackBacktrace(); }

    ~HeavyweightLeakedObjectDetector()                                                   { getBacktraceMap().erase (this); }

private:
    //==============================================================================
    typedef std::map<HeavyweightLeakedObjectDetector<OwnerClass>*, String> BacktraceMap;

    //==============================================================================
    struct BacktraceMapHolder
    {
        BacktraceMapHolder() = default;

        ~BacktraceMapHolder()
        {
            if (map.size() > 0)
            {
                DBG ("*** Leaked objects detected: " << map.size() << " instance(s) of class " << getLeakedObjectClassName());
                DBG (getFormattedBacktracesString());

                /** If you hit this, then you've leaked one or more objects of the type specified by
                    the 'OwnerClass' template parameter - the name and stack trace of its creation should
                    have been printed by the lines above.

                    If you're leaking, it's probably because you're using old-fashioned, non-RAII techniques for
                    your object management. Tut, tut. Always, always use std::unique_ptrs, OwnedArrays,
                    ReferenceCountedObjects, etc, and avoid the 'delete' operator at all costs!
                */
                HAssertfalse;
            }
        }

        String getFormattedBacktracesString() const
        {
            String str;

            int counter = 1;
            for (auto& bt : map)
            {
                str << "\nBacktrace " << String (counter++)                                << "\n"
                    << "-----------------------------------------------------------------" << "\n"
                    << bt.second;
            }

            return str;
        }

        BacktraceMap map;
    };

    static BacktraceMap& getBacktraceMap()
    {
        static BacktraceMapHolder holder;
        return holder.map;
    }

    static const char* getLeakedObjectClassName()
    {
        return OwnerClass::getLeakedObjectClassName();
    }
};

//==============================================================================
#if DOXYGEN || ! defined (HHEAVYWEIGHT_LEAK_DETECTOR)
 #if (DOXYGEN || HCHECK_MEMORY_LEAKS)
  /** This macro lets you embed a heavyweight leak-detecting object inside a class.

      To use it, simply declare a HHEAVYWEIGHT_LEAK_DETECTOR (YourClassName) inside a private section
      of the class declaration. E.g.

      @code
      class MyClass
      {
      public:
          MyClass();
          void blahBlah();

      private:
          HHEAVYWEIGHT_LEAK_DETECTOR (MyClass)
      };
      @endcode

      NB: you should only use this when you really need to track down a tricky memory leak, and
      should never leave one of these inside a class!

      @see HeavyweightLeakedObjectDetector, HLEAK_DETECTOR, LeakedObjectDetector
  */
  #define HHEAVYWEIGHT_LEAK_DETECTOR(OwnerClass) \
        friend class HeavyweightLeakedObjectDetector<OwnerClass>; \
        static const char* getLeakedObjectClassName() noexcept { return #OwnerClass; } \
        HeavyweightLeakedObjectDetector<OwnerClass> HJOIN_MACRO (leakDetector, __LINE__);
 #else
  #define HHEAVYWEIGHT_LEAK_DETECTOR(OwnerClass)
 #endif
#endif


