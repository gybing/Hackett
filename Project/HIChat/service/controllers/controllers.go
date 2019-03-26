package controllers

import "github.com/labstack/echo"

func CreateRoute(e *echo.Echo) {
	userGroup := e.Group("/user", C.AuthTokenMiddleware)
	userGroup.GET("/getuserinfo", C.GetUserInfo)
	userGroup.GET("/setuserinfo", C.SetUserInfo)

	service := GetService()

	service.echo.
		RouteUser
	route["/user"] = GetUserInfo
}
