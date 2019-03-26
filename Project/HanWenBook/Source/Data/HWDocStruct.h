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

// <UserUataVer>:用户数据版本
struct HWTUserUataVer
{
    int nHead; // @head- 头像版本号
    int nAttention; // @attention- 关注列表版本号
    int nTagList; // @taglist- 标签列表版本号
    int nBookList; // @booklist- 图书列表版本号
    int nHistory; // @history- 阅读历史版本号

private:
    JUCE_LEAK_DETECTOR (HWTUserUataVer)
};

// <BookDataVer>:图书数据版本
struct HWTBookDataVer
{
    int nInfo; // @info- 图书信息版本号
    int nPic; // @pic- 图书图片版本号
    int nDraw; // @draw- 图书绘制数据版本号
    int nText; // @text- 图书全文数据版本号

private:
    JUCE_LEAK_DETECTOR (HWTBookDataVer)
};


//      数据说明
// 版本号与所控制数据的对应关系是：
// info : 图书基本信息(X.1)、简介(X.2)、目录(X.2)、阅读基础数据(X.2)。
// pic : 所有尺寸的图书封面及书脊图片(X.3,X.4)
// draw：页面数据(页面描述和页面框架)(Y.1)、页面映射(Y.2)。

// <UserBookVer>:用户图书数据版本
struct HWTUserBookVer
{
    int nBookMark; // @bookmark- 我的图书书签版本号
    int nSign; // @sign- 我的图书划线版本号
    int nPostil; // @postil- 我的图书批注版本号
    int nComment; // @comment- 我的图书评论版本号

private:
    JUCE_LEAK_DETECTOR (HWTUserBookVer)
};

// <UserBookStat>:用户图书统计信息
struct HWTUserBookStat
{
    int nSign; // @sign- 我对本书做的划线数量
    int nBookMark; // @bookmark- 我对本书做的书签数量
    int nPostil; // @postil- 我对本书做的批注数量
    int nComment; // @comment- 我对本书做的书评数量

private:
    JUCE_LEAK_DETECTOR (HWTUserBookStat)
};

// <ClassInfo>:班级信息
struct HWTClassInfo
{
    int nClassid; // @classid- 班级ID
    int nGrade; // @gradeno- 年级号
    juce::String strName; // @name- 名称
    juce::String strSchool; // @school- 学校
    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTClassInfo)
};

//     数据规则：
//年级号由服务端根据班级的学年和当前时间生成，已经毕业的班级的年级号为毕业学年。对于未毕业班级，年级号1-9；已经毕业//的班级，年级号大于2000。
//客户端负责根据年级号和班号生成班级名称，2003年入学的班级，班号7，2008年时年级号为5，名称为五年级七班，2009年毕业以//后年级号为2009，班级名称为2009届七班。
//一个班级的年级号在每年8月1日和后台指定班级毕业时更新。

// <UserInfo>:用户数据
struct HWTUserInfo
{
    HWEnum_UserType enUserType; // @usertype- 用户类型，参见附录常量值定义
    int nMember; // @member:1234567890		// 会员资格到期时间戳(8字节)
    juce::String strShelfNo; // @shelfno- 书架号
    juce::String strName; // @name- 姓名
    HWEnum_Gender enSex; // @sex- 性别
    int nCredits; // @credits- 积分

    void FromHWDict(const HWDict* dict);

    // EXTEND
    int ExtClass() const;
    int ExtStudentNo() const;
    std::vector<int>& ExtClasses();
    std::vector<juce::String> ExtChildren();

private:
    // 学生
    int nExtClass; // @class:12- 学生所属班级
    int nExtStudentNo; // @studentno- 学号
    std::vector<int> arrExtClasses; // @classes- 老师负责的班级，允许零个或多个
    std::vector<juce::String> arrExtChildren; // @children- 孩子的用户ID列表

private:
    JUCE_LEAK_DETECTOR (HWTUserInfo)
};


//      数据规则：
// 如果用户类型为老师，学号字段无意义(null)，用户班级可以为空或多项；
// 如果是学生，用户班级可以为空或一项，如果有班级，则必须有学号，如果班级为空，学号字段无意义(null)。

// <UserTag>:用户标签信息
struct HWTUserTag
{
    int nId; // @id- 标签ID
    juce::String strName; // @name- 标签名称

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTUserTag)
};

//      数据规则
// 用户标签值从1开始

