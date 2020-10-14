#include "gx-cr-sync-kernel-workers.hpp"
#include <utility>

#ifndef GX_THREAD_NOT_SUPPORTED
#include "gx-cr-sync-semaphore.hpp"

void gearoenix::core::sync::KernelWorkers::thread_loop(const unsigned int kernel_index) noexcept
{
    state = State::Running;
#define GX_HELPER                  \
    if (state != State::Running) { \
        state = State::Terminated; \
        return;                    \
    }
    while (state == State::Running) {
        signals[kernel_index]->lock();
        GX_HELPER
        std::lock_guard<std::mutex> _lock(*workers_syncers[kernel_index]);
        for (const Worker& worker : workers) {
            GX_HELPER
            worker.waits[kernel_index]->lock();
            GX_HELPER
            worker.worker(kernel_index);
            GX_HELPER
            worker.signals[kernel_index]->release();
        }
    }
#undef GX_HELPER
}
#endif

gearoenix::core::sync::KernelWorkers::KernelWorkers() noexcept
#ifdef GX_THREAD_NOT_SUPPORTED
    = default;
#else
{
    const unsigned int kernels_count = std::thread::hardware_concurrency();
    signals.reserve(kernels_count);
    threads.reserve(kernels_count);
    workers_syncers.reserve(kernels_count);
    for (unsigned int ki = 0; ki < kernels_count; ++ki) {
        signals.push_back(std::make_shared<Semaphore>());
        threads.emplace_back([this, ki] { thread_loop(ki); });
        workers_syncers.push_back(std::make_shared<std::mutex>());
    }
}
#endif

gearoenix::core::sync::KernelWorkers::~KernelWorkers() noexcept
#ifdef GX_THREAD_NOT_SUPPORTED
    = default;
#else
{
    state = State::Terminating;
    while (state != State::Terminated)
        for (const auto& s : signals)
            s->release();
    for (std::thread& t : threads)
        t.join();
}
#endif

void gearoenix::core::sync::KernelWorkers::add_step(std::function<void(const unsigned int)> worker, std::function<void()> receiver) noexcept
{
    add_step([] {}, std::move(worker), [] {}, std::move(receiver));
}

void gearoenix::core::sync::KernelWorkers::add_step(std::function<void()> sender, std::function<void(const unsigned int)> worker, std::function<void()> meanwhile, std::function<void()> receiver) noexcept
{
#ifdef GX_THREAD_NOT_SUPPORTED
    workers.push_back(Worker {
        .sender = std::move(sender),
        .worker = std::move(worker),
        .meanwhile = std::move(meanwhile),
        .receiver = std::move(receiver) });
#else
    const size_t kernels_count = threads.size();
    std::vector<std::shared_ptr<Semaphore>> waits;
    std::vector<std::shared_ptr<Semaphore>> worker_signals;
    waits.reserve(kernels_count);
    worker_signals.reserve(kernels_count);
    for (size_t ki = 0; ki < kernels_count; ++ki) {
        waits.push_back(std::make_shared<Semaphore>());
        worker_signals.push_back(std::make_shared<Semaphore>());
    }
    for (std::shared_ptr<std::mutex>& m : workers_syncers)
        m->lock();
    workers.push_back(Worker {
        .waits = waits,
        .signals = worker_signals,
        .sender = std::move(sender),
        .worker = std::move(worker),
        .meanwhile = std::move(meanwhile),
        .receiver = std::move(receiver) });
    for (std::shared_ptr<std::mutex>& m : workers_syncers)
        m->unlock();
#endif
}

void gearoenix::core::sync::KernelWorkers::do_steps() noexcept
{
#ifdef GX_THREAD_NOT_SUPPORTED
    for (Worker& worker : workers) {
        worker.sender();
        worker.worker(0);
        worker.meanwhile();
        worker.receiver();
    }
#else
    for (const std::shared_ptr<Semaphore>& signal : signals)
        signal->release();
    for (Worker& worker : workers) {
        worker.sender();
        for (const std::shared_ptr<Semaphore>& wait : worker.waits)
            wait->release();
        worker.meanwhile();
        for (const std::shared_ptr<Semaphore>& signal : worker.signals)
            signal->lock();
        worker.receiver();
    }
#endif
}

unsigned int gearoenix::core::sync::KernelWorkers::get_threads_count() const noexcept
{
#ifdef GX_THREAD_NOT_SUPPORTED
    return static_cast<unsigned int>(1);
#else
    return static_cast<unsigned int>(threads.size());
#endif
}
