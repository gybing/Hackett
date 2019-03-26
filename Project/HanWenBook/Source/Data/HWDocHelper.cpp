#include "JuceHeader.h"
#include "HWDocHelper.h"

// 学校相关
const juce::String HWRes_School_GradeNames[] =
{ L"一年级", L"二年级", L"三年级", L"四年级", L"五年级", L"六年级", L"七年级", L"八年级", L"九年级" };

const juce::String HWRes_School_ClassNames[] =
{ L"1班", L"2班", L"3班", L"4班", L"5班", L"6班", L"7班", L"8班", L"9班" };

juce_ImplementSingleton(HWDocHelper)

    bool HWDocHelper::CheckResponseError(HWEnum_ReqID reqID, HWEnum_ErrorID err,HWEnum_BusiID busi )
{
    // #ifndef _DEBUG
    //     return true;
    // #endif // _DEBUG

    // 过滤
    if (reqID == HWReq_System_CheckVersion ||
        err == HWError_SUCCEED ||
        (err==HWError_CUSTOMINFO && busi == HWBUSI_NO_READ_POWER))
    {
        return true;
    }


    juce::String strErr, strTitle;

    switch (err)
    {
    case HWError_SERVERFAIL:
        {
            strTitle=L"网站请求失败";
            strErr=L"服务端异常";
            break;
        }
    case HWError_INTERNAL:
        {
            strTitle=L"内部错误";
            strErr=L"客户端后台异常";
            break;
        }
    case HWError_INVALIDARG:
        {
            strTitle=L"参数错误";
            strErr=L"客户端请求参数无效";
            break;
        }
    case HWError_MAINTENANCE:
        {
            strTitle=L"服务端正在维护";
            strErr=L"服务端维护，请稍后再登录。";
            goto relogin;
            break;
        }
    case HWError_NETWORKFAIL:
        {
            strTitle=L"网络访问失败";
            strErr=L"网络连接异常，请重新登录";
            goto relogin;
            break;
        }
        //     case HWError_CANCEL:
        //         {
        //             strTitle=L"过期响应";
        //             strErr=L"";
        //             break;
        //         }
    case HWError_KICKED:
        {
            strTitle=L"被踢下线";
            strErr=L"您的帐号在另一地点登录";
            goto relogin;
            break;
        }
    case HWError_TOKENEXPIRED:
        {
            strTitle=L"汉文令牌过期";
            strErr=L"长时间未登录，请重新登录";
            goto relogin;
            break;
        }
    case HWError_THIRDTOENEXPIRED:
        {
            strTitle=L"第三方令牌过期";
            strErr=L"长时间未登录，请重新登录";
            goto relogin;
            break;
        }
    case HWError_ONLINELIMIT:
        {
            strTitle=L"在线人数超出限制";
            strErr=L"登录失败";
            break;
        }
    case HWError_CUSTOMINFO:
        {
            strTitle=L"业务逻辑错误";

            switch (busi)
            {
            case HWBUSI_INTERNAL:
                {
                    strErr = L"标准业务逻辑错误";
                    break;
                }
            case HWBUSI_PARAMETER:
                {
                    strErr = L"输入参数无效";
                    break;
                }

                // 通用
            case HWBUSI_PIC_FORMAT_INVALID:
                {
                    strErr = L"上传图片格式无效";
                    break;
                }
            case HWBUSI_SECCODE_ERROR:
                {
                    strErr = L"校验码错误";
                    break;
                }
            case HWBUSI_BOOK_DROP:
                {
                    strErr = L"图书下架";
                    break;
                }
            case HWBUSI_OPEATE_FORBID:
                {
                    strErr = L"用户操作被禁止";
                    break;
                }
            case HWBUSI_TEXT_TOO_LONG:
                {
                    strErr = L"用户输入文本过长";
                    break;
                }
            case HWBUSI_NO_READ_POWER:
                {
                    strErr = L"无阅读权限";
                    break;
                }

                // 登录注册1xx
            case OBERROR_BUSI_DEVICE_CODE_ERROR:
                {
                    strErr = L"设备特征码无效";
                    break;
                }
            case OBERROR_BUSI_THIRD_REQTOKEN_ERROR:
                {
                    strErr = L"第三方请求令牌无效";
                    goto relogin;
                    break;
                }

                //用户2xx

                //藏书3xx
            case HWBUSI_SHELF_FORBID:
                {
                    strErr = L"藏书禁止访问，学生不能访问老师的藏书";
                    break;
                }
            case HWBUSI_TAG_NUM_LIMIT:
                {
                    strErr = L"标签数量超限";
                    break;
                }
            case HWBUSI_TAG_NAME_TOOLONG:
                {
                    strErr = L"标签名称过长";
                    break;
                }
            case HWBUSI_BOOK_ALREADY_COLLECTED:
                {
                    strErr = L"已经收藏过本图书，不能再次收藏";
                    break;
                }
            case HWBUSI_BOOK_REACH_LIMIT:
                {
                    strErr = L"藏书数量超限(30000本)";
                    break;
                }

                //划线书签5xx
            case HWBUSI_BOOKSIGN_REACH_LIMIT:
                {
                    strErr = L"本书内划线数量已超限";
                    break;
                }
            case HWBUSI_BOOKMARK_ALREADY:
                {
                    strErr = L"目标页码已存在书签，不能在同一页重复添加书签";
                    break;
                }
            case HWBUSI_BOOKMARK_REACH_LIMIT:
                {
                    strErr = L"本书内划线数量已超限";
                    break;
                }
            case HWBUSI_BOOKMARK_NOTEXIST:
                {
                    strErr = L"本书内划线数量已超限";
                    break;
                }
            case HWBUSI_BOOKMARK_TOOLONG_TITLE:
                {
                    strErr = L"书签标题过长";
                    break;
                }

                //笔记6xx

                //关系7xx
            case HWBUSI_ATTENT_ALREADY:
                {
                    strErr = L"你已关注了对方，不能再次关注";
                    break;
                }
            case HWBUSI_ATTENT_REACH_LIMIT:
                {
                    strErr = L"超出了可关注最大人数";
                    break;
                }
            case HWBUSI_ATTENT_NOT_ATTENT:
                {
                    strErr = L"你没有关注对方，操作失败 ";
                    break;
                }

                //社区8xx

                //动态9xx
            case HWBUSI_DYNAMIC_DELETE_FORBID:
                {
                    strErr = L"动态不能删除";
                    break;
                }
            case HWBUSI_DYNAMIC_FLOWER_FORBID:
                {
                    strErr = L"动态不能加红花";
                    break;
                }
            case HWBUSI_DYNAMIC_FLOWER_ALREADY:
                {
                    strErr = L"动态已经加过红花";
                    break;
                }
            case HWBUSI_DYNAMIC_FLOWER_NOTEXIST:
                {
                    strErr = L"动态还没有加过红花";
                    break;
                }
            case HWBUSI_DYNAMIC_FAVOUR_ALREADY:
                {
                    strErr = L"动态已经加过点赞";
                    break;
                }
            case HWBUSI_DYNAMIC_FAVOUR_NOTEXIST:
                {
                    strErr = L"动态还没有加过点赞";
                    break;
                }
            case HWBUSI_DYNAMIC_FLOWER_LIMIT:
                {
                    strErr = L"红花超过限制(10)";
                    break;
                }

                //私信消息10xx

                //推荐11xx
            case HWBUSI_RECOMMEND_CLASS_LIMIT:
                {
                    strErr = L"只能对所在班级推荐";
                    break;
                }
            case HWBUSI_RECOMMEND_STUDENT_LIMIT:
                {
                    strErr = L"只能对学生进行图书推荐";
                    break;
                }
            case HWBUSI_RECOMMEND_DESTNUM_LIMIT:
                {
                    strErr = L"图书推荐的用户数量超过限制(1000)";
                    break;
                }
            case HWBUSI_RECOMMEND_CLASSNUM_LIMIT:
                {
                    strErr = L"班级推荐的班级数量超过限制(100)";
                    break;
                }
            case HWBUSI_RECOMMEND_BOOKNUM_LIMIT:
                {
                    strErr = L"班级推荐的图书数量超过限制(100)";
                    break;
                }
            default:
                break;
            }
            break;
        }
    default:
        break;
    }

    if (!strErr.isEmpty())
    {
        juce::AlertWindow::showMessageBox(juce::AlertWindow::AlertIconType::WarningIcon, strErr, strTitle);
    }
    return true;

relogin:
    if (!strErr.isEmpty())
    {
        juce::AlertWindow::showMessageBox(juce::AlertWindow::AlertIconType::WarningIcon, strErr, strTitle);
    }

    LoginWindowHelper::getInstance()->asyncRelogin();
    return true;
}


