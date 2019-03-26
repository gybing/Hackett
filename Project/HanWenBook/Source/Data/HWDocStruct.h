#pragma once

// <CheckVersion>
struct HWTCheckVersion
{
    juce::String strVersion;
    juce::String strNewVersion;
    bool bForce;
    juce::String strDescription;
    juce::String strDownURL;
    std::vector<std::pair<juce::String,int>> vecDefFiles;

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTCheckVersion)
};

// <UserUataVer>:�û����ݰ汾
struct HWTUserUataVer
{
    int nHead; // @head- ͷ��汾��
    int nAttention; // @attention- ��ע�б�汾��
    int nTagList; // @taglist- ��ǩ�б�汾��
    int nBookList; // @booklist- ͼ���б�汾��
    int nHistory; // @history- �Ķ���ʷ�汾��

private:
    JUCE_LEAK_DETECTOR (HWTUserUataVer)
};

// <BookDataVer>:ͼ�����ݰ汾
struct HWTBookDataVer
{
    int nInfo; // @info- ͼ����Ϣ�汾��
    int nPic; // @pic- ͼ��ͼƬ�汾��
    int nDraw; // @draw- ͼ��������ݰ汾��
    int nText; // @text- ͼ��ȫ�����ݰ汾��

private:
    JUCE_LEAK_DETECTOR (HWTBookDataVer)
};


//      ����˵��
// �汾�������������ݵĶ�Ӧ��ϵ�ǣ�
// info : ͼ�������Ϣ(X.1)�����(X.2)��Ŀ¼(X.2)���Ķ���������(X.2)��
// pic : ���гߴ��ͼ����漰�鼹ͼƬ(X.3,X.4)
// draw��ҳ������(ҳ��������ҳ����)(Y.1)��ҳ��ӳ��(Y.2)��

// <UserBookVer>:�û�ͼ�����ݰ汾
struct HWTUserBookVer
{
    int nBookMark; // @bookmark- �ҵ�ͼ����ǩ�汾��
    int nSign; // @sign- �ҵ�ͼ�黮�߰汾��
    int nPostil; // @postil- �ҵ�ͼ����ע�汾��
    int nComment; // @comment- �ҵ�ͼ�����۰汾��

private:
    JUCE_LEAK_DETECTOR (HWTUserBookVer)
};

// <UserBookStat>:�û�ͼ��ͳ����Ϣ
struct HWTUserBookStat
{
    int nSign; // @sign- �ҶԱ������Ļ�������
    int nBookMark; // @bookmark- �ҶԱ���������ǩ����
    int nPostil; // @postil- �ҶԱ���������ע����
    int nComment; // @comment- �ҶԱ���������������

private:
    JUCE_LEAK_DETECTOR (HWTUserBookStat)
};

// <ClassInfo>:�༶��Ϣ
struct HWTClassInfo
{
    int nClassid; // @classid- �༶ID
    int nGrade; // @gradeno- �꼶��
    juce::String strName; // @name- ����
    juce::String strSchool; // @school- ѧУ
    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTClassInfo)
};

//     ���ݹ���
//�꼶���ɷ���˸��ݰ༶��ѧ��͵�ǰʱ�����ɣ��Ѿ���ҵ�İ༶���꼶��Ϊ��ҵѧ�ꡣ����δ��ҵ�༶���꼶��1-9���Ѿ���ҵ//�İ༶���꼶�Ŵ���2000��
//�ͻ��˸�������꼶�źͰ�����ɰ༶���ƣ�2003����ѧ�İ༶�����7��2008��ʱ�꼶��Ϊ5������Ϊ���꼶�߰࣬2009���ҵ��//���꼶��Ϊ2009���༶����Ϊ2009���߰ࡣ
//һ���༶���꼶����ÿ��8��1�պͺ�ָ̨���༶��ҵʱ���¡�

