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
#include <string.h>
#include <stdexcept>
#include <iostream>
#include <functional>

class TWouldBlockException : std::exception {
};

class TCallback {
  typedef std::function<void()> THandler;
  THandler Handler;
public:
  TCallback(THandler handler)
  : Handler(handler) {}
  
  void operator()() {
    Handler();
  }
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

  void Add(int Socket, TCallback* callback);
  void Remove(int Socket);
  void Wait();

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