const juce::String* HWDocHelper::GetGradeNameFromeID(int nGradeId)
{
    int nIdx = nGradeId - 1;
    if (nIdx >=0 && nIdx < sizeof(HWRes_School_GradeNames))
        return &HWRes_School_GradeNames[nIdx];

    return NULL;
}

const juce::String* HWDocHelper::GetClassNameFromeID(int nClassId)
{
    int nIdx = nClassId - 1;
    if (nIdx >=0 && nIdx < sizeof(HWRes_School_ClassNames))
        return &HWRes_School_ClassNames[nIdx];
    return NULL;
}

int HWDocHelper::GetGradeIDFromName(const wchar_t* wszGrade)
{
    for (unsigned int idx=0; idx < sizeof(HWRes_School_GradeNames); ++idx)
    {
        if (HWRes_School_GradeNames[idx] == juce::String(wszGrade))
        {
            return idx;
        }
    }
    return -1;
}

int HWDocHelper::GetClassIDFromName(const wchar_t* wszClass)
{
    for (unsigned int idx=0; idx < sizeof(HWRes_School_ClassNames); ++idx)
    {
        if (HWRes_School_ClassNames[idx] == juce::String(wszClass))
        {
            return idx;
        }
    }
    return -1;
}

const juce::String* HWDocHelper::GetUserReadLevel( int nReadCount, HWEnum_Gender enSex )
{
    int nLevelCount = sizeof(CSHWUserLevelHold)/sizeof(CSHWUserLevelHold[0]);
    for (int i = nLevelCount - 1; i >= 0; --i)
    {
        if (nReadCount >= CSHWUserLevelHold[i])
        {
            return &CSHWUserLevel[i + nLevelCount * ((enSex == HWGender_Male) ? 0 : 1)];
        }
    }
    return NULL;
}