// <UserInfo>:�û�����
struct HWTUserInfo
{
    HWEnum_UserType enUserType; // @usertype- �û����ͣ��μ���¼����ֵ����
    int nMember; // @member:1234567890		// ��Ա�ʸ���ʱ���(8�ֽ�)
    juce::String strShelfNo; // @shelfno- ��ܺ�
    juce::String strName; // @name- ����
    HWEnum_Gender enSex; // @sex- �Ա�
    int nCredits; // @credits- ����

    void FromHWDict(const HWDict* dict);

    // EXTEND
    int ExtClass() const;
    int ExtStudentNo() const;
    std::vector<int>& ExtClasses();
    std::vector<juce::String> ExtChildren();

private:
    // ѧ��
    int nExtClass; // @class:12- ѧ�������༶
    int nExtStudentNo; // @studentno- ѧ��
    std::vector<int> arrExtClasses; // @classes- ��ʦ����İ༶�������������
    std::vector<juce::String> arrExtChildren; // @children- ���ӵ��û�ID�б�

private:
    JUCE_LEAK_DETECTOR (HWTUserInfo)
};


//      ���ݹ���
// ����û�����Ϊ��ʦ��ѧ���ֶ�������(null)���û��༶����Ϊ�ջ���
// �����ѧ�����û��༶����Ϊ�ջ�һ�����а༶���������ѧ�ţ�����༶Ϊ�գ�ѧ���ֶ�������(null)��

// <UserTag>:�û���ǩ��Ϣ
struct HWTUserTag
{
    int nId; // @id- ��ǩID
    juce::String strName; // @name- ��ǩ����

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTUserTag)
};

//      ���ݹ���
// �û���ǩֵ��1��ʼ

// <UserBook>:�û�ͼ����Ϣ
struct HWTUserBook
{
    int nId; // @id- �û�ͼ��ID
    juce::String strBook; // @guid- GUID
    int nTag; // @tag- ������ǩID
    time_t tTime; // @time- �ղ�ʱ��,32λʱ���
    juce::String strName; // @name- ����

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTUserBook)
};


//      ���ݹ���
// ͼ�����û�б�ǩ����ʱtagֵΪnull

// <ReadData>:�Ķ�����
struct HWTReadData
{
    juce::String strBook; // @guid- ͼ����
    int nPageNo; // @pageno- ����Ķ�ҳ��(���棺-1�����
    juce::String strShowPage; // @showpage- ��ʾҳ�룬���Ϊ0��������������
    int nPageCount; // @pagecount- �Ķ�ҳ��
    int nDuiation; // @duration- �Ķ�ʱ��
    time_t tEndTime; // @endtime- �����Ķ�ʱ��

private:
    JUCE_LEAK_DETECTOR (HWTReadData)
};

// <HistoryData>:�Ķ���ʷ����
struct HWTHistoryData
{
    juce::String strBook; // @guid- ͼ����
    int nPageNo; // @pageno- ����Ķ�ҳ��
    time_t tTime; // @time- ����Ķ�ʱ��
    juce::String strShowPage; // @showpage- ��ʾҳ��

private:
    JUCE_LEAK_DETECTOR (HWTHistoryData)
};

// <ReadBase>:�Ķ���������
struct HWTReadBase
{
    std::vector<juce::String> arrAuthor; // @A- ["����1","����2"]
    int nWordCount; // @WC- "����",
    int nPDFPageCount; // @PDFPC- "PDFҳ��",
    int nPDFWidth; // @PDFW- "PDF���",
    int nPDFHeight; // @PDFH- "PDF�߶�",
    Json::Value jvPDFNoInfo; // @PDFNoInfo- [ҳ����Ϣ],
    int nBodyPDFPageNo; // @BodyPDFPageNo- 13 ����ҳPDFҳ��
    std::vector<juce::String> arrNSDMode; // @NSDM- ["ģʽ1","ģʽ2"...]
    bool bExistText; // @ExistText- �Ƿ����ı���1���У� 0��û�У�
    float fHVScale; // @HVScale- ���ݱ�ֵ(float����), Ĭ��Ϊ1.0������1.0��ԭͼ��������ԭͼ���ߡ�
    bool bIsSupportPactMode; // @IsSupportPactMode- �Ƿ�֧�ֽ���ģʽ��1��֧�֣�0����֧�֣�
    int nFontWidth; // @FW- 11 �������ο��

