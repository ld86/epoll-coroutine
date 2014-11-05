#include "io_stuff.h"

#include <string.h>
#include <iostream>

TEpoll::TEpoll()
: Handle(epoll_create(1))
{
  assert(Handle != -1);
}

void TEpoll::Wait() {
  size_t readyCount = epoll_wait(Handle, Events, MAX_EVENT, -1);
}

void TEpoll::Add(const TAcceptor& acceptor) {
  epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.events = EPOLLIN;
  ev.data.fd = acceptor.GetSocket();
  assert(epoll_ctl(Handle, EPOLL_CTL_ADD, ev.data.fd, &ev) != -1);
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
  std::cout << Socket << std::endl;
}

size_t TSocket::Write(const std::string& string) {
  return write(Socket, string.data(), string.length());
}

TSocket::~TSocket() {
  close(Socket);
}
