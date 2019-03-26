#include "AltQHeader.h"
#include "MainComponent.h"
#include "native/AltQNativeHelper.h"
#include "AltQExtentions.h"

//==============================================================================
class AutoQApplication  : public JUCEApplication
{
public:
    //==============================================================================
    AutoQApplication() {}

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return false; }

    //==============================================================================
    void initialise (const String& commandLine) override
    {
        // 设置执行文件路径为当前目录
        File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().setAsCurrentWorkingDirectory();

        // 初始化核心模块
        AltQModuleCore::getInstance();
        
        // 初始化扩展模块
        AltQExtentions::getInstance()->Initialize();

        // 创建主窗口
        mainWindow = new MainWindow (getApplicationName());

        // 快捷键
        AltQNative::RegisterHotKey(mainWindow->getPeer()->getNativeHandle(), IDB_HOTKEY, QMOD_ALT, AQ_HOTKEY);
    }

    void shutdown() override
    {
        AltQNative::UnregisterHotKey(mainWindow->getPeer()->getNativeHandle(), IDB_HOTKEY);
        if (mainWindow) mainWindow = nullptr;

        AltQExtentions::getInstance()->Finalize();
        AltQExtentions::deleteInstance();
        AltQModuleCore::deleteInstance();
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
        
    }

    //==============================================================================
    class MainWindow : public DocumentWindow
    {
    public:
        MainWindow(String name) : DocumentWindow(name, Colours::white, 0)
        {
#ifdef JUCE_WINDOWS
            juce::Desktop::getInstance().getDefaultLookAndFeel().setDefaultSansSerifTypefaceName(CharPointer_UTF8("\xe5\xbe\xae\xe8\xbd\xaf\xe9\x9b\x85\xe9\xbb\x91"));
#else
            juce::Desktop::getInstance().getDefaultLookAndFeel().setDefaultSansSerifTypefaceName("STHeitiSC-Light");
#endif // ALTQ_WINDOWS

            setAlwaysOnTop(true);
            setBackgroundColour(Colour(0xEFFFFFFF));
            setResizeLimits(1, 1, 32768, 32768);
            setUsingNativeTitleBar(false);
            setTitleBarHeight(0);
            setWantsKeyboardFocus(false);
            setContentOwned(new MainContentComponent(), true);

            const Rectangle<int> parentArea(Desktop::getInstance().getDisplays().getMainDisplay().userArea);

            setBounds(parentArea.getCentreX() - getWidth() / 2, parentArea.getCentreY() - AltQConstHeight / 2, getWidth(), getHeight());
            trayIcon = new AQTrayIcon(*this);
            trayIcon->setIconImage(AltQLoadImage(AutoQ_png));
            AltQNative::SetWindowTransparent(getPeer()->getNativeHandle());
            setVisible (true);
        }

        BorderSize<int> getBorderThickness() override
        {
            return BorderSize<int>(1);
        }

        virtual void focusLost(FocusChangeType cause) override
        {
            focusOfChildComponentChanged(cause);
        }

        virtual void focusOfChildComponentChanged(FocusChangeType cause)
        {
            if (!getCurrentlyFocusedComponent())
                setVisible(false);
        }

        virtual bool keyPressed(const KeyPress& key)
        {
            if (key.isKeyCode(KeyPress::escapeKey))
            {
                if (isVisible())
                {
                    setVisible(false);
                    return true;
                }
                return true;
            }
            return false;
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        class AQTrayIcon : public SystemTrayIconComponent
        {
        public:
            AQTrayIcon(MainWindow& o) : owner(o) {}

            virtual void mouseDown(const MouseEvent& event) 
            {
                if (event.mods.isRightButtonDown())
                {
                    bool isStartup = AltQNative::GetStartupBoot();

                    PopupMenu m;
                    m.addItem(1, L"搜索 (Alt+Q)");
                    m.addItem(2, L"开机启动", true, isStartup);
                    m.addItem(3, L"捐助");
                    m.addItem(4, L"关于");
                    m.addSeparator();
                    m.addItem(5, L"退出");

                    switch (m.show())
                    {
                    case 1:
                        if (!owner.isVisible())
                            owner.setVisible(true);
                        break;
                    case 2:
                        AltQNative::SetStartupBoot(!isStartup);
                        break;
                    case 3:
                        {
                            class AltQDonet : public Component
                            {
                            public:
                                AltQDonet()
                                {
                                    addAndMakeVisible(codeComp1 = new ImageComponent());
                                    addAndMakeVisible(codeComp2 = new ImageComponent());
                                    codeComp1->setImage(AltQLoadImage(weixin_png), RectanglePlacement::fillDestination);
                                    codeComp2->setImage(AltQLoadImage(zhifubao_png), RectanglePlacement::fillDestination);
                                    setWantsKeyboardFocus(false);
                                    setInterceptsMouseClicks(false, false);
                                    setSize(550, 300);
                                }
                                ~AltQDonet(){}

                            protected:
                                virtual void resized() override
                                {
                                    const Rectangle<int> bounds = getBounds();
                                    const int margin = 10;
                                    const int size = jmin(((bounds.getWidth() / 2) - margin), (bounds.getHeight() - margin));
                                    codeComp1->setBounds((bounds.getWidth() / 2) - size - margin, ((bounds.getHeight() - size) / 2) - margin, size, size);
                                    codeComp2->setBounds((bounds.getWidth() / 2) + margin, ((bounds.getHeight() - size) / 2) - margin, size, size);
                                }
                            private:
                                ScopedPointer<ImageComponent> codeComp1;
                                ScopedPointer<ImageComponent> codeComp2;
                            };

                            {
                                AlertWindow* aw = new AlertWindow(L"捐助 - 您的支持是我们最大的动力！", "", AlertWindow::NoIcon, nullptr);
                                aw->addButton(L"确定", 0,
                                    KeyPress(KeyPress::escapeKey),
                                    KeyPress(KeyPress::returnKey));
                                ScopedPointer<AltQDonet> cm = new AltQDonet;
                                aw->addCustomComponent(cm);

                                ScopedPointer<Component> alertBox(aw);
#if JUCE_MODAL_LOOPS_PERMITTED
                                alertBox->runModalLoop();
#elif
                                alertBox->enterModalState(true, callback, true);
                                alertBox.release();
#endif
                            }
                        }
                        break;
                    case 4:
                        Process::openDocument("http://www.altquery.com/", "");
                        break;
                    case 5:
                        JUCEApplication::getInstance()->systemRequestedQuit();
                        break;
                        
                    default: break;
                    }
                    return; 
                }

                if (!owner.isVisible()) 
                    owner.setVisible(true);
            }
            virtual void mouseDrag(const MouseEvent& event){}
            virtual void mouseUp(const MouseEvent& event){}
            virtual void mouseDoubleClick(const MouseEvent& event) {}
            virtual void mouseWheelMove(const MouseEvent& event, const MouseWheelDetails& wheel){}

        protected:
            MainWindow& owner;
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AQTrayIcon)
        };
    private:
        ScopedPointer<AQTrayIcon> trayIcon;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    ScopedPointer<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(AutoQApplication)
