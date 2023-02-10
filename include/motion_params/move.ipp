#include "move.h"
#include <chrono>
#include <functional>
#include <memory>
#include <optional>

template <typename Derived, typename T>
Derived&& AbstractMove<Derived, T>::me() {
  return std::move(*static_cast<Derived*>(this));
}

template <typename Derived, typename T, typename F>
MoveBase<Derived, T, F>::MoveBase(const T& start_value, F&& shift,
                                  std::shared_ptr<AbstractTimer> timer)
    : start_value_(start_value), shift_(std::move(shift)),
      timer_(std::move(timer)) {
  if constexpr (LEq<T>) {
    reached_end_ = [](const T& a, const T& b) { return b <= a; };
  }
}

template <typename Derived, typename T, typename F>
void MoveBase<Derived, T, F>::set_end_value(const T& end) & {
  end_value_ = end;
}

template <typename Derived, typename T, typename F>
Derived&& MoveBase<Derived, T, F>::end_value(const T& end) && {
  set_end_value(end);
  return std::move(Base::me());
}

template <typename Derived, typename T, typename F>
void MoveBase<Derived, T, F>::set_reached_end(
    std::function<bool(const T&, const T&)>&& f) & {
  reached_end_ = std::move(f);
}

template <typename Derived, typename T, typename F>
Derived&& MoveBase<Derived, T, F>::reached_end(
    std::function<bool(const T&, const T&)>&& f) && {
  set_reached_end(std::move(f));
  return std::move(Base::me());
}

template <typename Derived, typename T, typename F>
T MoveBase<Derived, T, F>::get() {
  auto cur_value = shift_(start_value_, timer_->elapsed());
  if (end_value_.has_value()) {
    if ((*reached_end_)(cur_value, *end_value_)) {
      cur_value = *end_value_;
    }
  }
  return cur_value;
}

template <typename T, typename F>
Move<T, F>::Move(const T& start_value, F&& shift,
                 std::shared_ptr<AbstractTimer> timer)
    : Base(start_value, std::move(shift), std::move(timer)) {}
