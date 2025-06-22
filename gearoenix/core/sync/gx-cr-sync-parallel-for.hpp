#pragma once
#include <functional>

namespace gearoenix::core::sync {
struct ParallelFor final {

    static void exec(const std::function<void(unsigned int, unsigned int)>& fun);

    ParallelFor() = delete;
    ~ParallelFor() = delete;

    template <typename Iter>
    static void advance(Iter& iter, const Iter& end, auto n)
    {
        for (auto i = decltype(n) { 0 }; i < n; ++i, ++iter) {
            if (iter == end) {
                return;
            }
        }
    }

    template <typename Iter, typename Fun>
    static void exec(Iter first, Iter end, Fun&& f)
    {
        exec([&](const auto kernels_count, const auto kernel_index) {
            Iter iter = first;
            advance(iter, end, kernel_index);
            while (iter != end) {
                f(*iter, kernel_index);
                advance(iter, end, kernels_count);
            }
        });
    }

    template <typename Iter, typename Fun>
    static void execi(Iter first, Iter end, Fun&& f)
    {
        exec([&](const auto kernels_count, const auto kernel_index) {
            auto index = kernel_index;
            Iter iter = first;
            advance(iter, end, kernel_index);
            for (; iter != end; index += kernels_count) {
                f(*iter, index, kernel_index);
                advance(iter, end, kernels_count);
            }
        });
    }
};
}