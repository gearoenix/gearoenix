#include "cr-sync-kernel-workers.hpp"
#include "cr-sync-semaphore.hpp"
#include "../../system/sys-log.hpp"

void gearoenix::core::sync::KernelWorker::thread_loop(const unsigned int kernel_index) noexcept
{
#define GX_HELPER           \
    if (state != RUNNING) { \
        state = TERMINATED; \
        return;             \
    }
    while (state == RUNNING) {
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

gearoenix::core::sync::KernelWorker::KernelWorker() noexcept
{
    const unsigned int kernels_count = std::thread::hardware_concurrency();
    signals.reserve(kernels_count);
    threads.reserve(kernels_count);
    workers_syncers.reserve(kernels_count);
    for (unsigned int ki = 0; ki < kernels_count; ++ki) {
        signals.push_back(std::make_shared<Semaphore>());
        threads.push_back(std::thread(std::bind(&KernelWorker::thread_loop, this, ki)));
        workers_syncers.push_back(std::make_shared<std::mutex>());
    }
}

gearoenix::core::sync::KernelWorker::~KernelWorker() noexcept
{
    state = TERMINATING;
    while (state != TERMINATED)
        for (const auto& s : signals)
            s->release();
    for (std::thread& t : threads)
        t.join();
}

void gearoenix::core::sync::KernelWorker::add_step(std::function<void(const unsigned int)> worker, std::function<void()> receiver) noexcept
{
    add_step([] {}, worker, receiver);
}

void gearoenix::core::sync::KernelWorker::add_step(std::function<void()> sender, std::function<void(const unsigned int)> worker, std::function<void()> receiver) noexcept
{
    const size_t kernels_count = threads.size();
    std::vector<std::shared_ptr<Semaphore>> waits;
    std::vector<std::shared_ptr<Semaphore>> signals;
    waits.reserve(kernels_count);
    signals.reserve(kernels_count);
    for (size_t ki = 0; ki < kernels_count; ++ki) {
        waits.push_back(std::make_shared<Semaphore>());
        signals.push_back(std::make_shared<Semaphore>());
    }
    for (std::shared_ptr<std::mutex>& m : workers_syncers)
        m->lock();
    workers.push_back({ 
        waits,
        sender,
        worker,
        receiver,
        signals
    });
    for (std::shared_ptr<std::mutex>& m : workers_syncers)
        m->unlock();
}

void gearoenix::core::sync::KernelWorker::do_steps() noexcept
{
    for (const std::shared_ptr<Semaphore>& signal : signals)
        signal->release();
    for (Worker& worker : workers) {
        worker.sender();
        for (const std::shared_ptr<Semaphore>& wait : worker.waits)
            wait->release();
        for (const std::shared_ptr<Semaphore>& signal : worker.signals)
            signal->lock();
        worker.receiver();
    }
}

unsigned int gearoenix::core::sync::KernelWorker::get_threads_count() const noexcept
{
    return static_cast<unsigned int>(threads.size());
}
