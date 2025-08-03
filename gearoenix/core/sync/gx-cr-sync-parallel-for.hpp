#pragma once
#include <functional>
#include <stacktrace>

namespace gearoenix::core::sync {
struct ParallelFor final {

    static void exec(const std::function<void(unsigned int, unsigned int)>& fun);

    ParallelFor() = delete;
    ~ParallelFor() = delete;

    template <typename Iter>
    [[nodiscard]] static bool advance(Iter& curr_iter, const Iter& iter_end, const auto n)
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
    static void exec(Iter iter_first, Iter iter_end, Fun&& f)
    {
        exec([&](const auto kernels_count, const auto kernel_index) {
            Iter curr_iter = iter_first;
            if (!advance(curr_iter, iter_end, kernel_index)) {
                return;
            }
            do {
                f(*curr_iter, kernel_index);
            } while (advance(curr_iter, iter_end, kernels_count));
        });
    }

    template <typename Iter, typename Fun>
    static void execi(Iter iter_first, Iter iter_end, Fun&& f)
    {
        exec([&](const auto kernels_count, const auto kernel_index) {
            auto index = kernel_index;
            Iter curr_iter = iter_first;
            if (!advance(curr_iter, iter_end, kernel_index)) {
                return;
            }
            do {
                f(*curr_iter, index, kernel_index);
                index += kernels_count;
            } while (advance(curr_iter, iter_end, kernels_count));
        });
    }
};
}