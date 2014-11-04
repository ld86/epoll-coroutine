#include "coroutine_stuff.h"
#include "io_stuff.h"

int main() {
  TAcceptor acceptor(1234);
  TSocket client = acceptor.Accept();
  client.Write("Hello, world!\n");
}
