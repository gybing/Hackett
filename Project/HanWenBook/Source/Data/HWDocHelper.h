#ifndef __DATA_HELPER_HH__
#define __DATA_HELPER_HH__

class HWDocHelper
{
public:
    juce_DeclareSingleton(HWDocHelper, false)
    bool CheckResponseError(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi);

    // 学校相关
    const juce::String* GetGradeNameFromeID(int nGradeId);
    const juce::String* GetClassNameFromeID(int nClassId);
    int GetGradeIDFromName(const wchar_t* wszGrade);
    int GetClassIDFromName(const wchar_t* wszClass);

    const juce::String* GetUserReadLevel(int nReadCount, HWEnum_Gender enSex);
    void GetTimeFormat(time_t tTime, juce::String& strFmt, bool bShort = true);

    void GetHeadPicRequestUrl(juce::String& strUrl, const char* szUserID);

    void GetRomanNum(unsigned int n, juce::String& strNum);
    void GetChineseNum(unsigned int n, juce::String& strNum);

    void GetUserLoginStatStr(int nCount, time_t tTime, juce::String& strStat);

private:
    JUCE_LEAK_DETECTOR (HWDocHelper)
};


#endif // __DATA_USERINFOHH__