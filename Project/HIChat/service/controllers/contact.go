package controllers

import (
	M "../modules"
	"github.com/labstack/echo"
)

// 获取好友列表
func GetContactList(c echo.Context) error {
	us := []M.UserInfo{}
	M.GetContactList(c.(*AuthContext).ID, &us)
	return Response(c, Succeed, "", us)
}
