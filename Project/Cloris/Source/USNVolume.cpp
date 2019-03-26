#include "USNVolume.h"

USNVolume::USNVolume(HANDLE h)
    : Thread(L"volume_thread")
    , hVolume(h)
    , bytesPerCluster(0)
    , fileRecSize(0)
    , journalID(0)
    , firstUSN(0)
    , nextUSN(0)
{
    
}

USNVolume::~USNVolume()
{
}

void USNVolume::run()
{
    getNtfsData();
    monitorProc();
}

void USNVolume::getNtfsData()
{
    DWORD dwWritten;
    NTFS_VOLUME_DATA_BUFFER ntfsVolData;
    if (!DeviceIoControl(hVolume,
        FSCTL_GET_NTFS_VOLUME_DATA,
        NULL, 0,
        &ntfsVolData, sizeof(ntfsVolData),
        &dwWritten, NULL))
    {
        juce::String error((int)GetLastError());
        juce::AlertWindow::showMessageBox(AlertWindow::WarningIcon, L"����", error);
    }

    bytesPerCluster = ntfsVolData.BytesPerCluster;
    fileRecSize = sizeof(NTFS_FILE_RECORD_OUTPUT_BUFFER) - 1 + ntfsVolData.BytesPerFileRecordSegment;
    outBuffer.setSize(fileRecSize);
    USN_JOURNAL_DATA ujd;
    if (!QueryUsnJournal(hVolume, &ujd))
    {
        //��ѯʧ��
        switch (GetLastError())
        {
        case ERROR_JOURNAL_NOT_ACTIVE:
        {
            CreateUsnJournal(hVolume, 0x800000, 0x100000);
            QueryUsnJournal(hVolume, &ujd);
            break;
        }
        case ERROR_JOURNAL_DELETE_IN_PROGRESS:
        {
            DWORD cb;
            DELETE_USN_JOURNAL_DATA del_ujd;
            del_ujd.UsnJournalID = ujd.UsnJournalID;
            del_ujd.DeleteFlags = USN_DELETE_FLAG_NOTIFY;
            if (!DeviceIoControl(hVolume, FSCTL_DELETE_USN_JOURNAL,
                &del_ujd, sizeof(DELETE_USN_JOURNAL_DATA),
                NULL, 0, &cb, NULL
                )) 
            {

            }

            CreateUsnJournal(hVolume, 0x2000000, 0x400000);
            QueryUsnJournal(hVolume, &ujd);
            break;
        }
        default:
            break;
        }
    }
    journalID = ujd.UsnJournalID;
    firstUSN = ujd.FirstUsn;
    nextUSN = ujd.NextUsn;
}

bool USNVolume::QueryUsnJournal(HANDLE hVolume, PUSN_JOURNAL_DATA pUsnJournalData)
{
    DWORD cb;
    return DeviceIoControl(hVolume
        , FSCTL_QUERY_USN_JOURNAL
        , NULL, 0,
        pUsnJournalData, sizeof(USN_JOURNAL_DATA), &cb
        , NULL);
}

bool USNVolume::CreateUsnJournal(HANDLE hVolume, DWORDLONG MaximumSize, DWORDLONG AllocationDelta)
{
    DWORD cb;
    CREATE_USN_JOURNAL_DATA cujd;
    cujd.MaximumSize = MaximumSize;
    cujd.AllocationDelta = AllocationDelta;
    return DeviceIoControl(hVolume, FSCTL_CREATE_USN_JOURNAL,
        &cujd, sizeof(cujd), NULL, 0, &cb, NULL);
}