    void FromJson(const Json::Value& jv);

private:
    JUCE_LEAK_DETECTOR (HWTReadBase)
};

//      ���ݹ���
// ?    PDFNoInfo:
// ҳ����Ϣ���ڽ�PDFҳ��ת��Ϊͼ��ҳ�벢������ʾ��ʽ��Ϊһ��json���飬��ʽ���£�
// [��ʾ��ʽ,ҳ���ֵ,��ʼҳ��,PDF��ʼҳ��,PDF����ҳ��...]
// ������ʾ��ʽΪһ����������ʾ��ͬ��ͼ��ҳ����ʾ��ʽ��ҳ���ֵΪһ����������ʾͼ��ҳ����PDFҳ���ӳ���ϵ����ʼҳ��// Ϊ��0��ʼ��������PDF��ʼ�ͽ���ҳ���Ϊ��1��ʼ��������Ҫ��PDF����ҳ�����PDF��ʼҳ�롣����5������Ϊһ�飬���ͼ����// �������ҳ��ҳ��������ж��������
// ��ʾ��ʽ����ȡ����ֵ����Ӧ��ͬ��ҳ����ʾ��ʽ��
// 0            1,2,3,4,5,6,7,8...
// 1            I,II,III,IV,V,VI,VII,VIII...
// 2            һ,��,��,��,��,��,��,��...
// ��ʾ��ʽ�Ժ���Ը�����Ҫ���䡣
// ҳ���ֵ����ȡ����ֵ������nΪ����0��������:
// n            1��PDFҳ���Ӧn��ͼ��ҳ��
// -n            n��PDFҳ���Ӧ1��ͼ��ҳ��
// ҳ���ֵ-1��1�ǵȼ۵ģ����ҳ���ֵΪ0����Ϊ1��
// ���ҳ���ֵ����1����һ��PDFҳ���Ӧ���ͼ��ҳ�룬��ʱӦ��ʾ��PDFҳ���Ӧ��ͼ��ҳ�뷶Χ�����磺��14-15ҳ��
// ͼ��ҳ��ļ��㷽ʽ��
// ��ָ����PDFҳ�룬���ȸ���PDF��ʼ�ͽ���ҳ���ҵ���PDFҳ�����ڵĲ����飬���û���ҵ������PDFҳ���Ӧ������ҳ�������// �������ҳ���ֵ����0����
// ͼ��ҳ�� �� ��ʼҳ�� + (PDFҳ�� �C PDF��ʼҳ��) * |ҳ���ֵ|
// ���ҳ���ֵС��0����
// ͼ��ҳ�� �� ��ʼҳ�� + (PDFҳ�� �C PDF��ʼҳ��) / |ҳ���ֵ|
// ������
// [0,1.0,10,1,125,0,1.0,136,126,255]
// ����������¼�������Ӧ��ϵ��ҳ������Ϊ1.0����ʾ��ʽΪ0��
// PDFҳ��1��125��Ӧ��ʾҳ��10��134
// PDFҳ��126��255��Ӧ��ʾҳ��136��265
// ҳ����Ϣ����û�У���ʱֱ����ʾPDFҳ�롣
// ?    NSDM:
// ͼ�鲻֧�ֵ���ʾģʽ��ͼ���ڿͻ�����ʾʱ���ڶ�����ʾģʽ��Ŀǰ������ģʽ�ͷ�ҳģʽ��NSDMΪһ�����飬��¼��ͼ�鲻֧// �ֵ�ȫ����ʾģʽ��������
// ["��ҳ"]
// ����Ŀǰ����������ʾģʽ���������֧�֣����ͼ���޷���ʾ���Ժ����ʵ����������ʾģʽ��������ж��ֵ��

