#pragma once
#include <functional>

namespace gearoenix::core::sync {
typedef const std::function<void(unsigned int /*kernel_index*/, unsigned int /*kernels_count*/)> parallel_for_t;

void parallel(const parallel_for_t& fun);

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
void parallel_for(const Iter& iter_first, const Iter& iter_end, const Fun& fun)
{
    const parallel_for_t f = [&](auto const kernel_index, auto const kernels_count) {
        Iter curr_iter = iter_first;
        if (!advance(curr_iter, iter_end, kernel_index)) {
            return;
        }
        do {
            fun(*curr_iter, kernel_index);
        } while (advance(curr_iter, iter_end, kernels_count));
    };
    parallel(f);
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
void parallel_for_i(const Iter& iter_first, const Iter& iter_end, const Fun& fun)
{
    const parallel_for_t f = [&](auto const kernel_index, auto const kernels_count) {
        auto index = kernel_index;
        Iter curr_iter = iter_first;
        if (!advance(curr_iter, iter_end, kernel_index)) {
            return;
        }
        do {
            fun(*curr_iter, index, kernel_index);
            index += kernels_count;
        } while (advance(curr_iter, iter_end, kernels_count));
    };
    parallel(f);
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