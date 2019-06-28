

void Logger::outputDebugString (const String& text)
{
    __android_log_print (ANDROID_LOG_INFO, "H", "%s", text.toUTF8().getAddress());
}


