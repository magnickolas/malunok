#pragma once

#include "move.h"

template <typename T, typename F, typename... Sliders>
class MetaMove : public MoveBase<MetaMove<T, F, Sliders...>, T> {
  using Base = MoveBase<MetaMove<T, F, Sliders...>, T>;

private:
  template <std::size_t... I> T get_impl(std::index_sequence<I...>) {
    return shift_(Base::cur_value_, std::get<I>(sliders_).get()...);
  }

public:
  MetaMove(const T& start_value, F&& shift, const Sliders&... sliders);
  virtual T get() override;

private:
  F shift_;
  std::tuple<Sliders...> sliders_;
};

#include "metamove.ipp"
