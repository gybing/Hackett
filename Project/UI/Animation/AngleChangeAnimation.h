#pragma once

// 单个对象任意角度的旋转，继承自B3DAnimation.可实例化
class AngleChangeAnimation
	: public B3DAnimation
{
public:
	AngleChangeAnimation(void);
	~AngleChangeAnimation();

	// 绑定ImageObject对象
	void BindObj(ImageObject obj);

	// 获得绑定的对象
	ImageObject GetBindObj(void);

	// 设置旋转的角度信息
	void SetKeyFrameAngle(float startXangle, float endXangle, float startYangle, float endYangle, float startZangle, float endZangle);
	void SetKeyFrameRange(float startRangeX, float endRangeX, float startRangeY, float endRangeY, float startRangeZ, float endRangeZ);
	
protected:
	float m_fSrcAnglex; // 源位置的x轴角度
	float m_fSrcAngley; // 源位置的y轴角度
	float m_fSrcAnglez; // 源位置的z轴角度
	float m_fDestAnglex; // 目标位置的x轴角度
	float m_fDestAngley; // 目标位置的y轴角度
	float m_fDestAnglez; // 目标位置的z轴角度
};
