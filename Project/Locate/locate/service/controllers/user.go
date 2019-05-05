package controllers

import (
	M "../modules"
	"github.com/labstack/echo"
	"gopkg.in/mgo.v2/bson"
)

// 设置用户信息
func SetUserInfo(c echo.Context) error {
	Name := c.QueryParam("Name")
	Face := c.QueryParam("Face")
	Pass := c.QueryParam("Pass")

	u := M.UserInfo{}
	err := M.GetUserInfo(c.(*AuthContext).ID, &u)
	if err != nil {
		return Response(c, ErrorParameter, "用户不存在", nil)
	}

	if Face != "" {
		u.Face = Face
	}

	if Name != "" {
		u.Name = Name
	}

	if Pass != "" {
		u.Pass = M.GetPassHash(Pass)
	}

	// 设置用户信息
	err = M.SetUserInfo(u.ID, &u)
	if err != nil {
		return Response(c, ErrorServer, "创建用户失败", nil)
	}

	return Response(c, Success, "", u)
}

// 获取用户信息
func GetUserInfo(c echo.Context) error {
	ID := c.QueryParam("ID")
	u := M.UserInfo{}

	// 参数检查
	if ID == "" {
		M.GetUserInfo(c.(*AuthContext).ID, &u)
		return Response(c, Success, "", u)
	}

	M.GetUserInfo(bson.ObjectIdHex(ID), &u)
	return Response(c, Success, "", u)
}
