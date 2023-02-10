#pragma once

#include <tuple>

template <typename T, typename F, typename... Sliders> class MetaMove {
private:
  template <std::size_t... I> T get_impl(std::index_sequence<I...>) {
    return shift_(start_value_, std::get<I>(sliders_).get()...);
  }

public:
  MetaMove(const T& start_value, F&& shift, const Sliders&... sliders);
  T get();

private:
  T start_value_;
  F shift_;
  std::tuple<Sliders...> sliders_;
};

#include "metamove.ipp"
