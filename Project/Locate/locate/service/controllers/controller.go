package controllers

import (
	"net/http"

	M "../modules"
	"github.com/labstack/echo"
	"gopkg.in/mgo.v2/bson"
)

const (
	Success        int = iota // 成功
	ErrorParameter            // 输入参数错误
	ErrorServer               // 服务内部错误
	ErrorToken                // 令牌无效
	ErrorPower                // 无权限
	ErrorExist                // 已存在
)

// HTTP响应数据结构
type ResponseData struct {
	Code    int         `bson:"Code"`
	Message string      `bson:"Message"`
	Data    interface{} `bson:"Data"`
}

func (he *ResponseData) Error() string {
	return he.Message
}

// HTTP响应方法
func Response(c echo.Context, code int, message string, data interface{}) error {
	return c.JSON(http.StatusOK, ResponseData{code, message, data})
}

// 默认HTTP错误处理
func HTTPErrorHandlerDefault(err error, c echo.Context) {

	if he, ok := err.(*ResponseData); ok {
		c.JSON(http.StatusOK, ResponseData{he.Code, he.Error(), he.Data})
		return
	}

	c.JSON(http.StatusOK, ResponseData{ErrorServer, err.Error(), nil})
	return
}

// Context
type AuthContext struct {
	echo.Context

	ID bson.ObjectId
}

// 令牌验证中间件
func AuthorizationMiddleware(h echo.HandlerFunc) echo.HandlerFunc {
	return func(c echo.Context) error {

		cookie, err := c.Cookie("token")
		if err != nil || cookie.Value == "" {
			return &ResponseData{Code: ErrorToken, Message: "无效的Cookie"}
		}

		token, err := M.CheckToken(cookie.Value)
		if err != nil || token == nil || token.ID == "" {
			return &ResponseData{Code: ErrorToken, Message: "无效的token"}
		}

		return h(&AuthContext{c, token.ID})
	}
}
