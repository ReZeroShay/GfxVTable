#pragma once
#include <utility>
#include <functional>
#include <concepts>

namespace GfxVTable {
    template <typename F>
        requires std::invocable<F>
    class ScopeGuard {
      public:
        explicit ScopeGuard(F fn) : Fn(std::move(fn)) {}
        ScopeGuard(const ScopeGuard &) = delete;
        ScopeGuard &operator=(const ScopeGuard &) = delete;
        ScopeGuard(ScopeGuard &&) = delete;
        ScopeGuard &operator=(ScopeGuard &&) = delete;
        ~ScopeGuard() { std::invoke(Fn); }

      private:
        F Fn;
    };
    template <typename F> ScopeGuard(F) -> ScopeGuard<std::decay_t<F>>;

} // namespace GfxVTable
