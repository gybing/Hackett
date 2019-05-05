package modules

import (
	"time"

	"../db"
	"gopkg.in/mgo.v2/bson"
)

// 用户信息
type UserInfo struct {
	// 基本信息
	ID     bson.ObjectId `bson:"_id" json:"ID"`
	Mobile string        `bson:"Mobile"`
	Name   string        `bson:"Name"`
	Face   string        `bson:"Face"`
	Time   time.Time     `bson:"Time"`

	// 认证
	Pass string `bson:"Pass" json:"-"`
}

// 设置用户信息
func SetUserInfo(id bson.ObjectId, i interface{}) error {
	_, err := db.User.Upsert(bson.M{"_id": id}, i)
	return err
}

// 获取用户ID
func GetMobileInfo(m string, i interface{}) error {
	return db.User.Find(bson.M{"Mobile": m}).One(i)
}

// 获取用户信息
func GetUserInfo(id bson.ObjectId, i interface{}) error {
	return db.User.Find(bson.M{"_id": id}).One(i)
}

// 批量获取用户信息
func GetUserInfoList(ids []bson.ObjectId, i interface{}) error {
	return db.User.Find(bson.M{"_id": bson.M{"$in": ids}}).All(i)
}
