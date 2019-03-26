#pragma once

// 托盘图标类继承自SystemTrayIconComponent类
class CXTaskbarComponent  : public juce::SystemTrayIconComponent
{
public:
    CXTaskbarComponent()
    {
        setIconImage (CXIMAGE_CACHE_LOAD(L"hanwenbook.png")); // 设置托盘图标所用的图像
        
        // 鼠标指向时显示提示信息
        setIconTooltip (ProjectInfo::projectTitle);
    }
    
    // 鼠标按下，本例为弹出菜单
    void mouseDown (const juce::MouseEvent&)
    {
        juce::PopupMenu m;
        m.addItem (1, L"显示主程序");
        m.addItem (2, L"官方网站");
        m.addItem (3, L"切换用户");
        m.addItem (4, L"关于");
        m.addItem (5, L"设置");
        m.addItem (6, L"退出");
        
        const int result = m.show();
        
        switch (result)
        {
            case 1:
                //ReadWindowHelper::getInstance()->showWindow();
                break;
            case 3:
                LoginWindowHelper::getInstance()->asyncRelogin();
                break;
            case 4:
                juce::AlertWindow::showMessageBox(juce::AlertWindow::InfoIcon, L"关于", L"乐学阅读");
                break;
            case 5:
                OptionComponent::showOptionWindow(nullptr, 1);
                break;
            case 6:
                juce::JUCEApplication::getInstance()->systemRequestedQuit();
                break;
            default:
                break;
        }            
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CXTaskbarComponent)
};