package controllers

import (
	M "../modules"
	"github.com/labstack/echo"
	"gopkg.in/mgo.v2/bson"
)

// 获取好友列表
func GetUserLocation(c echo.Context) error {
	ID := c.QueryParam("ID")

	// 参数检查
	if ID == "" {
		return Response(c, ErrorParameter, "用户ID错误", nil)
	}

	u := M.LocationData{}
	M.GetUserLocation(bson.ObjectIdHex(ID), &u)
	return Response(c, Success, "", u)
}
