

//==============================================================================
/** Fills a block of memory with zeros. */
inline void zeromem (void* memory, size_t numBytes) noexcept        { memset (memory, 0, numBytes); }

/** Overwrites a structure or object with zeros. */
template <typename Type>
inline void zerostruct (Type& structure) noexcept                   { memset ((void*) &structure, 0, sizeof (structure)); }

/** Delete an object pointer, and sets the pointer to null.

    Remember that it's not good c++ practice to use delete directly - always try to use a std::unique_ptr
    or other automatic lifetime-management system rather than resorting to deleting raw pointers!
*/
template <typename Type>
inline void deleteAndZero (Type& pointer)                           { delete pointer; pointer = nullptr; }

/** A handy function which adds a number of bytes to any type of pointer and returns the result.
    This can be useful to avoid casting pointers to a char* and back when you want to move them by
    a specific number of bytes,
*/
template <typename Type, typename IntegerType>
inline Type* addBytesToPointer (Type* basePointer, IntegerType bytes) noexcept  { return (Type*) (const_cast<char*> (reinterpret_cast<const char*> (basePointer)) + bytes); }

/** A handy function to round up a pointer to the nearest multiple of a given number of bytes.
    alignmentBytes must be a power of two. */
template <typename Type, typename IntegerType>
inline Type* snapPointerToAlignment (Type* basePointer, IntegerType alignmentBytes) noexcept
{
    return (Type*) ((((size_t) basePointer) + (alignmentBytes - 1)) & ~(alignmentBytes - 1));
}

/** A handy function which returns the difference between any two pointers, in bytes.
    The address of the second pointer is subtracted from the first, and the difference in bytes is returned.
*/
template <typename Type1, typename Type2>
inline int getAddressDifference (Type1* pointer1, Type2* pointer2) noexcept  { return (int) (((const char*) pointer1) - (const char*) pointer2); }

/** If a pointer is non-null, this returns a new copy of the object that it points to, or safely returns
    nullptr if the pointer is null.
*/
template <class Type>
inline Type* createCopyIfNotNull (const Type* objectToCopy) { return objectToCopy != nullptr ? new Type (*objectToCopy) : nullptr; }

//==============================================================================
/** A handy function to read un-aligned memory without a performance penalty or bus-error. */
template <typename Type>
inline Type readUnaligned (const void* srcPtr) noexcept
{
    Type value;
    memcpy (&value, srcPtr, sizeof (Type));
    return value;
}

/** A handy function to write un-aligned memory without a performance penalty or bus-error. */
template <typename Type>
inline void writeUnaligned (void* dstPtr, Type value) noexcept
{
    memcpy (dstPtr, &value, sizeof (Type));
}

//==============================================================================
#if HMAC || HIOS || DOXYGEN

 /** A handy C++ wrapper that creates and deletes an NSAutoreleasePool object using RAII.
     You should use the HAUTORELEASEPOOL macro to create a local auto-release pool on the stack.

     @tags{Core}
 */
 class API ScopedAutoReleasePool
 {
 public:
     ScopedAutoReleasePool();
     ~ScopedAutoReleasePool();

 private:
     void* pool;

     HDECLARE_NON_COPYABLE (ScopedAutoReleasePool)
 };

 /** A macro that can be used to easily declare a local ScopedAutoReleasePool
     object for RAII-based obj-C autoreleasing.
     Because this may use the \@autoreleasepool syntax, you must follow the macro with
     a set of braces to mark the scope of the pool.
 */
#if (HCOMPILER_SUPPORTS_ARC && defined (__OBJC__)) || DOXYGEN
 #define HAUTORELEASEPOOL  @autoreleasepool
#else
 #define HAUTORELEASEPOOL  const ScopedAutoReleasePool HJOIN_MACRO (autoReleasePool_, __LINE__);
#endif

#else
 #define HAUTORELEASEPOOL
#endif

//==============================================================================
/* In a Windows DLL build, we'll expose some malloc/free functions that live inside the DLL, and use these for
   allocating all the objects - that way all H objects in the DLL and in the host will live in the same heap,
   avoiding problems when an object is created in one module and passed across to another where it is deleted.
   By piggy-backing on the HLEAK_DETECTOR macro, these allocators can be injected into most H classes.
*/
#if HMSVC && (defined (HDLL) || defined (HDLL_BUILD)) && ! (HDISABLE_DLL_ALLOCATORS || DOXYGEN)
 extern API void* HDLL_malloc (size_t);
 extern API void  HDLL_free (void*);

 #define HLEAK_DETECTOR(OwnerClass)  public:\
    static void* operator new (size_t sz)           { return HDLL_malloc (sz); } \
    static void* operator new (size_t, void* p)     { return p; } \
    static void operator delete (void* p)           { HDLL_free (p); } \
    static void operator delete (void*, void*)      {}
#endif

//==============================================================================
/** (Deprecated) This was a Windows-specific way of checking for object leaks - now please
    use the HLEAK_DETECTOR instead.
*/
#ifndef UseDebuggingNewOperator
 #define UseDebuggingNewOperator
#endif


