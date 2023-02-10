#include "metamove.h"

template <typename T, typename F, typename... Sliders>
MetaMove<T, F, Sliders...>::MetaMove(const T& start_value, F&& shift,
                                     const Sliders&... sliders)
    : start_value_(start_value), shift_(std::move(shift)),
      sliders_(sliders...) {}

template <typename T, typename F, typename... Sliders>
T MetaMove<T, F, Sliders...>::get() {
  return get_impl(std::index_sequence_for<Sliders...>());
}
