package modules

import (
	"time"

	"../db"
	"gopkg.in/mgo.v2/bson"
)

// 位置信息
type LocationData struct {
	ID        bson.ObjectId `bson:"_id" json:"ID"`
	Accuracy  float64       `bson:"Accuracy"`
	Longitude float64       `bson:"Longitude"`
	Latitude  float64       `bson:"Latitude"`
	Altitude  float64       `bson:"Altitude"`
	Time      time.Time     `bson:"Time"`
}

// 获取好友列表
func GetUserLocation(id bson.ObjectId, l interface{}) {
	cs := []LocationData{}
	db.Contact.Find(bson.M{"UserID": id}).All(&cs)

	uids := make([]bson.ObjectId, len(cs))
	for i, each := range cs {
		uids[i] = each.ID
	}
	// GetUserInfoList(uids, us)
}
