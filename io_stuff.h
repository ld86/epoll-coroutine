#pragma once

#include <unistd.h>
#include <assert.h>

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h> 

#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>

#include <string>
#include <stdexcept>

class TWouldBlockException : std::exception {
};

class TSocket {
    int Socket;
  public:
    explicit TSocket(int d);
    size_t Write(const std::string& string);
    int GetSocket() const;
    ~TSocket();
};

class TAcceptor;
class TEpoll {
  int Handle;
  const static size_t MAX_EVENT = 10;
  epoll_event Events[MAX_EVENT];
public:
  TEpoll();
  void Wait();
  void Add(const TAcceptor& acceptor);
  ~TEpoll();
};

class TAcceptor {
    int Socket;
    const static size_t QUEUE_SIZE = 10;
  public:
    explicit TAcceptor(int port);
    TSocket Accept();
    int GetSocket() const;
    ~TAcceptor();
};

class TScheduler {
    TEpoll Epoll;
  public:
    void Schedule();
};
