package base

import (
	"github.com/labstack/echo"
)

type Controller struct {
	name  string
	route echo.Group
}

func (m *Controller) GetRoute() map[string]interface{} {
	return nil
}