// <UserBook>:用户图书信息
struct HWTUserBook
{
    int nId; // @id- 用户图书ID
    juce::String strBook; // @guid- GUID
    int nTag; // @tag- 所属标签ID
    time_t tTime; // @time- 收藏时间,32位时间戳
    juce::String strName; // @name- 书名

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTUserBook)
};


//      数据规则
// 图书可以没有标签，此时tag值为null

// <ReadData>:阅读数据
struct HWTReadData
{
    juce::String strBook; // @guid- 图书句柄
    int nPageNo; // @pageno- 最后阅读页码(封面：-1，封底
    juce::String strShowPage; // @showpage- 显示页码，如果为0，则代表非正文区
    int nPageCount; // @pagecount- 阅读页数
    int nDuiation; // @duration- 阅读时长
    time_t tEndTime; // @endtime- 结束阅读时间

private:
    JUCE_LEAK_DETECTOR (HWTReadData)
};

// <HistoryData>:阅读历史数据
struct HWTHistoryData
{
    juce::String strBook; // @guid- 图书句柄
    int nPageNo; // @pageno- 最后阅读页码
    time_t tTime; // @time- 最后阅读时间
    juce::String strShowPage; // @showpage- 显示页码

private:
    JUCE_LEAK_DETECTOR (HWTHistoryData)
};

// <ReadBase>:阅读基础数据
struct HWTReadBase
{
    std::vector<juce::String> arrAuthor; // @A- ["作者1","作者2"]
    int nWordCount; // @WC- "字数",
    int nPDFPageCount; // @PDFPC- "PDF页数",
    int nPDFWidth; // @PDFW- "PDF宽度",
    int nPDFHeight; // @PDFH- "PDF高度",
    Json::Value jvPDFNoInfo; // @PDFNoInfo- [页码信息],
    int nBodyPDFPageNo; // @BodyPDFPageNo- 13 正文页PDF页码
    std::vector<juce::String> arrNSDMode; // @NSDM- ["模式1","模式2"...]
    bool bExistText; // @ExistText- 是否有文本（1：有， 0：没有）
    float fHVScale; // @HVScale- 横纵比值(float类型), 默认为1.0。大于1.0则原图拉宽，否则原图拉高。
    bool bIsSupportPactMode; // @IsSupportPactMode- 是否支持紧凑模式（1：支持，0：不支持）
    int nFontWidth; // @FW- 11 常用字形宽度

    void FromJson(const Json::Value& jv);

private:
    JUCE_LEAK_DETECTOR (HWTReadBase)
};

//      数据规则：
// ?    PDFNoInfo:
// 页码信息用于将PDF页码转换为图书页码并决定显示格式，为一个json数组，格式如下：
// [显示格式,页码比值,起始页码,PDF起始页码,PDF结束页码...]
// 其中显示格式为一个整数，表示不同的图书页码显示方式；页码比值为一个整数，表示图书页码与PDF页码的映射关系；起始页码// 为从0开始的整数；PDF起始和结束页码均为从1开始的整数，要求PDF结束页码大于PDF起始页码。上述5个参数为一组，如果图书中// 间存在无页码页，则可以有多组参数。
// 显示格式可以取下列值，对应不同的页码显示方式：
// 0            1,2,3,4,5,6,7,8...
// 1            I,II,III,IV,V,VI,VII,VIII...
// 2            一,二,三,四,五,六,七,八...
// 显示格式以后可以根据需要扩充。
// 页码比值可以取下列值，其中n为大于0的正整数:
// n            1个PDF页码对应n个图书页码
// -n            n个PDF页码对应1个图书页码
// 页码比值-1和1是等价的，如果页码比值为0则置为1。
// 如果页码比值大于1，则一个PDF页码对应多个图书页码，此时应显示该PDF页码对应的图书页码范围，比如：第14-15页。
// 图书页码的计算方式：
// 对指定的PDF页码，首先根据PDF起始和结束页码找到该PDF页码所在的参数组，如果没有找到，则该PDF页码对应非正文页；如果找// 到，如果页码比值大于0，则：
// 图书页码 ＝ 起始页码 + (PDF页码 – PDF起始页码) * |页码比值|
// 如果页码比值小于0，则：
// 图书页码 ＝ 起始页码 + (PDF页码 – PDF起始页码) / |页码比值|
// 样例：
// [0,1.0,10,1,125,0,1.0,136,126,255]
// 上述样例记录了两组对应关系，页码增量为1.0，显示格式为0：
// PDF页码1－125对应显示页码10－134
// PDF页码126－255对应显示页码136－265
// 页码信息可以没有，此时直接显示PDF页码。
// ?    NSDM:
// 图书不支持的显示模式，图书在客户端显示时存在多种显示模式，目前有连续模式和翻页模式。NSDM为一个数组，记录了图书不支// 持的全部显示模式，样例：
// ["翻页"]
// 由于目前仅有两种显示模式，如果都不支持，则该图书无法显示，以后如果实现了其余显示模式，则可以有多个值。

