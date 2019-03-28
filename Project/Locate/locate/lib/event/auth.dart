import 'package:meta/meta.dart';
import 'package:equatable/equatable.dart';

abstract class BaseEvent extends Equatable {
  BaseEvent([List props = const []]) : super(props);
}

class AppStartEvent extends BaseEvent {
  @override
  String toString() => 'AppStartEvent';
}

class LoginEvent extends BaseEvent {
  final String username;
  final String password;

  LoginEvent({
    @required this.username,
    @required this.password,
  }) : super([username, password]);

  @override
  String toString() =>
      'LoginEvent { username: $username, password: $password }';
}

class LoginSuccessEvent extends BaseEvent {
  final String token;

  LoginSuccessEvent({@required this.token}) : super([token]);

  @override
  String toString() => 'LoginSuccessEvent { token: $token }';
}

class LogoutEvent extends BaseEvent {
  @override
  String toString() => 'LogoutEvent';
}
