#pragma  once

#ifdef WIN32
#include <iostream>
#include <string>
static std::string ConvertAnsiToUtf8( const std::string & str )
{
    std::string strTemp;
    std::wstring strWTemp;
    {
        int wLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), NULL, 0);
        wchar_t* wBuf = new wchar_t[wLen + 1];
        memset(wBuf, 0, wLen * 2 + 2);
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), wBuf, wLen);
        strWTemp = wBuf;
        delete[] wBuf;
    }
    {
        int ulen = WideCharToMultiByte(CP_UTF8, 0, strWTemp.c_str(), strWTemp.length(), NULL, 0, NULL, NULL); 
        char* uBuf = new char[ulen + 1];
        memset(uBuf, 0, ulen + 1);
        WideCharToMultiByte (CP_UTF8, 0, strWTemp.c_str(), strWTemp.length(), uBuf, ulen, NULL, NULL);	
        strTemp = uBuf;
        delete[] uBuf;
    }
    return strTemp;
}
#define _U(s) ConvertAnsiToUtf8(s).c_str()
#else
#define _U(s) s
#endif