// <BookSign>:�û���������
struct HWTBookSign
{
    int nId; // @id- ����ID
    int nPageNo; // @pageno- ҳ��
    DWORD dwClr; // @clr- ������ɫ
    time_t tTime; // @time- ����ʱ��
    int nBe[2]; // @be- �����ڱ�ҳ�е��ַ���ʼ��ź�����
    juce::String strText;

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTBookSign)
};

struct HWTBookPostil
{
    int nID; // @id- (int),
    int nDynamicID; // @dynamicid- (int),
    time_t tTime; // @time- (int),
    HWEnum_Device enDevice; // @devicetype- (int),			// �豸����
    juce::String strBook; // @guid- (string),
    int nPageNo; // @page- (int),
    juce::String strShowPage; // @showpage- (string),
    int nZone; // @zone- (int),
    juce::String strText; // @text- (string),				// ��ע�ı�
    juce::String strBookText; // @booktext- (string),		// ͼ��ԭ��
    int nBex; // @be- (jsonstring)			// json(��ʼ�ַ����ַ�����)
    int nBec;

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTBookPostil)
};

struct HWTBookComment
{
    int nID; // @id- (int),                    // ����ID
    int nDynamicID; // @dynamicid- (int),             // ��̬ID�����ڻ�ȡ�����ܱ�������Ϣ��
    time_t tTime; // @time- (int),
    HWEnum_Device enDeviceType; // @devicetype-  (int),			// �豸����
    juce::String strBook; // @guid- (string),
    juce::String strText; // @text-(string),				// �����ı�

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTBookComment)

};
// ���ݹ���
// clr��������ɫ���޷�����������ʽΪ0xAARRGGBB������ֽ�λʼ������Ϊ0xFF��
// pageno����������PDFҳ�룬������ʾҳ��ǰ����ʹ���Ķ����������е�PDFNoInfo�ֶ�ֵ����PDFҳ�뵽��ʾҳ���ת����
// beΪ�����ڱ�ҳ�е��ַ���Χ������μ�E.ʵ��˵����

// <BookMark>:�û���ǩ����
struct HWTBookMark
{
    int nPageNo; // @pageno- ҳ��
    juce::String strTitle; // @title- ��ǩ����
    DWORD dwClr; // @clr- ��ǩ��ɫ
    time_t tTime; // @time- ����ʱ��

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTBookMark)
};

// ���ݹ���
// clr��������ɫ������RGB��ʽ�����λʼ������Ϊ0xFF��
// pageno����������PDFҳ��

// <DynamicData>:��̬����
struct HWTDynamicData
{
    int nId; // @id- ��̬ID
    int nType; // @type- ��̬����
    time_t tTime; // @time- ����ʱ��
    int nReplyCount; // @replycount- �ظ���
    int nReviewCount; // @reviewcount- ������������ʦ����������
    int nFlowerCount; // @flowercount- �컨����
    int nFavourCount; // @favourcount- ��������
    HWDict dictContent; // @content- ��̬���ݣ��μ���¼��̬���ݸ�ʽ����
    HWEnum_Device enDeviceType; // @devicetype- �豸����
    unsigned long uIPaddr; // @ipaddr- IP��ַ

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTDynamicData)
};

// <DynamicOP>:��̬����״̬
struct HWTDynamicOP
{
    int nFlower; // @flower- "1"
    int nFavour; // @favour- "0"

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTDynamicOP)
};

//      ���ݹ���
// dynamicop������һ���û��Զ�̬�Ĳ���״̬��Ŀǰ֧�ֺ컨�͵��ޣ�0��ʾδ��ӣ�1��ʾ�Ѿ���ӡ�ע�⣺�ܷ���к컨������// ��������ƣ���������ʦ��ѧ���Ķ�̬��Ӻ컨�����Һ컨�����������Ϊ10
// <DynamicInfo>:��̬��Ϣ
struct HWTDynamicInfo
{
    HWTUserInfo user; // @user- ��̬������Ϣ
    HWTDynamicData data;// @data- ��̬����
    HWTDynamicOP op; // @op- ��̬���������߶Զ�̬�Ĳ���״̬

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTDynamicInfo)
};

