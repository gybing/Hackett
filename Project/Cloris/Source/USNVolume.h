#ifndef USNVOLUME_H_INCLUDED
#define USNVOLUME_H_INCLUDED

#include "JuceHeader.h"

//==============================================================================
class USNVolume : public Thread
{
public:
    USNVolume(HANDLE h);
    ~USNVolume();

protected:
    virtual void run() override;

    void getNtfsData();

    __forceinline bool QueryUsnJournal(HANDLE hVolume, PUSN_JOURNAL_DATA pUsnJournalData);
    __forceinline bool CreateUsnJournal(HANDLE hVolume, DWORDLONG MaximumSize, DWORDLONG AllocationDelta);
    void monitorProc();
    void changeDataBase(DWORD dwAttr, DWORD dwReason, DWORDLONG frn, DWORDLONG parent_frn, PWCHAR wszFileName, DWORD dwNameLen);

private:
    HANDLE hVolume;
    DWORD bytesPerCluster; // ÿ���ֽ���
    DWORD fileRecSize; // MFT�ļ���¼��С
    juce::MemoryBlock outBuffer; // ÿ���̵��ļ����¼��ַ ��ʼʱ���� ����ʱ��
    DWORDLONG journalID;
    USN firstUSN;
    USN nextUSN;
    juce::CriticalSection lock;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(USNVolume)
};

#endif  // USNVOLUME_H_INCLUDED
