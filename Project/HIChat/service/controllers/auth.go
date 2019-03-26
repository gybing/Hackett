package controllers

import (
	"time"

	M "../modules"
	"github.com/labstack/echo"
	"gopkg.in/mgo.v2/bson"
)

// 注册
func Regist(c echo.Context) error {
	Device := c.QueryParam("Device")
	Mobile := c.QueryParam("Mobile")
	Pass := c.QueryParam("Pass")
	Name := c.QueryParam("Name")
	Face := c.QueryParam("Face")

	// 参数检查
	if Device == "" {
		return Response(c, InvalidParameter, "设备参数错误", nil)
	}

	if Mobile == "" {
		return Response(c, InvalidParameter, "手机号错误", nil)
	}

	if Pass == "" {
		return Response(c, InvalidParameter, "密码错误", nil)
	}

	u := M.UserInfo{}
	err := M.GetMobileInfo(Mobile, &u)
	if err == nil {
		return Response(c, InvalidParameter, "用户已注册", nil)
	}

	// 创建新用户
	u.ID = bson.NewObjectId()
	u.Mobile = Mobile
	u.Name = Name
	u.Face = Face
	u.Time = time.Now()
	u.Pass = Pass

	err = M.SetUserInfo(u.ID, &u)
	if err != nil {
		return Response(c, Internal, "创建用户失败", nil)
	}

	return Response(c, Succeed, "注册成功", u)
}

// 登陆
func Login(c echo.Context) error {

	// 参数验证
	Device := c.QueryParam("Device")
	Mobile := c.QueryParam("Mobile")
	Pass := c.QueryParam("Pass")

	if Mobile == "" || Pass == "" {
		return Response(c, InvalidParameter, "用户名密码错误", nil)
	}

	u := M.UserInfo{}
	err := M.GetMobileInfo(Mobile, &u)
	if err != nil {
		return Response(c, InvalidMobile, "用户名不存在", nil)
	}

	// 密码验证
	cookie := M.AuthUserPass(&u, Pass, Device)
	if cookie == nil {
		return Response(c, InvalidPass, "密码不正确", nil)
	}

	// 设置Cookie
	c.SetCookie(cookie)

	// 返回用户信息
	return Response(c, Succeed, "", u)
}
