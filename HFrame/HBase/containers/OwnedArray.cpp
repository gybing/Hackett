

#if HUNIT_TESTS

static struct OwnedArrayTest : public UnitTest
{
    struct Base
    {
        Base() = default;
        virtual ~Base() = default;

        HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Base)
    };

    struct Derived : Base
    {
        Derived() = default;

        HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Derived)
    };

    struct DestructorObj
    {
        DestructorObj (OwnedArrayTest& p,
                       OwnedArray<DestructorObj>& arr)
            : parent (p), objectArray (arr)
        {}

        ~DestructorObj()
        {
            data = 0;

            for (auto* o : objectArray)
            {
                parent.expect (o != nullptr);
                parent.expect (o != this);
                parent.expectEquals (o->data, 956);
            }
        }

        OwnedArrayTest& parent;
        OwnedArray<DestructorObj>& objectArray;
        int data = 956;

        HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DestructorObj)
    };

    OwnedArrayTest()
        : UnitTest ("OwnedArray", UnitTestCategories::containers)
    {}

    void runTest() override
    {
        beginTest ("After converting move construction, ownership is transferred");
        {
            OwnedArray<Derived> derived { new Derived{}, new Derived{}, new Derived{} };

            OwnedArray<Base> base  { std::move (derived) };

            expectEquals (base.size(), 3);
            expectEquals (derived.size(), 0);
        }

        beginTest ("After converting move assignment, ownership is transferred");
        {
            OwnedArray<Base> base;

            base = OwnedArray<Derived> { new Derived{}, new Derived{}, new Derived{} };

            expectEquals (base.size(), 3);
        }

        beginTest ("Iterate in destructor");
        {
            {
                OwnedArray<DestructorObj> arr;

                for (int i = 0; i < 2; ++i)
                    arr.add (new DestructorObj (*this, arr));
            }

            OwnedArray<DestructorObj> arr;

            for (int i = 0; i < 1025; ++i)
                arr.add (new DestructorObj (*this, arr));

            while (! arr.empty())
                arr.remove (0);

            for (int i = 0; i < 1025; ++i)
                arr.add (new DestructorObj (*this, arr));

            arr.removeRange (1, arr.size() - 3);

            for (int i = 0; i < 1025; ++i)
                arr.add (new DestructorObj (*this, arr));

            arr.set (500, new DestructorObj (*this, arr));
        }
    }
} ownedArrayTest;

#endif
