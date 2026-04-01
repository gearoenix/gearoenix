#pragma once
#include <functional>
#include <iterator>

namespace gearoenix::core::sync {
typedef std::function<void(unsigned int /*kernel_index*/, unsigned int /*kernels_count*/)> parallel_for_t;

void parallel(parallel_for_t&& fun);

template <typename Iter>
[[nodiscard]] bool advance(Iter& curr_iter, const Iter& iter_end, const auto n)
{
    if constexpr (std::random_access_iterator<Iter>) {
        if (iter_end - curr_iter <= static_cast<std::iter_difference_t<Iter>>(n)) {
            return false;
        }
        curr_iter += n;
        return true;
    } else {
        for (auto i = decltype(n) { 0 };; ++i, ++curr_iter) {
            if (curr_iter == iter_end) {
                return false;
            }
            if (i >= n) {
                return true;
            }
        }
    }
}

template <typename Iter, typename Fun>
void parallel_for(const Iter& iter_first, const Iter& iter_end, const Fun& fun)
{
    if constexpr (std::random_access_iterator<Iter>) {
        parallel([&](auto const kernel_index, auto const kernels_count) {
            const auto total = static_cast<unsigned int>(iter_end - iter_first);
            const auto chunk = total / kernels_count;
            const auto rem = total % kernels_count;
            const auto start = chunk * kernel_index + (kernel_index < rem ? kernel_index : rem);
            const auto end = start + chunk + (kernel_index < rem ? 1u : 0u);
            auto iter = iter_first + start;
            for (auto i = start; i < end; ++i, ++iter) {
                fun(*iter, kernel_index);
            }
        });
    } else {
        parallel([&](auto const kernel_index, auto const kernels_count) {
            Iter curr_iter = iter_first;
            if (!advance(curr_iter, iter_end, kernel_index)) {
                return;
            }
            do {
                fun(*curr_iter, kernel_index);
            } while (advance(curr_iter, iter_end, kernels_count));
        });
    }
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
    if constexpr (std::random_access_iterator<Iter>) {
        parallel([&](auto const kernel_index, auto const kernels_count) {
            const auto total = static_cast<unsigned int>(iter_end - iter_first);
            const auto chunk = total / kernels_count;
            const auto rem = total % kernels_count;
            const auto start = chunk * kernel_index + (kernel_index < rem ? kernel_index : rem);
            const auto end = start + chunk + (kernel_index < rem ? 1u : 0u);
            auto iter = iter_first + start;
            for (auto i = start; i < end; ++i, ++iter) {
                fun(*iter, i, kernel_index);
            }
        });
    } else {
        parallel([&](auto const kernel_index, auto const kernels_count) {
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