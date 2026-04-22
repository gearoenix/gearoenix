#pragma once
#include <functional>
#include <iterator>

namespace gearoenix::core::sync {
typedef void (*parallel_for_t)(unsigned int /*kernel_index*/, unsigned int /*kernels_count*/, void* const /*user-data*/);

void parallel(parallel_for_t fun, void* fun_data);

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
    struct Context {
        const Iter& iter_first;
        const Iter& iter_end;
        const Fun& fun;
    } ctx { iter_first, iter_end, fun };

    if constexpr (std::random_access_iterator<Iter>) {
        parallel(+[](const unsigned int kernel_index, const unsigned int kernels_count, void* const context) -> void {
            const auto& [begin_iter, end_iter, f] = *static_cast<const Context*>(context);
            const auto total = static_cast<unsigned int>(end_iter - begin_iter);
            const auto chunk = total / kernels_count;
            const auto rem = total % kernels_count;
            const auto start = chunk * kernel_index + (kernel_index < rem ? kernel_index : rem);
            const auto end = start + chunk + (kernel_index < rem ? 1u : 0u);
            auto iter = begin_iter + start;
            for (auto i = start; i < end; ++i, ++iter) {
                f(*iter, kernel_index);
            } }, &ctx);
    } else {
        parallel(+[](const unsigned int kernel_index, const unsigned int kernels_count, const void* const context) -> void {
            const auto& [first_iter, end_iter, f] = *static_cast<const Context*>(context);
            Iter curr_iter = first_iter;
            if (!advance(curr_iter, end_iter, kernel_index)) {
                return;
            }
            do {
                f(*curr_iter, kernel_index);
            } while (advance(curr_iter, end_iter, kernels_count)); }, &ctx);
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
    struct Context {
        const Iter& iter_first;
        const Iter& iter_end;
        const Fun& fun;
    } ctx { iter_first, iter_end, fun };

    if constexpr (std::random_access_iterator<Iter>) {
        parallel(+[](const unsigned int kernel_index, const unsigned int kernels_count, void* const context) -> void {
            const auto& [first_iter, end_iter, f] = *static_cast<const Context*>(context);
            const auto total = static_cast<unsigned int>(end_iter - first_iter);
            const auto chunk = total / kernels_count;
            const auto rem = total % kernels_count;
            const auto start = chunk * kernel_index + (kernel_index < rem ? kernel_index : rem);
            const auto end = start + chunk + (kernel_index < rem ? 1u : 0u);
            auto iter = first_iter + start;
            for (auto i = start; i < end; ++i, ++iter) {
                f(*iter, i, kernel_index);
            } }, &ctx);
    } else {
        parallel(+[](const unsigned int kernel_index, const unsigned int kernels_count, void* const context) {
            const auto& [first_iter, end_iter, f] = *static_cast<const Context*>(context);
            auto index = kernel_index;
            Iter curr_iter = first_iter;
            if (!advance(curr_iter, end_iter, kernel_index)) {
                return;
            }
            do {
                f(*curr_iter, index, kernel_index);
                index += kernels_count;
            } while (advance(curr_iter, end_iter, kernels_count)); }, &ctx);
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