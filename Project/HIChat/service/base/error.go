package base

import (
	"net/http"

	"github.com/labstack/echo"
	"gopkg.in/mgo.v2/bson"
)

const (
	Succeed           int = iota // 成功
	ErrorData                    // 无数据，不存在
	ErrorParameter               // 输入参数错误
	ErrorServer                  // 服务内部错误
	ErrorNotSupport              // 不支持的请求(一般用在LXObjectBase中的Invoke方法)
	ErrorNoPower                 // 无权限(账号无修改他人数据权限，账号权限不能做某项业务等等...)
	ErrorToLong                  // 输入的字符串过长
	ErrorNotAllow                // 隐私设置限制
	ErrorReachLimit              // 到达上限
	ErrorAlreadyExist            // 已存在
	ErrorNoUpdate                // 版本未改变，不需要更新
	ErrorToken                   // 令牌无效
	ErrorMobile                  // 账号不存在
	ErrorPass                    // 密码错误
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
	}

	if he, ok := err.(*echo.HTTPError); ok {
		c.JSON(he.Code, ResponseData{he.Code, he.Error(), he.Message})
	}

	c.JSON(http.StatusOK, ResponseData{ServerError, err.Error(), nil})
}

// Context
type AuthContext struct {
	echo.Context

	ID bson.ObjectId
}

// 令牌验证中间件
func AuthTokenMiddleware(h echo.HandlerFunc) echo.HandlerFunc {
	return func(c echo.Context) error {

		cookie, err := c.Cookie("token")
		if err != nil || cookie.Value == "" {
			return &ResponseData{Code: InvalidToken, Message: "无效的Cookie"}
		}

		token, err := M.CheckToken(cookie.Value)
		if err != nil || token == nil || token.ID == "" {
			return &ResponseData{Code: InvalidToken, Message: "无效的token"}
		}

		return h(&AuthContext{c, token.ID})
	}
}
