#include <iostream>

#include "coroutine_stuff.h"
#include "io_stuff.h"

void SetSocketToNonBlocking(int socket) {
  int flags; 
  assert((flags = fcntl(socket, F_GETFL, 0)) >= 0);
  assert(fcntl(socket, F_SETFL, flags | O_NONBLOCK) >= 0) ;
}

class TAsyncAcceptor {
  TAcceptor SyncAcceptor;
public:
  TAsyncAcceptor(int port)
  : SyncAcceptor(port) {
    SetSocketToNonBlocking(SyncAcceptor.GetSocket());
  }

  TSocket Accept() {
    for (;;) {
      try {
        return SyncAcceptor.Accept();
      } catch (const TWouldBlockException&) {
        Yield();
      }
    }
  }
};

class THandler {
  int Port;
public:
  THandler(int port) 
    : Port(port) {
    }

  void operator()() {
    TAsyncAcceptor acceptor(Port);
    TSocket socket = acceptor.Accept();
  }
};

template<typename T>
  void Go(T& t) {
    t.Go();
  }

int main() {
  THandler handler(12345);
  TCoroutine coroutine(handler);
  Go(coroutine);
}
