// 用户信息
class UserInfo {
  final String ID;
  final String Mobile;
  final String Name;
  final String Face;
  final DateTime Time;

  UserInfo(this.ID, this.Mobile, this.Name, this.Face, this.Time);

  UserInfo.fromJson(Map<String, dynamic> json)
      : ID = json['ID'],
        Mobile = json['Mobile'],
        Name = json['Name'],
        Face = json['Face'],
        Time = DateTime.tryParse(json['Time']);

  Map<String, dynamic> toJson() => {
        'ID': ID,
        'Mobile': Mobile,
        'Name': Name,
        'Face': Face,
        'Time': Time.toString(),
      };
}

// 好友信息
class ContactInfo {
  final String ID;
  final String ContactID;
  final int State;
  final DateTime Time;

  ContactInfo(this.ID, this.ContactID, this.State, this.Time);

  ContactInfo.fromJson(Map<String, dynamic> json)
      : ID = json['ID'],
        ContactID = json['ContactID'],
        State = json['State'],
        Time = DateTime.tryParse(json['Time']);

  Map<String, dynamic> toJson() => {
        'ID': ID,
        'ContactID': ContactID,
        'State': State,
        'Time': Time.toString(),
      };
}