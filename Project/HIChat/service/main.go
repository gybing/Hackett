package main

import (
	"log"

	C "./controllers"
	"github.com/labstack/echo"
	"github.com/labstack/echo/middleware"
)

// 主程序入口
func main() {
	// 初始化
	e := echo.New()
	e.Use(middleware.Recover())
	e.HTTPErrorHandler = HTTPErrorHandlerDefault

	log.SetOutput(e.logger.OutPut())
	//------------
	// 路由规则

	// Auth
	authGroup := e.Group("/auth")
	authGroup.GET("/regist", C.Regist)
	authGroup.GET("/login", C.Login)

	// User
	userGroup := e.Group("/user", C.AuthTokenMiddleware)
	userGroup.GET("/getuserinfo", C.GetUserInfo)
	userGroup.GET("/setuserinfo", C.SetUserInfo)

	// User
	contactGroup := e.Group("/contact", C.AuthTokenMiddleware)
	contactGroup.GET("/getcontactlist", C.GetContactList)

	//------------
	// 服务启动
	e.Logger.Fatal(e.Start(":80"))
}
