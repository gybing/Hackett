package main

import (
	C "./controllers"
	"github.com/labstack/echo"
	"github.com/labstack/echo/middleware"
)

// 主程序入口
func main() {
	// 初始化
	e := echo.New()
	e.Use(middleware.Recover())
	e.HTTPErrorHandler = C.HTTPErrorHandlerDefault
	// log.SetOutput(e.Logger.Output())

	//------------
	// 路由规则

	// Auth
	Auth := e.Group("/Auth")
	Auth.GET("/Regist", C.Regist)
	Auth.GET("/Login", C.Login)
	Auth.GET("/Logout", C.Logout)

	// User
	User := e.Group("/User", C.AuthorizationMiddleware)
	User.GET("/GetUserInfo", C.GetUserInfo)
	User.GET("/SetUserInfo", C.SetUserInfo)

	// Contact
	Contact := e.Group("/Contact", C.AuthorizationMiddleware)
	Contact.GET("/Request", C.Request)
	Contact.GET("/Process", C.Process)
	Contact.GET("/Remove", C.Remove)
	Contact.GET("/GetRequestList", C.GetRequestList)
	Contact.GET("/GetContactList", C.GetContactList)

	//------------
	// 服务启动
	e.Logger.Fatal(e.Start(":80"))
}
