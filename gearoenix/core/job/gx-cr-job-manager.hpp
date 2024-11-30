#pragma once
#include <functional>
#include <optional>
#include <thread>

namespace gearoenix::core::job {
void initialise();

/// \note Call this function in a right time otherwise it can cause trace and violent memory access.
/// \note The right time to call this is the start of the application
void register_current_thread();

/// \note Call this function in a right time otherwise it can cause trace and violent memory access
/// \note The right time to call this is the start of the application
void register_thread(std::thread::id thread_id, std::optional<std::thread>&& thread = std::nullopt);

/// \note Call this function in a right time otherwise it can cause trace and violent memory access
/// \note The right time to call this is the start of the application
[[nodiscard]] std::thread::id register_new_thread();

void send_job(std::thread::id receiver_thread_id, std::function<void()>&& job);
void send_job_io1(std::function<void()>&& job);
void send_job_net1(std::function<void()>&& job);

void execute_current_thread_jobs();

void terminate();
}
