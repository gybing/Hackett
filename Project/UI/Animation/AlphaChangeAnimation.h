#pragma once

// 动画的三级模块，继承自KeyFrameAnimation。实现对象渐隐渐现动态效果。可实例化
// 成员方法列表
class AlphaChangeAnimation
	: public KeyFrameAnimation
{	
public:
	AlphaChangeAnimation(void);
	~AlphaChangeAnimation();

	// 绑定ImageObject到动画上
	void BindObj(ImageObject obj);

	// 获得绑定在动画上的对象
	ImageObject GetBindObj(void);

	// 设置开始和结束的alpha通道信息
	void SetKeyFrameAlpha(long startAlpha, long endAlpha);

protected:

	// 源alpha值 0~255
	long m_nSrcAlpha;

	// 目标alpha值 0~255
	long m_nDestAlpha;
};
