package db

import (
	mgo "gopkg.in/mgo.v2"
)

var host = "mongodb://hackett:roothan@144.34.183.114"
var dbname = "locate"

// 数据库
var session, err = mgo.Dial(host)

// 获取数据库链接
func GetCollection(table string) *mgo.Collection {
	s := session.Clone()
	// defer s.Close()

	return s.DB(dbname).C(table)
}

var User = GetCollection("User")
var Auth = GetCollection("Auth")
var Contact = GetCollection("Contact")