void USNVolume::monitorProc()
{
    const DWORD SEARCH_TITLE_REASON_FLAG =
        //֧���ļ�������
        USN_REASON_FILE_CREATE
        | USN_REASON_FILE_DELETE
        | USN_REASON_RENAME_OLD_NAME
        | USN_REASON_RENAME_NEW_NAME

        //֧���ļ����Ը��� //���������ʱ��֧��
        //         |USN_REASON_DATA_EXTEND
        //         |USN_REASON_DATA_OVERWRITE
        //         |USN_REASON_DATA_TRUNCATION
        //         |USN_REASON_BASIC_INFO_CHANGE   //�ļ����Ըı�
        //         |USN_REASON_OBJECT_ID_CHANGE    //LAST ACCESS
        ;

    READ_USN_JOURNAL_DATA_V0 rujd;
    rujd.BytesToWaitFor = 0;//�˴���1�ĳ�0 0ʱ�ȴ���¼���������
    rujd.ReasonMask = SEARCH_TITLE_REASON_FLAG;
    rujd.ReturnOnlyOnClose = 0;
    rujd.StartUsn = nextUSN;
    rujd.Timeout = 0;
    rujd.UsnJournalID = journalID;

    DWORD dwBytes;
    DWORD dwRetBytes;
    BYTE Buffer[USN_PAGE_SIZE];
    PUSN_RECORD pRecord;

    for (;;)
    {
        if (threadShouldExit())
            break;
        
        sleep(rand() % 1000 + 1000);

        if (!DeviceIoControl(hVolume,
            FSCTL_READ_USN_JOURNAL,
            &rujd,
            sizeof(rujd),
            Buffer,
            USN_PAGE_SIZE,
            &dwBytes,
            NULL))
        {
            String errormsg;
            DWORD dwError = GetLastError();
            switch (dwError)
            {
            case ERROR_INVALID_FUNCTION:
                errormsg = L"ERROR_INVALID_FUNCTION";
                break;
            case ERROR_INVALID_PARAMETER:
                errormsg = L"ERROR_INVALID_PARAMETER";
                break;
            case ERROR_JOURNAL_DELETE_IN_PROGRESS:
                errormsg = L"ERROR_JOURNAL_DELETE_IN_PROGRESS";
                DWORD cb;
                DELETE_USN_JOURNAL_DATA del_ujd;
                del_ujd.UsnJournalID = rujd.UsnJournalID;
                del_ujd.DeleteFlags = USN_DELETE_FLAG_NOTIFY;
                if (!DeviceIoControl(hVolume, FSCTL_DELETE_USN_JOURNAL,
                    &del_ujd, sizeof(DELETE_USN_JOURNAL_DATA),
                    NULL, 0, &cb, NULL
                    )) {
                }
                CreateUsnJournal(hVolume, 0x2000000, 0x400000);
                USN_JOURNAL_DATA ujd;
                QueryUsnJournal(hVolume, &ujd);
                journalID = ujd.UsnJournalID;
                nextUSN = ujd.NextUsn;
                break;
            case ERROR_JOURNAL_NOT_ACTIVE:
                errormsg = L"ERROR_JOURNAL_NOT_ACTIVE";
                break;
            case ERROR_JOURNAL_ENTRY_DELETED:
                errormsg = L"ERROR_JOURNAL_ENTRY_DELETED";
                break;
            default:
                errormsg = L"ERROR_UNKNOWN";
                break;
            }

            if (errormsg.isNotEmpty())
            {
                juce::AlertWindow::showMessageBox(AlertWindow::WarningIcon, L"����", errormsg);
            }
            break;
        }

        if (dwBytes <= sizeof(USN))
            continue; // ����!

        dwRetBytes = dwBytes - sizeof(USN); // ������1��USN����USN����һ�۲�ѯ���
        
        //������û����¼
        //�Ľ� �������еļ��Ӽ�¼��������
        pRecord = PUSN_RECORD((PBYTE)Buffer + sizeof(USN));

        {   // ����
            juce::ScopedLock l(lock);

            while (dwRetBytes > 0)//������1��USN�󣬻��ж����ֽڣ���������1����¼
            {
                changeDataBase(pRecord->FileAttributes
                    , pRecord->Reason
                    , pRecord->FileReferenceNumber
                    , pRecord->ParentFileReferenceNumber
                    , PWCHAR(pRecord->FileName)
                    , pRecord->FileNameLength >> 1
                    );
                dwRetBytes -= pRecord->RecordLength;

                //����һ����¼
                pRecord = (PUSN_RECORD)(((PBYTE)pRecord) + pRecord->RecordLength);
            }
        }

        //������ʼUSN
        rujd.StartUsn = nextUSN = *(USN*)Buffer;
    }
}

