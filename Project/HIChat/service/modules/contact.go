package modules

import (
	"time"

	"../db"
	"gopkg.in/mgo.v2/bson"
)

// 关系信息
type ContactInfo struct {
	ID        bson.ObjectId `bson:"_id" json:"ID"`
	UserID    bson.ObjectId `bson:"UserID"`
	ContactID bson.ObjectId `bson:"ContactID"`
	Time      time.Time     `bson:"Time"`
}

// 获取好友列表
func GetContactList(id bson.ObjectId, us interface{}) {
	cs := []ContactInfo{}
	db.Contacts.Find(bson.M{"UserID": id}).All(&cs)

	uids := make([]bson.ObjectId, len(cs))
	for i, each := range cs {
		uids[i] = each.ContactID
	}
	GetUserInfoList(uids, us)
}
