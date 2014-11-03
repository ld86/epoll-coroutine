#include <unistd.h>
#include <assert.h>

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class TEpoll {
  int Handle;
public:
  TEpoll()
  : Handle(epoll_create(1))
  {
    assert(Handle != -1);
  }

  ~TEpoll() {
    close(Handle);
  }
};

class TAcceptor {
};

class TScheduler {
    TEpoll epoll;
  public:
    void Schedule() {
    }
};
