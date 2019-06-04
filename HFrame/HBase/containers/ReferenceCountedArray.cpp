

#if HUNIT_TESTS

class ReferenceCountedArrayTests   : public UnitTest
{
public:
    ReferenceCountedArrayTests()
        : UnitTest ("ReferenceCountedArray", UnitTestCategories::containers)
    {}

    //==============================================================================
    void runTest() override
    {
        beginTest ("Add derived objects");
        {
            ReferenceCountedArray<TestDerivedObj> derivedArray;
            derivedArray.add (static_cast<TestDerivedObj*> (new TestBaseObj()));
            expectEquals (derivedArray.size(), 1);
            expectEquals (derivedArray.getObjectPointer (0)->getReferenceCount(), 1);
            expectEquals (derivedArray[0]->getReferenceCount(), 2);

            for (auto o : derivedArray)
                expectEquals (o->getReferenceCount(), 1);

            ReferenceCountedArray<TestBaseObj> baseArray;
            baseArray.addArray (derivedArray);

            for (auto o : baseArray)
                expectEquals (o->getReferenceCount(), 2);

            derivedArray.clearQuick();
            baseArray.clearQuick();

            auto* baseObject = new TestBaseObj();
            TestBaseObj::Ptr baseObjectPtr = baseObject;
            expectEquals (baseObject->getReferenceCount(), 1);

            auto* derivedObject = new TestDerivedObj();
            TestDerivedObj::Ptr derivedObjectPtr = derivedObject;
            expectEquals (derivedObject->getReferenceCount(), 1);

            baseArray.add (baseObject);
            baseArray.add (derivedObject);

            for (auto o : baseArray)
                expectEquals (o->getReferenceCount(), 2);

            expectEquals (baseObject->getReferenceCount(),    2);
            expectEquals (derivedObject->getReferenceCount(), 2);

            derivedArray.add (derivedObject);

            for (auto o : derivedArray)
                expectEquals (o->getReferenceCount(), 3);

            derivedArray.clearQuick();
            baseArray.clearQuick();

            expectEquals (baseObject->getReferenceCount(),    1);
            expectEquals (derivedObject->getReferenceCount(), 1);

            baseArray.add (baseObjectPtr);
            baseArray.add (derivedObjectPtr.get());

            for (auto o : baseArray)
                expectEquals (o->getReferenceCount(), 2);

            derivedArray.add (derivedObjectPtr);

            for (auto o : derivedArray)
                expectEquals (o->getReferenceCount(), 3);
        }

        beginTest ("Iterate in destructor");
        {
            {
                ReferenceCountedArray<DestructorObj> arr;

                for (int i = 0; i < 2; ++i)
                    arr.add (new DestructorObj (*this, arr));
            }

            ReferenceCountedArray<DestructorObj> arr;

            for (int i = 0; i < 1025; ++i)
                arr.add (new DestructorObj (*this, arr));

            while (! arr.isEmpty())
                arr.remove (0);

            for (int i = 0; i < 1025; ++i)
                arr.add (new DestructorObj (*this, arr));

            arr.removeRange (1, arr.size() - 3);

            for (int i = 0; i < 1025; ++i)
                arr.add (new DestructorObj (*this, arr));

            arr.set (500, new DestructorObj (*this, arr));
        }
    }

private:
    struct TestBaseObj : public ReferenceCountedObject
    {
        using Ptr = ReferenceCountedObjectPtr<TestBaseObj>;

        TestBaseObj() = default;

        HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestBaseObj)
    };

    struct TestDerivedObj : public TestBaseObj
    {
        using Ptr = ReferenceCountedObjectPtr<TestDerivedObj>;

        TestDerivedObj() = default;

        HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestDerivedObj)
    };

    struct DestructorObj : public ReferenceCountedObject
    {
        DestructorObj (ReferenceCountedArrayTests& p,
                       ReferenceCountedArray<DestructorObj>& arr)
            : parent (p), objectArray (arr)
        {}

        ~DestructorObj()
        {
            data = 0;

            for (auto* o : objectArray)
            {
                parent.expect (o != nullptr);
                parent.expect (o != this);
                parent.expectEquals (o->data, 374);
            }
        }

        ReferenceCountedArrayTests& parent;
        ReferenceCountedArray<DestructorObj>& objectArray;
        int data = 374;

        HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DestructorObj)
    };
};

static ReferenceCountedArrayTests referenceCountedArrayTests;

#endif


