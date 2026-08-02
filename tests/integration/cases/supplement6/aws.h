#pragma once
#include <cassert>
#include <memory>
#include <mutex>
#include <string>

namespace myblob {

namespace network {
class ConnectionManager;
class HttpClient;
class TaskedSendReceiver;
class TaskedSendReceiverHandle;
}

namespace cloud {
  class AWS{
  public:
  protected:
    thread_local static AWS *_validInstance;
  };
}
}
