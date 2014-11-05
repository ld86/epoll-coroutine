#include "io_stuff.h"

#include <iostream>

TEpoll::TEpoll()
: Handle(epoll_create(1))
{
  assert(Handle != -1);
}

void TEpoll::Add(int Socket, TCallback* callback) {
  assert(callback);
  epoll_event ev = {0, {0}};
  ev.events = EPOLLIN | EPOLLET;
  ev.data.ptr = callback;
  assert(epoll_ctl(Handle, EPOLL_CTL_ADD, Socket, &ev) != -1);
}

void TEpoll::Remove(int Socket) {
  epoll_event ev = {0, {0}};
  assert(epoll_ctl(Handle, EPOLL_CTL_DEL, Socket, &ev) != -1);
}

void TEpoll::Wait() {
  size_t readyCount = epoll_wait(Handle, Events, MAX_EVENT, -1);
  for (size_t i = 0; i < readyCount; ++i) {
    TCallback* callback = static_cast<TCallback*>(Events[i].data.ptr);
    if (callback) {
      (*callback)();
    }
  }
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
  int socket = accept(Socket, nullptr, 0);
  if (socket == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
    throw TWouldBlockException();
  }
  return TSocket(socket);
}

int TAcceptor::GetSocket() const {
  return Socket;
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
}
