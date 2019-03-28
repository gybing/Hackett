import 'package:meta/meta.dart';
import 'package:equatable/equatable.dart';

abstract class AuthState extends Equatable {
  AuthState([List props = const []]) : super(props);
}

class AuthenticationUninitialized extends AuthState {
  @override
  String toString() => 'AuthenticationUninitialized';
}

class AuthenticationAuthenticated extends AuthState {
  @override
  String toString() => 'AuthenticationAuthenticated';
}

class AuthenticationUnauthenticated extends AuthState {
  @override
  String toString() => 'AuthenticationUnauthenticated';
}

class AuthenticationLoading extends AuthState {
  @override
  String toString() => 'AuthenticationLoading';
}

class LoginInitial extends AuthState {
  @override
  String toString() => 'LoginInitial';
}

class LoginLoading extends AuthState {
  @override
  String toString() => 'LoginLoading';
}

class LoginFailure extends AuthState {
  final String error;

  LoginFailure({@required this.error}) : super([error]);

  @override
  String toString() => 'LoginFailure { error: $error }';
}
