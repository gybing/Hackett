import 'package:bloc/bloc.dart';

enum MainFrameEvent { login, logout }

class MainFrameBloc extends Bloc<MainFrameEvent, int> {
  @override
  int get initialState => 1;

  @override
  Stream<int> mapEventToState(int currentState, MainFrameEvent event) async* {
    switch(event)
    {
      case MainFrameEvent.login:
      {
        yield currentState + 1;
        break;
      }
      case MainFrameEvent.logout:
      {
        yield currentState -1;
        break;
      }
    }

    yield 0;
  }
}