void HWDocHelper::GetTimeFormat(time_t t, juce::String& strFmt, bool bShort)
{

}

void HWDocHelper::GetHeadPicRequestUrl( juce::String& strUrl, const char* szUserID)
{
    strUrl << "http://image.lejiaolexue.com/userlogo/" << szUserID << "/3";
}

//------------------------------------------------------------------------------
void HWDocHelper::GetRomanNum( unsigned int n, juce::String& strNum )
{
    // 默认假设nPageNo小于1000
    static juce::String Roman[] = {L"CM",L"D",L"CD",L"C",L"XC",L"L",L"XL",L"X",L"IX",L"V",L"IV",L"I"};   // 罗马数组
    unsigned int Arabic[] = {900,500,400,100,90,50,40,10,9,5,4,1};  // 阿拉伯数组

    int i = 0;
    while(n > 0)
    {
        while(n >= Arabic[i])
        {
            strNum.append(Roman[i], Roman[i].length());  // 不断将对应的罗马数字追加入变量
            n -= Arabic[i];
        }
        i+=1;
    }
}

//------------------------------------------------------------------------------
void HWDocHelper::GetChineseNum( unsigned int n, juce::String& strNum )
{
    // 默认假设nPageNo小于1000
    static wchar_t* chDes[] = { L"零", L"一", L"二", L"三", L"四", L"五", L"六", L"七", L"八", L"九"};
    int nBai = n/100;
    int nShi = (n - nBai*100)/10;
    int nGe = n%10;

    strNum.clear();
    if (nBai != 0)
    {
        strNum << chDes[nBai] << L"百" << chDes[nShi] << L"十" << chDes[nGe];
    }
    else
    {
        if (nShi != 0)
        {
            strNum << chDes[nShi] << L"十" << chDes[nGe];
        }
        else
        {
            strNum = chDes[nGe];
        }
    }
}

void HWDocHelper::GetUserLoginStatStr( int nCount, time_t tTime, juce::String& strStat )
{
    if (nCount <= 0)
    {
        strStat = L"从未登录";
        return;
    }




}
