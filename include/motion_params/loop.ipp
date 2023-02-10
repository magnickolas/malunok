#include "loop.h"
#include "move.h"
#include <chrono>
#include <functional>
#include <memory>
#include <optional>

template <typename Derived, typename T, typename F>
LoopBase<Derived, T, F>::LoopBase(const T& start_value, F&& shift,
                                  std::shared_ptr<AbstractTimer> timer)
    : Base(start_value, std::move(shift), std::move(timer)) {
  if constexpr (LEq<T>) {
    reached_begin_ = [](const T& a, const T& b) { return a <= b; };
  }
}

template <typename Derived, typename T, typename F>
void LoopBase<Derived, T, F>::set_reached_begin(
    std::function<bool(const T&, const T&)>&& f) & {
  reached_begin_ = std::move(f);
}

template <typename Derived, typename T, typename F>
Derived&& LoopBase<Derived, T, F>::reached_begin(
    std::function<bool(const T&, const T&)>&& f) && {
  set_reached_begin(std::move(f));
  return std::move(Base::me());
}

template <typename Derived, typename T, typename F>
T LoopBase<Derived, T, F>::get() {
  T cur_value = direction_
                    ? Base::shift_(Base::start_value_, Base::timer_->elapsed())
                    : Base::shift_(*Base::end_value_, -Base::timer_->elapsed());
  if (direction_ && (*Base::reached_end_)(cur_value, *Base::end_value_)) {
    direction_ = !direction_;
    Base::timer_->reset();
    cur_value = *Base::end_value_;
  } else if (!direction_ && (*reached_begin_)(cur_value, Base::start_value_)) {
    direction_ = !direction_;
    Base::timer_->reset();
    cur_value = Base::start_value_;
  }
  return cur_value;
}

template <typename T, typename F>
Loop<T, F>::Loop(const T& start_value, F&& shift,
                 std::shared_ptr<AbstractTimer> timer)
    : Base(start_value, std::move(shift), std::move(timer)) {}
