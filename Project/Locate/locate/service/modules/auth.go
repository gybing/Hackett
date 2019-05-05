package modules

import (
	"crypto/md5"
	"encoding/hex"
	"math/rand"
	"net/http"
	"time"

	"../db"
	"gopkg.in/mgo.v2/bson"
)

// 认证信息
type AuthInfo struct {
	ID     bson.ObjectId `bson:"_id"`
	Token  string        `bson:"Token"`
	Device string        `bson:"Device"`
	Time   time.Time     `bson:"Time"`
}

func SetAuthInfo(t *AuthInfo) error {
	_, err := db.Auth.Upsert(bson.M{"_id": t.ID}, t)
	return err
}

// 密码认证
func AuthUserPass(u *UserInfo, pass string, device string) *http.Cookie {
	if GetPassHash(pass) != u.Pass {
		return nil
	}

	// 生成令牌
	token := AuthInfo{
		ID:     u.ID,
		Token:  createToken(),
		Device: device,
		Time:   time.Now(),
	}

	// 写入数据库
	err := SetAuthInfo(&token)
	if err != nil {
		return nil
	}

	return createCookie(token.Token)
}

// 检查令牌
func CheckToken(t string) (*AuthInfo, error) {
	token := AuthInfo{}
	err := db.Auth.Find(bson.M{"Token": t}).One(&token)
	return &token, err
}

// 生成Cookie
func createCookie(token string) *http.Cookie {
	cookie := new(http.Cookie)
	cookie.Name = "token"
	cookie.Value = token
	cookie.Path = "/"
	cookie.Expires = time.Now().Add(10 * 360 * 24 * time.Hour)
	return cookie
}

// 生成令牌
func createToken() string {
	l := 20
	bytes := []byte("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
	result := []byte{}
	r := rand.New(rand.NewSource(time.Now().UnixNano()))
	for i := 0; i < l; i++ {
		result = append(result, bytes[r.Intn(len(bytes))])
	}
	return string(result)
}

// 生成密码
func GetPassHash(pass string) string {
	h := md5.New()
	h.Write([]byte(pass))
	return hex.EncodeToString(h.Sum(nil))
}
