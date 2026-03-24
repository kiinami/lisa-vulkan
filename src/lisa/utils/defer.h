//
// Created by kinami on 3/24/26.
//

#ifndef LISA_DEFER_H
#define LISA_DEFER_H

#pragma once
#include <utility>

template <typename F>
class DeferGuard {
public:
    explicit DeferGuard(F&& f) : function(std::forward<F>(f)) {}
    ~DeferGuard() { function(); }

    DeferGuard(const DeferGuard&) = delete;
    DeferGuard& operator=(const DeferGuard&) = delete;
private:
    F function;
};

#define DEFER_CONCAT_IMPL(x, y) x##y
#define DEFER_CONCAT(x, y) DEFER_CONCAT_IMPL(x, y)
#define defer(code) \
    auto DEFER_CONCAT(_defer_guard_, __LINE__) = DeferGuard([&]() { code; })

#endif //LISA_DEFER_H