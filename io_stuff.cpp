#include "io_stuff.h"

#include <string.h>

TEpoll::TEpoll()
: Handle(epoll_create(1))
{
  assert(Handle != -1);
}

void TEpoll::Wait() {
  size_t readyCount = epoll_wait(Handle, Events, MAX_EVENT, -1);
}

TEpoll::~TEpoll() {
  close(Handle);
}

TAcceptor::TAcceptor(int port) {
  Socket = socket(AF_INET, SOCK_STREAM, 0);
  assert(Socket != -1);

  {
    int option = 1;
    setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
  }

  {
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    assert(bind(Socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != -1);
  }

  assert(listen(Socket, QUEUE_SIZE) != -1);
}

TSocket TAcceptor::Accept() {
  return TSocket(accept(Socket, nullptr, 0));
}

TAcceptor::~TAcceptor() {
  close(Socket);
}

TSocket::TSocket(int d) : Socket(d) {
  assert(Socket != -1);
}

size_t TSocket::Write(const std::string& string) {
  return write(Socket, string.data(), string.length());
}

TSocket::~TSocket() {
  close(Socket);
}

void TScheduler::Schedule() {
  Epoll.Wait();
}
