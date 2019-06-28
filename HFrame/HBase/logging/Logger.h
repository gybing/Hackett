

//==============================================================================
/**
    Acts as an application-wide logging class.

    A subclass of Logger can be created and passed into the Logger::setCurrentLogger
    method and this will then be used by all calls to writeToLog.

    The logger class also contains methods for writing messages to the debugger's
    output stream.

    @see FileLogger

    @tags{Core}
*/
class API Logger
{
public:
    //==============================================================================
    /** Destructor. */
    virtual ~Logger();

    //==============================================================================
    /** Sets the current logging class to use.

        Note that the object passed in will not be owned or deleted by the logger, so
        the caller must make sure that it is not deleted while still being used.
        A null pointer can be passed-in to reset the system to the default logger.
    */
    static void CALLTYPE setCurrentLogger (Logger* newLogger) noexcept;

    /** Returns the current logger, or nullptr if no custom logger has been set. */
    static Logger* CALLTYPE getCurrentLogger() noexcept;

    /** Writes a string to the current logger.

        This will pass the string to the logger's logMessage() method if a logger
        has been set.

        @see logMessage
    */
    static void CALLTYPE writeToLog (const String& message);


    //==============================================================================
    /** Writes a message to the standard error stream.

        This can be called directly, or by using the DBG() macro in
        PlatformDefs.h (which will avoid calling the method in non-debug builds).
    */
    static void CALLTYPE outputDebugString (const String& text);


protected:
    //==============================================================================
    Logger();

    /** This is overloaded by subclasses to implement custom logging behaviour.
        @see setCurrentLogger
    */
    virtual void logMessage (const String& message) = 0;

private:
    static Logger* currentLogger;
};


