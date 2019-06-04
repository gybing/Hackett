/*
  ==============================================================================

   This file is part of the H library.
   Copyright (c) 2017 - ROLI Ltd.

   H is an open source library subject to commercial or open-source
   licensing.

   By using H, you agree to the terms of both the H 5 End-User License
   Agreement and H 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.H.com/H-5-licence
   Privacy Policy: www.H.com/H-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   H IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#if HUNIT_TESTS

class ValueWithDefaultTests  : public UnitTest
{
public:
    ValueWithDefaultTests()
        : UnitTest ("ValueWithDefault", UnitTestCategories::values)
    {}

    void runTest() override
    {
        beginTest ("default constructor");
        {
            ValueWithDefault vwd;
            expect (vwd.isUsingDefault());
            expect (vwd.get() == var());
        }

        beginTest ("missing property");
        {
            ValueTree t ("root");
            ValueWithDefault vwd (t, "testKey", nullptr, "default");

            expect (vwd.isUsingDefault());
            expectEquals (vwd.get().toString(), String ("default"));
        }

        beginTest ("non-empty property");
        {
            ValueTree t ("root");
            t.setProperty ("testKey", "non-default", nullptr);

            ValueWithDefault vwd (t, "testKey", nullptr, "default");

            expect (! vwd.isUsingDefault());
            expectEquals (vwd.get().toString(), String ("non-default"));
        }

        beginTest ("set default");
        {
            ValueTree t ("root");

            ValueWithDefault vwd (t, "testkey", nullptr);
            vwd.setDefault ("default");

            expect (vwd.isUsingDefault());
            expectEquals (vwd.get().toString(), String ("default"));
        }

        beginTest ("set value");
        {
            ValueTree t ("root");
            t.setProperty ("testkey", "testvalue", nullptr);

            ValueWithDefault vwd (t, "testkey", nullptr, "default");
            vwd = "newvalue";

            expect (! vwd.isUsingDefault());
            expectEquals (t["testkey"].toString(), String ("newvalue"));

            vwd.resetToDefault();

            expect (vwd.isUsingDefault());
            expect (t["testkey"] == var());
        }
    }
};

static ValueWithDefaultTests valueWithDefaultTests;

#endif


