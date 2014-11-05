#include <iostream>

#include "coroutine_stuff.h"
#include "io_stuff.h"

void SetToNonBlocking(int socket) {
  int flags; 
  assert((flags = fcntl(socket, F_GETFL, 0)) >= 0);
  assert(fcntl(socket, F_SETFL, flags | O_NONBLOCK) >= 0) ;
}

template<typename T>
  void Go(T&& t) {
    TCoroutine coroutine(t);
    coroutine.Go();
  }

class TAsyncAcceptor {
  TAcceptor Acceptor;
public:
  TAsyncAcceptor(int port)
  : Acceptor(port) {
    SetToNonBlocking(Acceptor.GetSocket());
  }

  int GetSocket() const {
    return Acceptor.GetSocket();
  }

  TSocket Accept() {
    try {
      return Acceptor.Accept();
    } catch(const TWouldBlockException&) {
      Yield();
      return Acceptor.Accept();
    }
  }
};

int main() {
  TEpoll epoll;
  TCoroutine coroutine([&epoll](TCoroutine* me) {
    TAsyncAcceptor acceptor(1234);
    TCallback callback([me]() {
      me->Go();
    });
    epoll.Add(acceptor.GetSocket(), &callback);
    for (;;) {
        TSocket socket = acceptor.Accept();
        socket.Write("Hello, world!");
    }
  });
  coroutine.Go();
  for (;;) {
    epoll.Wait();
  }
}
