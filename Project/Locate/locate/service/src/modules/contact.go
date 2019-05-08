package modules

import (
	"errors"
	"time"

	"../db"
	"gopkg.in/mgo.v2/bson"
)

const (
	ContactTypeSend    = 1 // 发送
	ContactTypeReceive = 2 // 接收
)

const (
	ContactAgree  = 1 // 同意申请
	ContactDelete = 2 // 删除申请/好友
)

const (
	ContactStateRequest = 1 // 申请
	ContactStateClose   = 2 // 好友
)

// 关系信息
type ContactData struct {
	ID        bson.ObjectId `bson:"_id" json:"_id"`
	ContactID bson.ObjectId `bson:"ContactID"`
	State     int           `bson:"State"`
	Time      time.Time     `bson:"Time"`
}

// 申请加好友
func Request(id bson.ObjectId, cid bson.ObjectId) error {
	if id == cid {
		return errors.New("cannot request own")
	}

	ci := ContactData{
		ID:        id,
		ContactID: cid,
		State:     ContactStateRequest,
		Time:      time.Now(),
	}

	_, err := db.Contact.Upsert(bson.M{"_id": ci.ID}, ci)
	return err
}

// 处理申请
func Process(id bson.ObjectId, cid bson.ObjectId) error {
	ci := ContactData{}
	err := db.Contact.Find(bson.M{"_id": cid, "ContactID": id}).One(&ci)
	if err != nil {
		return err
	}

	if ci.State != ContactStateRequest {
		return errors.New("error contact state")
	}

	ci.State = ContactStateClose
	ci.Time = time.Now()
	_, err = db.Contact.Upsert(bson.M{"_id": cid, "ContactID": id}, ci)
	return err
}

// 处理申请
func Remove(id bson.ObjectId, cid bson.ObjectId) error {
	err := db.Contact.Remove(bson.M{"$or": []bson.M{
		bson.M{"_id": id, "ContactID": cid},
		bson.M{"_id": cid, "ContactID": id}}})
	return err
}

// 获取好友列表
func GetContactList(id bson.ObjectId, cs *[]ContactData) {
	db.Contact.Find(
		bson.M{"$or": []bson.M{
			bson.M{"_id": id, "State": ContactStateClose},
			bson.M{"ContactID": id, "State": ContactStateClose}}}).All(cs)

	for _, each := range *cs {
		if each.ID != id {
			each.ID, each.ContactID = each.ContactID, each.ID
		}
	}
}

// 获取申请列表
func GetRequestList(id bson.ObjectId, rtype int, cs *[]ContactData) {
	if rtype == 1 {
		db.Contact.Find(bson.M{"_id": id, "State": ContactStateRequest}).All(cs)
	} else {
		db.Contact.Find(bson.M{"ContactID": id, "State": ContactStateRequest}).All(cs)
		for _, each := range *cs {
			each.ID, each.ContactID = each.ContactID, each.ID
		}
	}
}