// <NoteData>:�ʼ�����
struct HWTNoteData
{
    int nId; // @id- �ʼ�ID
    int nType; // @type- �ʼ�����
    time_t tTime; // @time- ����ʱ��
    Json::Value jvContent; // @content- �ʼ�����
    HWEnum_Device enDeviceType; // @devicetype- �豸����
    unsigned long ipAddr; // @ipaddr- IP��ַ

private:
    JUCE_LEAK_DETECTOR (HWTNoteData)
};

//      ���ݹ���
// �ʼ�����ʵ�����Ƕ�̬���ݵ��Ӽ���ȥ����������صļ����ֶΣ�ʹ�ʼǿ������߷��ʡ�Ŀǰ�ʼ���Ȼ�Զ�̬��ʽʵ�֣�����ı�// �����;��Ƕ�̬����(20-21)���ʼ����ݾ��Ƕ�̬���ݡ�

// <CommInfo>:��̬������Ϣ
struct HWTCommInfo
{
    int nReplyCount; // @replycount- �ظ���
    int nReviewCount; // @reviewcount- ������������ʦ����������
    int nFlowerCount; // @flowercount- �컨����
    int nFavourCount; // @favourcount- ��������

private:
    JUCE_LEAK_DETECTOR (HWTCommInfo)
};

// <ReplyData>:�ظ�����
struct HWTReplyData
{
    HWTUserInfo destUser; // @destuser- �ظ�Ŀ���û�
    int nId; // @id- �ظ�ID
    int nType; // @type- �ظ�����
    time_t tTime; // @time- ����ʱ��
    juce::String strText; // @text- �ظ��ı�
    HWEnum_Device enDeviceType; // @devicetype- �豸����
    unsigned long ipAddr; // @ipaddr- IP��ַ

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTReplyData)
};

// <ReplyInfo>:�ظ���Ϣ
struct HWTReplyInfo
{
    HWTUserInfo user; //     @user  >,
    HWTReplyData data; //     @data  >

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTReplyInfo)
};

// <ScoreData>:��������
struct HWTScoreData
{
    int nStars; // @stars- �����Ǽ�
    time_t tTime; // @time- ����ʱ���

private:
    JUCE_LEAK_DETECTOR (HWTScoreData)
};

// <ScoreInfo>:������Ϣ
struct HWTScoreInfo
{
    HWTUserInfo user; // @user- �û���Ϣ
    HWTScoreData data; // @data- ��������

private:
    JUCE_LEAK_DETECTOR (HWTScoreInfo)
};

// <BookScore>:ͼ������
struct HWTBookScore
{
    int nCount; // @count- ��������
    float fStars; // @stars- ƽ���Ǽ�
    int nMyStar; // @mystar- �ҵ������Ǽ���0��ʾδ����

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTBookScore)
};

// <BookStat>:ͼ��ͳ����Ϣ
struct HWTBookStat
{
    int nCollect; // @collect- �ղش���         
    int nPostil; // @postil- ��ע����
    int nComment; // @comment- ��������
    int nScore; // @score- ���ִ���
    float fStars; // @stars- ƽ�������Ǽ�
    int nRead; // @read- �Ķ��ȶ�
    int nCommend; // @commend- �Ƽ�����

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTBookStat)
};

// <UserStat>:�û�ͳ����Ϣ
struct HWTUserStat
{
    int nCollect; // @collect- ��������
    int nAttention; // @attention- ��ע����
    int nFans; // @fans- ��˿����
    int nPostil; // @postil- ��ע����
    int nComment; // @comment- ��������
    // int nScore; // @score- ��������
    int nFlower; // @flower- �컨����

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTUserStat)
};


