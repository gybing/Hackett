package base

import (
	"log"

	"github.com/labstack/echo"
	"github.com/labstack/echo/middleware"
)

type Service struct {
	echo *echo.Echo
}

var service *Service

func GetService() *Service {
	if service == nil {
		createService()
	}
	return service
}

func createService() *Service {
	service = &Service{}

	// 初始化Echo
	service.echo = echo.New()
	service.echo.Use(middleware.Recover())
	service.echo.HTTPErrorHandler = HTTPErrorHandlerDefault
	log.SetOutput(service.echo.Logger.Output())

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