// <BookSign>:用户划线数据
struct HWTBookSign
{
    int nId; // @id- 划线ID
    int nPageNo; // @pageno- 页码
    DWORD dwClr; // @clr- 划线颜色
    time_t tTime; // @time- 创建时间
    int nBe[2]; // @be- 划线在本页中的字符起始序号和数量
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
    HWEnum_Device enDevice; // @devicetype- (int),			// 设备类型
    juce::String strBook; // @guid- (string),
    int nPageNo; // @page- (int),
    juce::String strShowPage; // @showpage- (string),
    int nZone; // @zone- (int),
    juce::String strText; // @text- (string),				// 批注文本
    juce::String strBookText; // @booktext- (string),		// 图书原文
    int nBex; // @be- (jsonstring)			// json(开始字符、字符数量)
    int nBec;

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTBookPostil)
};

struct HWTBookComment
{
    int nID; // @id- (int),                    // 书评ID
    int nDynamicID; // @dynamicid- (int),             // 动态ID（用于获取书评周边社区信息）
    time_t tTime; // @time- (int),
    HWEnum_Device enDeviceType; // @devicetype-  (int),			// 设备类型
    juce::String strBook; // @guid- (string),
    juce::String strText; // @text-(string),				// 书评文本

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTBookComment)

};
// 数据规则：
// clr代表划线颜色，无符号整数，格式为0xAARRGGBB，最高字节位始终设置为0xFF。
// pageno代表划线所在PDF页码，绘制显示页码前，需使用阅读基础数据中的PDFNoInfo字段值进行PDF页码到显示页码的转换。
// be为划线在本页中的字符范围，具体参见E.实现说明。

// <BookMark>:用户书签数据
struct HWTBookMark
{
    int nPageNo; // @pageno- 页码
    juce::String strTitle; // @title- 书签名称
    DWORD dwClr; // @clr- 书签颜色
    time_t tTime; // @time- 创建时间

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTBookMark)
};

// 数据规则：
// clr代表划线颜色，采用RGB格式，最高位始终设置为0xFF。
// pageno代表划线所在PDF页码

// <DynamicData>:动态数据
struct HWTDynamicData
{
    int nId; // @id- 动态ID
    int nType; // @type- 动态类型
    time_t tTime; // @time- 创建时间
    int nReplyCount; // @replycount- 回复数
    int nReviewCount; // @reviewcount- 点评数量，老师的评论数量
    int nFlowerCount; // @flowercount- 红花数量
    int nFavourCount; // @favourcount- 点赞数量
    HWDict dictContent; // @content- 动态内容，参见附录动态数据格式定义
    HWEnum_Device enDeviceType; // @devicetype- 设备类型
    unsigned long uIPaddr; // @ipaddr- IP地址

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTDynamicData)
};

// <DynamicOP>:动态操作状态
struct HWTDynamicOP
{
    int nFlower; // @flower- "1"
    int nFavour; // @favour- "0"

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTDynamicOP)
};

//      数据规则
// dynamicop定义了一个用户对动态的操作状态，目前支持红花和点赞，0表示未添加，1表示已经添加。注意：能否进行红花操作有// 额外的限制：仅允许老师给学生的动态添加红花，而且红花数量限制最大为10
// <DynamicInfo>:动态信息
struct HWTDynamicInfo
{
    HWTUserInfo user; // @user- 动态属主信息
    HWTDynamicData data;// @data- 动态数据
    HWTDynamicOP op; // @op- 动态数据请求者对动态的操作状态

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTDynamicInfo)
};

// <NoteData>:笔记数据
struct HWTNoteData
{
    int nId; // @id- 笔记ID
    int nType; // @type- 笔记类型
    time_t tTime; // @time- 创建时间
    Json::Value jvContent; // @content- 笔记内容
    HWEnum_Device enDeviceType; // @devicetype- 设备类型
    unsigned long ipAddr; // @ipaddr- IP地址

private:
    JUCE_LEAK_DETECTOR (HWTNoteData)
};

