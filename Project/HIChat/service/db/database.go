package db

import (
	mgo "gopkg.in/mgo.v2"
)

var host = "mongodb://127.0.0.1:27017/hi"
var dbname = "hichat"

// 数据库
var session, err = mgo.Dial(host)

// 获取数据库链接
func GetCollection(table string) *mgo.Collection {
	s := session.Clone()
	defer s.Close()

	return s.DB(dbname).C(table)
}
