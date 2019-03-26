#ifndef LXPROTOCOL_H_INCLUDED
#define LXPROTOCOL_H_INCLUDED

// 主菜单按钮异步广播事件
class LXMainMenuBroadcaster : public ChangeBroadcaster
{
public:
    int getCurrentMenu() { return currentSelectMenuIndex; }
    LXMainMenuBroadcaster() { currentSelectMenuIndex = -1; }

public:
    class Listener : public ChangeListener
    {
    public:
        virtual void changeMenuCallback(int index) = 0;
        Listener() {}
        ~Listener() {}
    private:
        virtual void changeListenerCallback(ChangeBroadcaster* source) override
        {
            if (LXMainMenuBroadcaster* broadcaster = static_cast<LXMainMenuBroadcaster*>(source))
                changeMenuCallback(broadcaster->getCurrentMenu());
        }
    };

protected:
    void setCurrentMenu(int idx)
    {
        currentSelectMenuIndex = idx;
        sendChangeMessage();
    }

private:
    int currentSelectMenuIndex;
};


#endif  // LXPROTOCOL_H_INCLUDED