//      数据规则
// 笔记数据实际上是动态数据的子集，去掉了社区相关的计数字段，使笔记可以离线访问。目前笔记仍然以动态方式实现，这里的笔// 记类型就是动态类型(20-21)，笔记内容就是动态内容。

// <CommInfo>:动态社区信息
struct HWTCommInfo
{
    int nReplyCount; // @replycount- 回复数
    int nReviewCount; // @reviewcount- 点评数量，老师的评论数量
    int nFlowerCount; // @flowercount- 红花数量
    int nFavourCount; // @favourcount- 点赞数量

private:
    JUCE_LEAK_DETECTOR (HWTCommInfo)
};

// <ReplyData>:回复数据
struct HWTReplyData
{
    HWTUserInfo destUser; // @destuser- 回复目标用户
    int nId; // @id- 回复ID
    int nType; // @type- 回复类型
    time_t tTime; // @time- 创建时间
    juce::String strText; // @text- 回复文本
    HWEnum_Device enDeviceType; // @devicetype- 设备类型
    unsigned long ipAddr; // @ipaddr- IP地址

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTReplyData)
};

// <ReplyInfo>:回复信息
struct HWTReplyInfo
{
    HWTUserInfo user; //     @user  >,
    HWTReplyData data; //     @data  >

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTReplyInfo)
};

// <ScoreData>:评分数据
struct HWTScoreData
{
    int nStars; // @stars- 评分星级
    time_t tTime; // @time- 创建时间戳

private:
    JUCE_LEAK_DETECTOR (HWTScoreData)
};

// <ScoreInfo>:评分信息
struct HWTScoreInfo
{
    HWTUserInfo user; // @user- 用户信息
    HWTScoreData data; // @data- 评分数据

private:
    JUCE_LEAK_DETECTOR (HWTScoreInfo)
};

// <BookScore>:图书评分
struct HWTBookScore
{
    int nCount; // @count- 评分数量
    float fStars; // @stars- 平均星级
    int nMyStar; // @mystar- 我的评分星级，0表示未评分

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTBookScore)
};

// <BookStat>:图书统计信息
struct HWTBookStat
{
    int nCollect; // @collect- 收藏次数         
    int nPostil; // @postil- 批注数量
    int nComment; // @comment- 书评次数
    int nScore; // @score- 评分次数
    float fStars; // @stars- 平均评分星级
    int nRead; // @read- 阅读热度
    int nCommend; // @commend- 推荐次数

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTBookStat)
};

// <UserStat>:用户统计信息
struct HWTUserStat
{
    int nCollect; // @collect- 藏书数量
    int nAttention; // @attention- 关注数量
    int nFans; // @fans- 粉丝数量
    int nPostil; // @postil- 批注数量
    int nComment; // @comment- 书评数量
    // int nScore; // @score- 评分数量
    int nFlower; // @flower- 红花数量

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTUserStat)
};


// <ClassStat>:班级统计
struct HWTClassStat
{
    int nDuration; // @duration- 阅读时长
    int nPage; // @page- 阅读页数
    int nPostil; // @postil- 批注数量
    int nComment; // @comment- 书评数量

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTClassStat)
};

//      数据规则
// 班级统计是用户阅读、登录统计数据的综合，再加上学生是否注册了手机号。

// <NotifyData>:通知数据
struct HWTNotifyData
{
    HWEnum_NotifyType enType; // @type- 通知类型，参见附录
    time_t tTime; // @time- 通知产生的时间
    Json::Value jvContent; // @content- 通知内容，参见附录通知数据格式定义

private:
    JUCE_LEAK_DETECTOR (HWTNotifyData)
};

//      数据规则：
// lastletter为读取者与user之间的最后一次私信，其中私信文本为摘要，最长50个字符。注意：最后一次私信数据中的类型，发送// (0)代表读取者发送给user的私信，接收(1)代表读取者从user接收的私信。

// <LetterData>:私信数据
struct HWTLetterData
{
    int nId; // @id- 私信ID
    int nType; // @type- 私信类型，参见附录
    time_t tTime; // @time- 发送时间
    juce::String strText; // @text- 私信文本

private:
    JUCE_LEAK_DETECTOR (HWTLetterData)
};

//      数据规则：
// 附录中的私信类型以私信数据的读取者为基点，类型为发送(0)表示读取者发送给对方的私信，接收(1)// 代表读取者从对方接收的私信