BOOL GetReasonString(DWORD dwReason, LPSTR pszReason, int cchReason) {
    static LPCSTR szCJReason[] = {
        "DataOverwrite",         // 0x00000001
        "DataExtend",            // 0x00000002
        "DataTruncation",        // 0x00000004
        "0x00000008",            // 0x00000008
        "NamedDataOverwrite",    // 0x00000010
        "NamedDataExtend",       // 0x00000020
        "NamedDataTruncation",   // 0x00000040
        "0x00000080",            // 0x00000080
        "FileCreate",            // 0x00000100
        "FileDelete",            // 0x00000200
        "PropertyChange",        // 0x00000400
        "SecurityChange",        // 0x00000800
        "RenameOldName",         // 0x00001000
        "RenameNewName",         // 0x00002000
        "IndexableChange",       // 0x00004000
        "BasicInfoChange",       // 0x00008000
        "HardLinkChange",        // 0x00010000
        "CompressionChange",     // 0x00020000
        "EncryptionChange",      // 0x00040000
        "ObjectIdChange",        // 0x00080000
        "ReparsePointChange",    // 0x00100000
        "StreamChange",          // 0x00200000
        "0x00400000",            // 0x00400000
        "0x00800000",            // 0x00800000
        "0x01000000",            // 0x01000000
        "0x02000000",            // 0x02000000
        "0x04000000",            // 0x04000000
        "0x08000000",            // 0x08000000
        "0x10000000",            // 0x10000000
        "0x20000000",            // 0x20000000
        "0x40000000",            // 0x40000000
        "*Close*"                // 0x80000000
    };
    CHAR sz[1024] = { 0 };
    sz[0] = sz[1] = sz[2] = 0;
    for (int i = 0; dwReason != 0; dwReason >>= 1, i++) {
        if ((dwReason & 1) == 1) {
            strcat(sz, ", ");
            strcat(sz, szCJReason[i]);
        }
    }
    BOOL fOk = FALSE;
    if (cchReason > strlen(&sz[2])) {
        strcpy(pszReason, &sz[2]);
        fOk = TRUE;
    }
    return(fOk);
}

