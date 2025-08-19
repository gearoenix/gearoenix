#pragma once
#include "gx-cr-sync-thread.hpp"
#include <algorithm>
#include <execution>
#include <ranges>

namespace gearoenix::core::sync {
// typedef const std::function<void(unsigned int, unsigned int)> parallel_for_t;

// void parallel(parallel_for_t* fun);

template <typename Iter>
[[nodiscard]] bool advance(Iter& curr_iter, const Iter& iter_end, const auto n)
{
    for (auto i = decltype(n) { 0 };; ++i, ++curr_iter) {
        if (curr_iter == iter_end) {
            return false;
        }
        if (i >= n) {
            return true;
        }
    }
}

template <typename Iter, typename Fun>
void parallel_for(Iter iter_first, Iter iter_end, const Fun& fun)
{
    static const auto kernels_count = threads_count();
    static const auto indices = std::views::iota(0, kernels_count);

    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](auto const kernel_index) {
        Iter curr_iter = iter_first;
        if (!advance(curr_iter, iter_end, kernel_index)) {
            return;
        }
        do {
            fun(*curr_iter, kernel_index);
        } while (advance(curr_iter, iter_end, kernels_count));
    });
}

template <typename Container, typename Fun>
void parallel_for(Container& container, const Fun& fun)
{
    if (container.empty()) {
        return;
    }
    parallel_for(container.begin(), container.end(), fun);
}

template <typename Iter, typename Fun>
void parallel_for_i(Iter iter_first, Iter iter_end, const Fun& fun)
{
    static const auto kernels_count = threads_count();
    static const auto indices = std::views::iota(0, kernels_count);

    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](auto const kernel_index) {
        auto index = kernel_index;
        Iter curr_iter = iter_first;
        if (!advance(curr_iter, iter_end, kernel_index)) {
            return;
        }
        do {
            fun(*curr_iter, index, kernel_index);
            index += kernels_count;
        } while (advance(curr_iter, iter_end, kernels_count));
    });
}

template <typename Container, typename Fun>
void parallel_for_i(Container& container, const Fun& fun)
{
    if (container.empty()) {
        return;
    }
    parallel_for_i(container.begin(), container.end(), fun);
}
}