// <ClassStat>:�༶ͳ��
struct HWTClassStat
{
    int nDuration; // @duration- �Ķ�ʱ��
    int nPage; // @page- �Ķ�ҳ��
    int nPostil; // @postil- ��ע����
    int nComment; // @comment- ��������

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTClassStat)
};

//      ���ݹ���
// �༶ͳ�����û��Ķ�����¼ͳ�����ݵ��ۺϣ��ټ���ѧ���Ƿ�ע�����ֻ��š�

// <NotifyData>:֪ͨ����
struct HWTNotifyData
{
    HWEnum_NotifyType enType; // @type- ֪ͨ���ͣ��μ���¼
    time_t tTime; // @time- ֪ͨ������ʱ��
    Json::Value jvContent; // @content- ֪ͨ���ݣ��μ���¼֪ͨ���ݸ�ʽ����

private:
    JUCE_LEAK_DETECTOR (HWTNotifyData)
};

//      ���ݹ���
// lastletterΪ��ȡ����user֮������һ��˽�ţ�����˽���ı�ΪժҪ���50���ַ���ע�⣺���һ��˽�������е����ͣ�����// (0)�����ȡ�߷��͸�user��˽�ţ�����(1)�����ȡ�ߴ�user���յ�˽�š�

// <LetterData>:˽������
struct HWTLetterData
{
    int nId; // @id- ˽��ID
    int nType; // @type- ˽�����ͣ��μ���¼
    time_t tTime; // @time- ����ʱ��
    juce::String strText; // @text- ˽���ı�

private:
    JUCE_LEAK_DETECTOR (HWTLetterData)
};

//      ���ݹ���
// ��¼�е�˽��������˽�����ݵĶ�ȡ��Ϊ���㣬����Ϊ����(0)��ʾ��ȡ�߷��͸��Է���˽�ţ�����(1)// �����ȡ�ߴӶԷ����յ�˽��

// <LetterUserInfo>:˽���û���Ϣ
struct HWTLetterUserInfo
{
    HWTUserInfo user; // @user- �û���Ϣ
    int nCount; // @count- ����û���˽��������
    int nNetCount; // @newcount- �Ӹ��û����յ�δ��˽������
    HWTLetterData lastLetter; // @lastletter- ���һ��˽��(�����Ƿ���
    // //�����˽������ΪժҪ���50��

private:
    JUCE_LEAK_DETECTOR (HWTLetterUserInfo)
};

// <MessageData>:��Ϣ����
struct HWTMessageData
{
    int nId; // @id- ��ϢID
    int nType; // @type- ��Ϣ���ͣ��μ���¼
    time_t tTime; // @time- ����ʱ��
    Json::Value jvContent; // @content- ��Ϣ���ݣ��μ���¼��Ϣ���ݸ�ʽ����

private:
    JUCE_LEAK_DETECTOR (HWTMessageData)
};

// <CommendData>:�༶�Ƽ�����
struct HWTCommendData
{
    time_t tTime; // @time- �Ƽ�ʱ��
    juce::String strText; // @text- �Ƽ�����
    int nCount; // @count- ͼ������
    std::vector<juce::String> arrBooks; // @books- �Ƽ�ͼ���б�
    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTCommendData)
};

// <CommendInfo>:�༶�Ƽ���Ϣ
struct HWTCommendInfo
{
    HWTUserInfo user; // @user- ��ʦ�û���Ϣ
    HWTCommendData commend; // @commend- �༶�Ƽ�����

private:
    JUCE_LEAK_DETECTOR (HWTCommendInfo)
};

// <CommendBook>:�Ƽ�ͼ��
struct HWTCommendBook
{
    time_t tTime; // @time- �Ƽ�ʱ��
    juce::String strBook; // @guid- �Ƽ�ͼ��

private:
    JUCE_LEAK_DETECTOR (HWTCommendBook)
};

// <TypeTree>:�����
struct HWTTypeTree
{
    Json::Value jvTypeTree; // [10001,"���/����",727,[...]

private:
    JUCE_LEAK_DETECTOR (HWTTypeTree)
};

