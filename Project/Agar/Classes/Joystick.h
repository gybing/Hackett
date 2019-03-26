#ifndef __AGAR_JOYSTICK_HH__
#define __AGAR_JOYSTICK_HH__

#include "cocos2d.h"
USING_NS_CC;

enum JoystickEnum
{
    DEFAULT,
    D_UP,
    D_DOWN,
    D_LEFT,
    D_RIGHT,
    D_LEFT_UP,
    D_LEFT_DOWN,
    D_RIGHT_UP,
    D_RIGHT_DOWN
};

class Joystick : public Layer
{
public:
    /** ���ӓu���� */
    void onRun();
    /** ������� */
    void onDisable();
    /** �O�������돽���������돽��ҡ����ʧЧ */
    void setDieRadius(float radius);
    /** �O�ßoЧ�^��돽������ڟoЧ�^��ȣ������ã� */
    void setFailRadius(float radius);
    /** �Ƿ��@ʾ�ױP���|�c */
    void setVisibleJoystick(bool visible);
    /** �Ƿ�����׃�Q�u������λ�ã�������Ļ��ÿһ�ΰ�����˕r�����ˌ��Ǔu���������ˣ��Ƅӕr������׃�u�������ˣ�ֱ���´ΰ������ */
    void setAutoPosition(bool value);
    /** ���{����ָ� */
    std::function<void(JoystickEnum)> onFixedDirection;
    std::function<void(Vec2)> onDirection;
    /** �o�B������������Ҫ����ױP���|�c�DƬ·���� */
    static Joystick* create(std::string chassisPath,std::string dotPath);
    /** ��ʼ���u��������Ҫ����ױP���|�c�DƬ·���� */
    void initWithJoystick(std::string chassisPath,std::string dotPath);
protected:
    /** ��Ч�^��돽 */
    float _radius;
    /** ʧЧ�^��돽 */
    float _failradius;
    /** �Ƿ������Ч�^��돽 */
    bool isDieRadius;
    /** �Ƿ�����׃�Q�u�������� */
    bool isAutoPosition;
    bool isTouchVisible;
    /** �ױP */
    Sprite* _chassis;
    /** �|�c */
    Sprite* _touchDot;
    EventListenerTouchOneByOne* listener;
    
    bool onTouchBegan(Touch* touch,Event* event);
    void onTouchMoved(Touch* touch,Event* event);
    void onTouchEnded(Touch* touch,Event* event);
    /** 
    1���O���|�c�����Д��Ƿ��ڟoЧ�^��ȣ�����ڟoЧ�^��ȣ������ã�
    2���l�ͽǶ�׃����������ڟoЧ�^��ȣ� */
    void setTouchDotPosition(Vec2 vec1,Vec2 vec2);
    /** 
    1��Ӌ��u�����˷���
    2���l�ͽǶ�׃�������{����׃������ */
    /** ���{�]�ԵıO ���� */
    void callDirectionFun(Vec2 position);
    /** ���ã������Ȳ��� DEFAULT�r�����ã� */
    void resetState();
    
};

#endif // __AGAR_JOYSTICK_HH__