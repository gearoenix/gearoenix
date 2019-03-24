#include "cr-sync-kernel-workers.hpp"
#include "cr-sync-signaler.hpp"

void gearoenix::core::sync::KernelWorker::thread_loop(const unsigned int kernel_index)
{
#define GXHELPER if(!running) { terminated = true; return; }
	while (running) {
		signals[kernel_index]->wait();
		GXHELPER;
		std::lock_guard<std::mutex> _lock(workers_syncer);
		for (const Worker &worker : workers)
		{
			GXHELPER;
			worker.waits[kernel_index]->wait();
			GXHELPER;
			worker.worker(kernel_index);
			GXHELPER;
			worker.signals[kernel_index]->signal();
		}
	}
#undef GXHELPER
}

gearoenix::core::sync::KernelWorker::KernelWorker()
{
	const unsigned int kernels_count = std::thread::hardware_concurrency();
	signals.reserve(kernels_count);
	threads.reserve(kernels_count);
	for (unsigned int ki = 0; ki < kernels_count; ++ki)
	{
		signals.push_back(std::make_shared<Signaler>());
		threads.push_back(std::thread(std::bind(&KernelWorker::thread_loop, this, ki)));
	}
}

gearoenix::core::sync::KernelWorker::~KernelWorker()
{
	running = false;
	while (!terminated)
		for (const auto s : signals)
			s->signal();
	for (std::thread &t : threads)
		t.join();
}

void gearoenix::core::sync::KernelWorker::add_step(std::function<void(const unsigned int)> worker, std::function<void()> boss)
{
	const size_t kernels_count = threads.size();
	std::vector<std::shared_ptr<Signaler>> waits;
	std::vector<std::shared_ptr<Signaler>> signals;
	waits.reserve(kernels_count);
	signals.reserve(kernels_count);
	for (rsize_t ki = 0; ki < kernels_count; ++ki)
	{
		waits.push_back(std::make_shared<Signaler>());
		signals.push_back(std::make_shared<Signaler>());
	}
	std::lock_guard<std::mutex> _lock(workers_syncer);
	workers.push_back({
		waits,
		worker,
		boss,
		signals});
}

void gearoenix::core::sync::KernelWorker::do_steps()
{
	for (const std::shared_ptr<Signaler> &signal : signals)
		signal->signal();
	for (Worker &worker : workers)
	{
		for (const std::shared_ptr<Signaler> &wait : worker.waits)
			wait->signal();
		for (const std::shared_ptr<Signaler> &signal : worker.signals)
			signal->signal();
		worker.boss();
	}
}
