#ifndef GEAROENIX_CORE_SYNC_STOP_POINT_HPP
#define GEAROENIX_CORE_SYNC_STOP_POINT_HPP
#include <memory>
#include <thread> 
#include <functional>

namespace gearoenix {
	namespace core {
		class FunctionLoader;
		namespace sync {
			class Semaphore;
			class WorkWaiter {
			private:
				const std::shared_ptr<Semaphore> semaphore;
				const std::shared_ptr<FunctionLoader> function_loader;
				std::thread thread;
				volatile bool running = true;

				void wait_loop();

			public:
				WorkWaiter();
				~WorkWaiter();
				void push(std::function<void()> f);
			};
		}
	}
}
#endif
