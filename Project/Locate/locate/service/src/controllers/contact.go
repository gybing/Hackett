package controllers

import (
	"strconv"

	M "../modules"
	"github.com/labstack/echo"
	"gopkg.in/mgo.v2/bson"
)

// 申请好友
func Request(c echo.Context) error {
	ContactID := c.QueryParam("ContactID")
	if ContactID == "" {
		return Response(c, ErrorParameter, "参数错误", nil)
	}

	M.Request(c.(*AuthContext).ID, bson.ObjectIdHex(ContactID))
	return Response(c, Success, "", nil)
}

// 处理申请
func Process(c echo.Context) error {
	ContactID := c.QueryParam("ContactID")
	Type := c.QueryParam("Type")

	if (ContactID == "") || (Type == "") {
		return Response(c, ErrorParameter, "参数错误", nil)
	}

	if Type == "1" {
		M.Process(c.(*AuthContext).ID, bson.ObjectIdHex(ContactID))
	} else if Type == "2" {
		M.Remove(c.(*AuthContext).ID, bson.ObjectIdHex(ContactID))
	} else {
		return Response(c, ErrorParameter, "参数错误", nil)
	}

	return Response(c, Success, "", nil)
}

// 处理申请
func Remove(c echo.Context) error {
	ContactID := c.QueryParam("ContactID")

	if ContactID == "" {
		return Response(c, ErrorParameter, "参数错误", nil)
	}

	M.Remove(c.(*AuthContext).ID, bson.ObjectIdHex(ContactID))
	return Response(c, Success, "", nil)
}

// 获取好友列表
func GetContactList(c echo.Context) error {
	cs := []M.ContactData{}
	M.GetContactList(c.(*AuthContext).ID, &cs)
	return Response(c, Success, "", cs)
}

// 获取申请列表
func GetRequestList(c echo.Context) error {
	Type := c.QueryParam("Type")
	if Type == "" {
		return Response(c, ErrorParameter, "参数错误", nil)
	}

	tp, err := strconv.Atoi(Type)
	if err != nil {
		return Response(c, ErrorParameter, "参数错误", nil)
	}

	cs := []M.ContactData{}
	M.GetRequestList(c.(*AuthContext).ID, tp, &cs)
	return Response(c, Success, "", cs)
}
