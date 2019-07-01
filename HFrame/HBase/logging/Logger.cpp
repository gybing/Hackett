

Logger::Logger() {}

Logger::~Logger()
{
    // You're deleting this logger while it's still being used!
    // Always call Logger::setCurrentLogger (nullptr) before deleting the active logger.
    HAssert (currentLogger != this);
}

Logger* Logger::currentLogger = nullptr;

void Logger::setCurrentLogger (Logger* const newLogger) noexcept    { currentLogger = newLogger; }
Logger* Logger::getCurrentLogger()  noexcept                        { return currentLogger; }

void Logger::writeToLog (const String& message)
{
    if (currentLogger != nullptr)
        currentLogger->logMessage (message);
    else
        outputDebugString (message);
}

#if HLOG_ASSERTIONS || HDEBUG
void API CALLTYPE logAssertion (const char* const filename, const int lineNum) noexcept
{
    String m ("H Assertion failure in ");
    m += File::createFileWithoutCheckingPath (filename).getFileName() + ':' + std::to_string(lineNum);

   #if HLOG_ASSERTIONS
    Logger::writeToLog (m);
   #else
    DBG (m);
   #endif
}
#endif