// <LetterUserInfo>:私信用户信息
struct HWTLetterUserInfo
{
    HWTUserInfo user; // @user- 用户信息
    int nCount; // @count- 与该用户的私信总数量
    int nNetCount; // @newcount- 从该用户接收的未读私信数量
    HWTLetterData lastLetter; // @lastletter- 最后一次私信(可以是发送
    // //里面的私信内容为摘要，最长50字

private:
    JUCE_LEAK_DETECTOR (HWTLetterUserInfo)
};

// <MessageData>:消息数据
struct HWTMessageData
{
    int nId; // @id- 消息ID
    int nType; // @type- 消息类型，参见附录
    time_t tTime; // @time- 发送时间
    Json::Value jvContent; // @content- 消息内容，参见附录消息数据格式定义

private:
    JUCE_LEAK_DETECTOR (HWTMessageData)
};

// <CommendData>:班级推荐数据
struct HWTCommendData
{
    time_t tTime; // @time- 推荐时间
    juce::String strText; // @text- 推荐附言
    int nCount; // @count- 图书数量
    std::vector<juce::String> arrBooks; // @books- 推荐图书列表
    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTCommendData)
};

// <CommendInfo>:班级推荐信息
struct HWTCommendInfo
{
    HWTUserInfo user; // @user- 老师用户信息
    HWTCommendData commend; // @commend- 班级推荐数据

private:
    JUCE_LEAK_DETECTOR (HWTCommendInfo)
};

// <CommendBook>:推荐图书
struct HWTCommendBook
{
    time_t tTime; // @time- 推荐时间
    juce::String strBook; // @guid- 推荐图书

private:
    JUCE_LEAK_DETECTOR (HWTCommendBook)
};

// <TypeTree>:类别树
struct HWTTypeTree
{
    Json::Value jvTypeTree; // [10001,"社科/文艺",727,[...]

private:
    JUCE_LEAK_DETECTOR (HWTTypeTree)
};

//      数据规则    
// 小学版本中的类别树仅提供2层类别，客户端不必处理大于2层类别的数据。

// <BookInfo>:图书信息
struct HWTBookInfo
{
    juce::String strBook; // @guid- guid
    //int nDataVer; // @dataver- 图书信息版本
    int nTypeId; // @typeid- 图书类别ID
    int nPageCount; // @pagecount- 图书页数
    int nWordCount; // @wordcount- 图书字数
    int nDataSize; // @datasize- 数据大小(B
    time_t tPressTime; // @presstime- 出版时间(32位
    time_t tOnlineTime; // @onlinetime- 上架时间(32位
    bool bOnline; // @bonline- 是否在架
    juce::String strName; // @name- 书名
    std::vector<juce::String> arrAuthors; // @authors- 作者(json格式，字符
    juce::String strPress; // @press- 出版社

    void FromJson(const Json::Value& jv);
    void FromHWDict(const HWDict* dict);
    HWTBookInfo& operator = (const HWTBookInfo& info);

private:
    JUCE_LEAK_DETECTOR (HWTBookInfo)
};

// <Catalog>:图书目录
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

//      数据规则：
// [目录名,PDF页码,显示页码,子目录集合]
// 目录名是一个string，PDF页码是一个从1开始的整数，显示页码是一个string，子目录集合是一个目录定义的array，没有子目录// 是为空的array，即[]。
// 目录表的最顶级为根，根的目录定义中目录名和显示页码为空的string，即””，PDF页码为0。

// <PageOffSets>:全文分页字符索引
struct HWTPageOffSets
{
    // 数据定义：
    // 前1页字符数量(4B)|前2页字符数量(4B)|...
    int n1PageWordCount; 
    int n2PageWordCount; 

private:
    JUCE_LEAK_DETECTOR (HWTPageOffSets)
};

// <historydata>:阅读历史数据
struct HWTReadHistoryData
{
    juce::String strBook;
    juce::String strShowPageNo;    // 显示页码
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
    juce::String strCode; // “code”: ”129621” //商品编码
    juce::String strName; // “name”:”一年期会员”,									//商品名称
    juce::String strDesc; // “desc”:”购买后，学生和相关家长的会员资格延长一年”,		//商品描述
    int nPrice; // “price”:6800,											//商品价格(单位：分)
    int nDiscount; // “discount”:5800										//商品折扣价(单位：分)

    void FromHWDict(const HWDict* dict);

private:
    JUCE_LEAK_DETECTOR (HWTIemInfo)
};