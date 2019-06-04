

ChildProcess::ChildProcess() {}
ChildProcess::~ChildProcess() {}

bool ChildProcess::isRunning() const
{
    return activeProcess != nullptr && activeProcess->isRunning();
}

int ChildProcess::readProcessOutput (void* dest, int numBytes)
{
    return activeProcess != nullptr ? activeProcess->read (dest, numBytes) : 0;
}

bool ChildProcess::kill()
{
    return activeProcess == nullptr || activeProcess->killProcess();
}

uint32 ChildProcess::getExitCode() const
{
    return activeProcess != nullptr ? activeProcess->getExitCode() : 0;
}

bool ChildProcess::waitForProcessToFinish (const int timeoutMs) const
{
    auto timeoutTime = Time::getMillisecondCounter() + (uint32) timeoutMs;

    do
    {
        if (! isRunning())
            return true;

        Thread::sleep (2);
    }
    while (timeoutMs < 0 || Time::getMillisecondCounter() < timeoutTime);

    return false;
}

String ChildProcess::readAllProcessOutput()
{
    MemoryOutputStream result;

    for (;;)
    {
        char buffer[512];
        auto num = readProcessOutput (buffer, sizeof (buffer));

        if (num <= 0)
            break;

        result.write (buffer, (size_t) num);
    }

    return result.toString();
}


//==============================================================================
//==============================================================================
#if HUNIT_TESTS

class ChildProcessTests  : public UnitTest
{
public:
    ChildProcessTests()
        : UnitTest ("ChildProcess", UnitTestCategories::threads)
    {}

    void runTest() override
    {
        beginTest ("Child Processes");

      #if HWINDOWS || HMAC || HLINUX
        ChildProcess p;

       #if HWINDOWS
        expect (p.start ("tasklist"));
       #else
        expect (p.start ("ls /"));
       #endif

        auto output = p.readAllProcessOutput();
        expect (output.isNotEmpty());
      #endif
    }
};

static ChildProcessTests childProcessUnitTests;

#endif


