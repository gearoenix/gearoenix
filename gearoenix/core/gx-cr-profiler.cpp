#include "gx-cr-profiler.hpp"
#if GX_PROFILING_MODE
#include "../platform/gx-plt-log.hpp"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace gearoenix::core::profiler {

using hi_clock = std::chrono::high_resolution_clock;
using time_point_t = hi_clock::time_point;
using duration_t = std::chrono::nanoseconds;

struct SvHash final {
    using is_transparent = void;
    std::size_t operator()(const std::string_view sv) const
    {
        return std::hash<std::string_view> {}(sv);
    }
};

struct SvEqual final {
    using is_transparent = void;
    bool operator()(const std::string_view a, const std::string_view b) const
    {
        return a == b;
    }
};

template <typename V>
using sv_map = std::unordered_map<std::string_view, V, SvHash, SvEqual>;

struct TagFrameAccumulator final {
    duration_t total_duration = duration_t::zero();
    std::uint32_t hit_count = 0;
};

struct TagStats final {
    double min_ms = std::numeric_limits<double>::max();
    double max_ms = 0.0;
    double total_ms = 0.0;
    std::uint64_t total_hits = 0;
    std::uint64_t frames_present = 0;
};

struct ThreadData final {
    sv_map<std::vector<time_point_t>> open_tags;
    sv_map<TagFrameAccumulator> current_frame_tags;
    ThreadData* next = nullptr;
    ThreadData* prev = nullptr;

    ~ThreadData();
};

static std::mutex global_lock;
static bool global_alive = true;
static ThreadData* thread_list_head = nullptr;
static thread_local ThreadData tl_data;
static thread_local bool tl_registered = false;

struct GlobalGuard final {
    ~GlobalGuard()
    {
        const std::lock_guard guard(global_lock);
        global_alive = false;
    }
};
static GlobalGuard global_guard;

static time_point_t frame_start = {};
static bool frame_active = false;
static std::uint64_t frame_count = 0;
static double min_frame_ms = std::numeric_limits<double>::max();
static double max_frame_ms = 0.0;
static double total_frame_ms = 0.0;

static sv_map<TagStats> all_tag_stats;

ThreadData::~ThreadData()
{
    const std::lock_guard guard(global_lock);
    if (!global_alive) {
        return;
    }
    if (prev) {
        prev->next = next;
    } else {
        thread_list_head = next;
    }
    if (next) {
        next->prev = prev;
    }
}

static double to_ms(const duration_t d)
{
    return std::chrono::duration<double, std::milli>(d).count();
}

static void ensure_registered()
{
    if (!tl_registered) {
        const std::lock_guard guard(global_lock);
        tl_data.next = thread_list_head;
        if (thread_list_head) {
            thread_list_head->prev = &tl_data;
        }
        thread_list_head = &tl_data;
        tl_registered = true;
    }
}

static void flush_frame()
{
    if (!frame_active) {
        return;
    }

    const auto frame_duration = hi_clock::now() - frame_start;
    const auto frame_ms = to_ms(frame_duration);

    ++frame_count;
    total_frame_ms += frame_ms;
    if (frame_ms < min_frame_ms) {
        min_frame_ms = frame_ms;
    }
    if (frame_ms > max_frame_ms) {
        max_frame_ms = frame_ms;
    }

    for (auto* td = thread_list_head; td != nullptr; td = td->next) {
        for (const auto& [tag, acc] : td->current_frame_tags) {
            auto& stats = all_tag_stats[tag];
            const auto tag_ms = to_ms(acc.total_duration);
            stats.total_ms += tag_ms;
            stats.total_hits += acc.hit_count;
            ++stats.frames_present;
            if (tag_ms < stats.min_ms) {
                stats.min_ms = tag_ms;
            }
            if (tag_ms > stats.max_ms) {
                stats.max_ms = tag_ms;
            }
        }
        td->current_frame_tags.clear();
        td->open_tags.clear();
    }

    frame_active = false;
}

void new_frame()
{
    const std::lock_guard guard(global_lock);
    flush_frame();
    frame_start = hi_clock::now();
    frame_active = true;
}

void begin(const char* const tag)
{
    ensure_registered();
    tl_data.open_tags[tag].push_back(hi_clock::now());
}

void end(const char* const tag)
{
    const auto now = hi_clock::now();
    const auto it = tl_data.open_tags.find(tag);
    if (it == tl_data.open_tags.end() || it->second.empty()) {
        return;
    }
    auto& acc = tl_data.current_frame_tags[tag];
    acc.total_duration += now - it->second.back();
    ++acc.hit_count;
    it->second.pop_back();
}

void report()
{
    const std::lock_guard guard(global_lock);
    flush_frame();

    if (frame_count == 0) {
        GX_LOG_I("Profiler: No frames recorded.");
        return;
    }

    const auto avg_frame_ms = total_frame_ms / static_cast<double>(frame_count);

    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);
    ss << "\n======== Profiler Report ========\n";
    ss << "Total frames: " << frame_count << "\n";
    ss << "Frame time (ms)  -  min: " << min_frame_ms
       << "  avg: " << avg_frame_ms
       << "  max: " << max_frame_ms << "\n";
    ss << "Avg FPS: " << (1000.0 / avg_frame_ms) << "\n";
    ss << "\n--- Per-Tag Statistics ---\n";

    std::vector<std::pair<std::string_view, const TagStats*>> sorted_tags;
    sorted_tags.reserve(all_tag_stats.size());
    std::size_t max_tag_len = 3; // "Tag"
    for (const auto& [tag, stats] : all_tag_stats) {
        sorted_tags.emplace_back(tag, &stats);
        if (tag.size() > max_tag_len) {
            max_tag_len = tag.size();
        }
    }
    std::sort(sorted_tags.begin(), sorted_tags.end(),
        [](const auto& a, const auto& b) { return a.second->total_ms > b.second->total_ms; });

    const auto tag_w = static_cast<int>(max_tag_len + 2);

    ss << std::left << std::setw(tag_w) << "Tag"
       << std::right << std::setw(10) << "Hits"
       << std::setw(12) << "Min(ms)"
       << std::setw(12) << "Avg(ms)"
       << std::setw(12) << "Max(ms)"
       << std::setw(12) << "Total(ms)"
       << std::setw(14) << "Frame(%)"
       << "\n";

    for (const auto& [tag, stats_ptr] : sorted_tags) {
        const auto& stats = *stats_ptr;
        const auto avg_tag_ms = stats.total_ms / static_cast<double>(stats.frames_present);
        const auto occupancy = (stats.total_ms / total_frame_ms) * 100.0;
        ss << std::left << std::setw(tag_w) << tag
           << std::right << std::setw(10) << stats.total_hits
           << std::setw(12) << stats.min_ms
           << std::setw(12) << avg_tag_ms
           << std::setw(12) << stats.max_ms
           << std::setw(12) << stats.total_ms
           << std::setw(13) << occupancy << "%"
           << "\n";
    }

    ss << "=================================\n";

    GX_LOG_I(ss.str());
}

}
#endif