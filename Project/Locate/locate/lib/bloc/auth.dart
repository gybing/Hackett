import 'dart:async';

import 'package:meta/meta.dart';
import 'package:bloc/bloc.dart';

import 'package:locate/event/event.dart';
import 'package:locate/state/state.dart';
import 'package:locate/store/store.dart';

class AuthBloc extends Bloc<BaseEvent, AuthState> {
  final AuthStore authStore;

  AuthBloc({@required this.authStore})
      : assert(authStore != null);

  @override
  AuthState get initialState => AuthenticationUninitialized();

  @override
  Stream<AuthState> mapEventToState(BaseEvent event) async* {
    if (event is AppStartEvent) {
      final bool hasToken = false;// await authStore.hasToken();

      if (hasToken) {
        yield AuthenticationAuthenticated();
      } else {
        yield AuthenticationUnauthenticated();
      }
    }

    if (event is LoginEvent) {
      yield LoginLoading();

      try {
        final token = 'token'; /*await authStore.authenticate(
          username: event.username,
          password: event.password,
        );*/

          dispatch(LoginSuccessEvent(token: token));
        yield LoginInitial();
      } catch (error) {
        yield LoginFailure(error: error.toString());
      }
    }

    if (event is LoginSuccessEvent) {
      yield AuthenticationLoading();
      // await authStore.SetToken(event.token);
      yield AuthenticationAuthenticated();
    }

    if (event is LogoutEvent) {
      yield AuthenticationLoading();
      // await authStore.DeleteToken();
      yield AuthenticationUnauthenticated();
    }
  }
}
