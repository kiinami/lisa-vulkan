//
// Created by kinami on 4/25/26.
//

#include "context.h"

#include "SDL3pp/SDL3pp_timer.h"
#include "utils/common.h"

namespace lisa::update::context {
  namespace {
    vector<std::function<void(float)>> callbacks_;
  }

  void init() {}

  void destroy() { callbacks_.clear(); }

  void register_update(const std::function<void(float)>& fn) {
    callbacks_.push_back(fn);
  }

  void tick() {
    static auto last = SDL::GetTicks();
    const auto now = SDL::GetTicks();
    const auto dt = std::chrono::duration<float>(now - last).count();
    last = now;
    for (auto fn : callbacks_)
      fn(dt);
  }
}
