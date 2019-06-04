// 用户信息
class UserInfo {
  final String id;
  final String mobile;
  final String name;
  final String face;
  final DateTime time;

  UserInfo(this.id, this.mobile, this.name, this.face, this.time);

  UserInfo.fromJson(Map<String, dynamic> json)
      : id = json['ID'],
        mobile = json['Mobile'],
        name = json['Name'],
        face = json['Face'],
        time = DateTime.tryParse(json['Time']);

  Map<String, dynamic> toJson() => {
        'ID': id,
        'Mobile': mobile,
        'Name': name,
        'Face': face,
        'Time': time.toString(),
      };
}

// 好友信息
class ContactInfo {
  final String id;
  final String contact;
  final int state;
  final DateTime time;
  UserInfo userinfo;

  ContactInfo(this.id, this.contact, this.state, this.time);

  ContactInfo.fromJson(Map<String, dynamic> json)
      : id = json['ID'],
        contact = json['ContactID'],
        state = json['State'],
        time = DateTime.tryParse(json['Time']);

  Map<String, dynamic> toJson() => {
        'ID': id,
        'ContactID': contact,
        'State': state,
        'Time': time.toString(),
      };
}