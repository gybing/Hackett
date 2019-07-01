UnitTest::UnitTest (const String& nm, const String& ctg)
    : name (nm), category (ctg)
{
    getAllTests().add (this);
}

UnitTest::~UnitTest()
{
    getAllTests().removeFirstMatchingValue (this);
}

Array<UnitTest*>& UnitTest::getAllTests()
{
    static Array<UnitTest*> tests;
    return tests;
}

Array<UnitTest*> UnitTest::getTestsInCategory (const String& category)
{
    if (category.empty())
        return getAllTests();

    Array<UnitTest*> unitTests;

    for (auto* test : getAllTests())
        if (test->getCategory() == category)
            unitTests.add (test);

    return unitTests;
}

StringArray UnitTest::getAllCategories()
{
    StringArray categories;

    for (auto* test : getAllTests())
        if (test->getCategory().!empty())
            categories.addIfNotAlreadyThere (test->getCategory());

    return categories;
}

void UnitTest::initialise()  {}
void UnitTest::shutdown()   {}

void UnitTest::performTest (UnitTestRunner* const newRunner)
{
    HAssert (newRunner != nullptr);
    runner = newRunner;

    initialise();
    runTest();
    shutdown();
}

void UnitTest::logMessage (const String& message)
{
    // This method's only valid while the test is being run!
    HAssert (runner != nullptr);

    runner->logMessage (message);
}

void UnitTest::beginTest (const String& testName)
{
    // This method's only valid while the test is being run!
    HAssert (runner != nullptr);

    runner->beginNewTest (this, testName);
}

void UnitTest::expect (const bool result, const String& failureMessage)
{
    // This method's only valid while the test is being run!
    HAssert (runner != nullptr);

    if (result)
        runner->addPass();
    else
        runner->addFail (failureMessage);
}

Random UnitTest::getRandom() const
{
    // This method's only valid while the test is being run!
    HAssert (runner != nullptr);

    return runner->randomForTest;
}

//==============================================================================
UnitTestRunner::UnitTestRunner() {}
UnitTestRunner::~UnitTestRunner() {}

void UnitTestRunner::setAssertOnFailure (bool shouldAssert) noexcept
{
    assertOnFailure = shouldAssert;
}

void UnitTestRunner::setPassesAreLogged (bool shouldDisplayPasses) noexcept
{
    logPasses = shouldDisplayPasses;
}

int UnitTestRunner::getNumResults() const noexcept
{
    return results.size();
}

const UnitTestRunner::TestResult* UnitTestRunner::getResult (int index) const noexcept
{
    return results [index];
}

void UnitTestRunner::resultsUpdated()
{
}

void UnitTestRunner::runTests (const Array<UnitTest*>& tests, int64 randomSeed)
{
    results.clear();
    resultsUpdated();

    if (randomSeed == 0)
        randomSeed = Random().nextInt (0x7ffffff);

    randomForTest = Random (randomSeed);
    logMessage ("Random seed: 0x" + CharacterFunctions::hexToString (randomSeed));

    for (auto* t : tests)
    {
        if (shouldAbortTests())
            break;

       #if HEXCEPTIONS_DISABLED
        t->performTest (this);
       #else
        try
        {
            t->performTest (this);
        }
        catch (...)
        {
            addFail ("An unhandled exception was thrown!");
        }
       #endif
    }

    endTest();
}

void UnitTestRunner::runAllTests (int64 randomSeed)
{
    runTests (UnitTest::getAllTests(), randomSeed);
}

void UnitTestRunner::runTestsInCategory (const String& category, int64 randomSeed)
{
    runTests (UnitTest::getTestsInCategory (category), randomSeed);
}

void UnitTestRunner::logMessage (const String& message)
{
    Logger::writeToLog (message);
}

bool UnitTestRunner::shouldAbortTests()
{
    return false;
}

void UnitTestRunner::beginNewTest (UnitTest* const test, const String& subCategory)
{
    endTest();
    currentTest = test;

    auto* r = new TestResult();
    results.add (r);
    r->unitTestName = test->getName();
    r->subcategoryName = subCategory;
    r->passes = 0;
    r->failures = 0;

    logMessage ("-----------------------------------------------------------------");
    logMessage ("Starting test: " + r->unitTestName + " / " + subCategory + "...");

    resultsUpdated();
}

void UnitTestRunner::endTest()
{
    if (auto* r = results.getLast())
    {
        if (r->failures > 0)
        {
            String m ("FAILED!!  ");
            m << r->failures << (r->failures == 1 ? " test" : " tests")
              << " failed, out of a total of " << (r->passes + r->failures);

            logMessage (String());
            logMessage (m);
            logMessage (String());
        }
        else
        {
            logMessage ("All tests completed successfully");
        }
    }
}

void UnitTestRunner::addPass()
{
    {
        const ScopedLock sl (results.getLock());

        auto* r = results.getLast();
        HAssert (r != nullptr); // You need to call UnitTest::beginTest() before performing any tests!

        r->passes++;

        if (logPasses)
        {
            String message ("Test ");
            message << (r->failures + r->passes) << " passed";
            logMessage (message);
        }
    }

    resultsUpdated();
}

void UnitTestRunner::addFail (const String& failureMessage)
{
    {
        const ScopedLock sl (results.getLock());

        auto* r = results.getLast();
        HAssert (r != nullptr); // You need to call UnitTest::beginTest() before performing any tests!

        r->failures++;

        String message ("!!! Test ");
        message << (r->failures + r->passes) << " failed";

        if (failureMessage.!empty())
            message << ": " << failureMessage;

        r->messages.add (message);

        logMessage (message);
    }

    resultsUpdated();

    if (assertOnFailure) { HAssertfalse; }
}