void USNVolume::changeDataBase(DWORD dwAttr, DWORD dwReason, DWORDLONG frn, DWORDLONG parent_frn, PWCHAR wszFileName, DWORD dwNameLen)
{
    BOOL bFileNameChange = FALSE;
    BOOL bDir = dwAttr&FILE_ATTRIBUTE_DIRECTORY;
    
    char buf[MAX_PATH] = { 0 }, buf2[1024] = { 0 };
    WideCharToMultiByte(CP_ACP, 0, wszFileName, dwNameLen, buf, MAX_PATH, 0, 0);
    GetReasonString(dwReason, buf2, 1024);

    String str;
    str << "\nIN ChangeDataBase:" << buf << "  " << (int)frn << " bDir= " << bDir << " \n" << "ԭ��" << buf2 << "\n";
    OutputDebugStringA(str.getCharPointer());
#ifdef OUT_PUT_MONITOR
    if ((USN_REASON_FILE_CREATE&dwReason) && (USN_REASON_CLOSE&dwReason))
    {
        //��
        if (bDir)AddDirectoryBase(dwDri, frn, parent_frn, wszFileName, dwNameLen);
        else AddFileBase(dwDri, frn, parent_frn, wszFileName, dwNameLen);
        bFileNameChange = TRUE;
    }
    else if ((USN_REASON_RENAME_NEW_NAME&dwReason) && (dwReason&USN_REASON_CLOSE))
    {
        //��
        if (bDir)//������Ŀ¼ ����Ŀ¼�仯
            RenameNewDirectory(dwDri, frn, parent_frn, wszFileName, dwNameLen);
        else AddFileBase(dwDri, frn, parent_frn, wszFileName, dwNameLen);
        bFileNameChange = TRUE;
    }
    else if (USN_REASON_RENAME_OLD_NAME&dwReason)
    {
        //ɾ
        if (USN_REASON_FILE_CREATE&dwReason)
        {
            //old ���溬�д���
        }
        else
        {
            if (bDir) //������Ŀ¼ ����Ŀ¼�仯
                RenameOldDirectory(dwDri, frn, wszFileName, dwNameLen);
            else DeleteFileBase(dwDri, frn, wszFileName, dwNameLen);
            bFileNameChange = TRUE;
        }
    }
    else if ((dwReason&USN_REASON_FILE_DELETE) && (USN_REASON_CLOSE&dwReason))
    {
        //ɾ
        if (USN_REASON_FILE_CREATE&dwReason)
        {
            //new close ���溬���������ڴ����д���
        }
        else
        {
            if (bDir) DeleteDirectoryBase(dwDri, frn, wszFileName, dwNameLen);
            else DeleteFileBase(dwDri, frn, wszFileName, dwNameLen);
            bFileNameChange = TRUE;
        }
    }
    else
    {
#ifdef OUT_PUT_MONITOR
        DebugTrace("����������ԭ��\n");
#endif
        // �����ǣ�
        // [RenameNewName|Close] ��ֹ�ظ�����
        // [FileCreate|Close]    ��ֹ�ظ�����
    }


    if (bFileNameChange)
    {
        // �����ļ����ı䣬���ǵ�ǰ���봮�Ľṹʱ�����û�����
        if (bDir && !g_bDirSearch) return; //�ı����Ŀ¼ ����ǰ������Ŀ¼
        if (!bDir && !g_bFileSearch) return;// �ı�����ļ� ����ǰ�������ļ�

        // ��鱾�ļ��Ƿ�Ե�ǰ��ѯ����Ӱ��
        SearchStrOpt *pStrOpt;
        if (g_bCase)
        {// ��Сд����
            pStrOpt = &g_StrOptCase;
        }
        else
        {// ������                         
            pStrOpt = &g_StrOptNoCase;
        }

#ifdef OUT_PUT_MONITOR
        DebugTrace("Check:\n");
#endif

        BOOL bEffect = FALSE; // ���ı����ʾ�����Ӱ��ô?
        if (pStrOpt->HasQuestion())
        { // ����?�����ܺ��С���ͨ��*��*?��
            bEffect = Helper_CheckQuestionSearch(g_bCase, wszFileName, dwNameLen);
        }
        else
        {
            //������?��
            if (pStrOpt->HasStar())
            {
                //������?��������*�����ܺ��С���ͨ��*?��
                bEffect = Helper_CheckStarSearch(g_bCase, wszFileName, dwNameLen);
            }
            else
            {
                //������?��*��*a
                if (pStrOpt->HasStarQuestion())
                {
                    //������?��*��������?*�����ܺ��С���ͨ��
                    bEffect = Helper_CheckStarQuestionSearch(g_bCase, wszFileName, dwNameLen);
                }
                else
                {
                    //������?��*��?*����������ͨ
                    //assert(pStrOpt->pNormal && "һ��������ͨ��");//��ͨ���Ѵ���
                    if (pStrOpt->pNormal)
                    {
                        //��ȫ*ʱ������ͨ��
                        bEffect = Helper_CheckOnlyNormalSearch(g_bCase, wszFileName, dwNameLen);//����
                    }
                }
            }
        }
        if (bEffect)//ֱ���޸Ľ��
        {
#ifdef OUT_PUT_MONITOR
            DebugTrace("SendMessage:\n");
#endif
            g_bMonitorChange = TRUE;
            KernelSearch();
            g_bMonitorChange = FALSE;
        }
    }
#ifdef OUT_PUT_MONITOR
    DebugTrace("OUT ChangeDataBase:%s  bDir=%d\n", buf, bDir);
#endif

#endif

}