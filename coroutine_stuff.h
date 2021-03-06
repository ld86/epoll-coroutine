#pragma once

#include <iostream>
#include <array>
#include <functional>
#include <vector>

#include <boost/context/all.hpp>

namespace NContext = boost::context;

class TCoroutine;
__thread TCoroutine* ActiveCoroutine = nullptr;
class TCoroutine {
  friend void Yield();
  typedef std::function<void(TCoroutine*)> TFunction;

  TFunction Function;
  bool Started;
  bool Running;
  std::array<uint8_t, 64 * 1024> Stack;
  NContext::fcontext_t* FunctionContext;
  NContext::fcontext_t MainContext;

  void Yield() {
    NContext::jump_fcontext(FunctionContext, &MainContext, 0);
  }
  
  static void Wrapper(intptr_t pointer) {
    TCoroutine* coroutine = reinterpret_cast<TCoroutine*>(pointer);
    coroutine->Started = true;
    coroutine->Function(coroutine);
    coroutine->Started = false;
    coroutine->Yield();
  }

public:
  explicit TCoroutine(TFunction function)
    : Function(function)
    , Started(false)
    , Running(false)
    , FunctionContext(NContext::make_fcontext(Stack.data() + Stack.size(), Stack.size(), Wrapper))
  {}

  void Go() {
    TCoroutine* previous = this;
    std::swap(ActiveCoroutine, previous);
    Running = true;
    NContext::jump_fcontext(&MainContext, FunctionContext, reinterpret_cast<intptr_t>(this));
    Running = false;
    std::swap(ActiveCoroutine, previous);
  }

  bool IsStarted() const {
    return Started;
  }

  bool IsRunning() const {
    return Running;
  }

};

void Yield() {
  ActiveCoroutine->Yield();
}
