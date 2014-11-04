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

class TSocket {
    int Socket;
  public:
    explicit TSocket(int d);
    size_t Write(const std::string& string);
    ~TSocket();
};

class TEpoll {
  int Handle;
  const static size_t MAX_EVENT = 10;
  epoll_event Events[MAX_EVENT];
public:
  TEpoll();
  void Wait();
  ~TEpoll();
};

class TAcceptor {
    int Socket;
    const static size_t QUEUE_SIZE = 10;
  public:
    explicit TAcceptor(int port);
    TSocket Accept();
    ~TAcceptor();
};

class TScheduler {
    TEpoll Epoll;
  public:
    void Schedule();
};