//      ���ݹ���    
// Сѧ�汾�е���������ṩ2����𣬿ͻ��˲��ش������2���������ݡ�

// <BookInfo>:ͼ����Ϣ
struct HWTBookInfo
{
    juce::String strBook; // @guid- guid
    //int nDataVer; // @dataver- ͼ����Ϣ�汾
    int nTypeId; // @typeid- ͼ�����ID
    int nPageCount; // @pagecount- ͼ��ҳ��
    int nWordCount; // @wordcount- ͼ������
    int nDataSize; // @datasize- ���ݴ�С(B
    time_t tPressTime; // @presstime- ����ʱ��(32λ
    time_t tOnlineTime; // @onlinetime- �ϼ�ʱ��(32λ
    bool bOnline; // @bonline- �Ƿ��ڼ�
    juce::String strName; // @name- ����
    std::vector<juce::String> arrAuthors; // @authors- ����(json��ʽ���ַ�
    juce::String strPress; // @press- ������

    void FromJson(const Json::Value& jv);
    void FromHWDict(const HWDict* dict);
    HWTBookInfo& operator = (const HWTBookInfo& info);

private:
    JUCE_LEAK_DETECTOR (HWTBookInfo)
};

// <Catalog>:ͼ��Ŀ¼
struct HWTCatalog
{
    juce::String strTitle;
    int nPageNo;
    juce::String strShowPage;
    std::vector<HWTCatalog*> vecCatalog;
    void FromJson(const Json::Value& jv);
    ~HWTCatalog();

private:
    JUCE_LEAK_DETECTOR (HWTCatalog)
};

//      ���ݹ���
// [Ŀ¼��,PDFҳ��,��ʾҳ��,��Ŀ¼����]
// Ŀ¼����һ��string��PDFҳ����һ����1��ʼ����������ʾҳ����һ��string����Ŀ¼������һ��Ŀ¼�����array��û����Ŀ¼// ��Ϊ�յ�array����[]��
// Ŀ¼������Ϊ��������Ŀ¼������Ŀ¼������ʾҳ��Ϊ�յ�string����������PDFҳ��Ϊ0��

// <PageOffSets>:ȫ�ķ�ҳ�ַ�����
struct HWTPageOffSets
{
    // ���ݶ��壺
    // ǰ1ҳ�ַ�����(4B)|ǰ2ҳ�ַ�����(4B)|...
    int n1PageWordCount; 
    int n2PageWordCount; 

private:
    JUCE_LEAK_DETECTOR (HWTPageOffSets)
};

// <historydata>:�Ķ���ʷ����
struct HWTReadHistoryData
{
    juce::String strBook;
    juce::String strShowPageNo;    // ��ʾҳ��
    int nPageNo;
    time_t tLastTime;

    void FromHWDict(const HWDict* dict);
    void FromJson(const Json::Value& jv);

private:
    JUCE_LEAK_DETECTOR (HWTReadHistoryData)
};

// <
struct HWTTextSearchData
{
    std::vector<int> vecPDFNoInfo;
    std::vector<std::pair<juce::String,std::vector<int>>> vecHiPage;
    std::map<int, std::pair<std::map<juce::String,juce::String>,std::vector<int>>> mapPageInfo;

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTTextSearchData)
};

struct HWTIemInfo
{
    juce::String strCode; // ��code��: ��129621�� //��Ʒ����
    juce::String strName; // ��name��:��һ���ڻ�Ա��,									//��Ʒ����
    juce::String strDesc; // ��desc��:�������ѧ������ؼҳ��Ļ�Ա�ʸ��ӳ�һ�ꡱ,		//��Ʒ����
    int nPrice; // ��price��:6800,											//��Ʒ�۸�(��λ����)
    int nDiscount; // ��discount��:5800										//��Ʒ�ۿۼ�(��λ����)

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTIemInfo)
};