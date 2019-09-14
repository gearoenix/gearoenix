#ifndef GEAROENIX_CORE_EVENT_ENGINE_HPP
#define GEAROENIX_CORE_EVENT_ENGINE_HPP
#include "../cr-types.hpp"
#include "../sync/cr-sync-semaphore.hpp"
#include "cr-ev-event.hpp"
#include "cr-ev-id.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <thread>
#include <atomic>
namespace gearoenix::core::event {
class Listner;
class Engine {
private:
    enum struct State : int {
        Running = 1,
        Terminating = 2,
        Terminated = 3,
    };

    std::atomic<State> state = State::Running;
    sync::Semaphore signaler;
    std::mutex events_guard;
    std::vector<Data> events;
    std::mutex listners_guard;
    std::map<Id, std::map<Real, std::set<Listner*>>> events_id_priority_listners;
    std::thread event_thread;

    void loop() noexcept;

public:
    Engine() noexcept;
    ~Engine() noexcept;
    void add_listner(Id event_id, Real priority, Listner* listner) noexcept;
    // Best function to remove listner
    void remove_listner(Id event_id, Real priority, Listner* listner) noexcept;
    void remove_listner(Id event_id, Listner* listner) noexcept;
    void remove_listner(Listner* listner) noexcept;
    void braodcast(Data event_data) noexcept;
};
}
#endif