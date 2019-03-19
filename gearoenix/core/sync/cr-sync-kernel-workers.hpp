#ifndef GEAROENIX_CORE_SYNC_KERNEL_WORKERS_HPP
#define GEAROENIX_CORE_SYNC_KERNEL_WORKERS_HPP
#include <memory>
#include <vector>
#include <thread> 
#include <functional>

namespace gearoenix {
	namespace core {
		namespace sync {
			class QueuedSemaphore;
			class KernelWorker {
			private:
				struct Worker {
					const std::function<void(unsigned int)> worker;
					const std::function<void()> boss;
					const std::vector<std::shared_ptr<QueuedSemaphore>> semaphores;
				};
				std::vector<std::thread> thread;
				std::vector<std::>
				std::vector<Worker> workers;
				volatile bool running = true;

				void loop();

			public:
				void add_step(std::function<void(unsigned int)> worker, std::function<void()> boss = [] {});
				void do_steps();
			};
		}
	}
}
#endif
