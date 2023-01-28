#include "move.h"
#include <chrono>
#include <functional>
#include <optional>

template <typename Derived, typename T>
Derived&& AbstractMove<Derived, T>::me() {
  return std::move(*static_cast<Derived*>(this));
}

template <typename Derived, typename T>
MoveBase<Derived, T, void>::MoveBase(const T& start_value)
    : start_value_(start_value), cur_value_(start_value) {
  prev_time_ = std::chrono::steady_clock::now();
  if constexpr (LEq<T>) {
    reached_end_ = [](const T& a, const T& b) { return b <= a; };
  }
}

template <typename Derived, typename T, typename F>
MoveBase<Derived, T, F>::MoveBase(const T& start_value, F&& shift)
    : Base(start_value), shift_(std::move(shift)) {
  Base::prev_time_ = std::chrono::steady_clock::now();
}

template <typename Derived, typename T>
void MoveBase<Derived, T, void>::set_end_value(const T& end) & {
  end_value_ = end;
}

template <typename Derived, typename T>
Derived&& MoveBase<Derived, T, void>::end_value(const T& end) && {
  set_end_value(end);
  return std::move(Base::me());
}

template <typename Derived, typename T>
void MoveBase<Derived, T, void>::set_duration(const duration_t& d) & {
  end_time_ = prev_time_ + d;
}

template <typename Derived, typename T>
Derived&& MoveBase<Derived, T, void>::duration(const duration_t& d) && {
  set_duration(d);
  return std::move(Base::me());
}

template <typename Derived, typename T>
void MoveBase<Derived, T, void>::set_reached_end(
    std::function<bool(const T&, const T&)>&& f) & {
  reached_end_ = std::move(f);
}

template <typename Derived, typename T>
Derived&& MoveBase<Derived, T, void>::reached_end(
    std::function<bool(const T&, const T&)>&& f) && {
  set_reached_end(std::move(f));
  return std::move(Base::me());
}

template <typename Derived, typename T>
typename MoveBase<Derived, T, void>::duration_t
MoveBase<Derived, T, void>::shift_time() {
  if (end_time_.has_value() && prev_time_ > end_time_.value()) {
    ended_ = true;
    return duration_t(0);
  }
  auto now = std::chrono::steady_clock::now();
  duration_t diff = std::chrono::duration_cast<duration_t>(now - prev_time_);
  prev_time_ = now;
  return diff;
}

template <typename Derived, typename T> T MoveBase<Derived, T, void>::get() {
  return cur_value_;
}

template <typename Derived, typename T, typename F>
T MoveBase<Derived, T, F>::get() {
  if (Base::ended_) {
    return Base::cur_value_;
  }
  auto diff = Base::shift_time();
  Base::cur_value_ = shift_(Base::cur_value_, diff.count());
  if (Base::end_value_.has_value()) {
    if (Base::reached_end_.value()(Base::cur_value_,
                                   Base::end_value_.value())) {
      Base::cur_value_ = Base::end_value_.value();
      Base::ended_ = true;
    }
  }
  return Base::cur_value_;
}

template <typename T, typename F>
Move<T, F>::Move(const T& start_value, F&& shift)
    : Base(start_value, std::move(shift)) {}
