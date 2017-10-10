#include "cr-gc.hpp"
#include "../../system/sys-log.hpp"
#include "../list/cr-list-node.hpp"
#include "../list/cr-list.hpp"
#include "cr-gc-range.hpp"
#include <utility>
#define DEBUG_GC
void gearoenix::core::gc::Gc::remove_range(const Range& r)
{
    const unsigned int start_offset = r.start->get_value()->end;
    const unsigned int range_size = (r.end->get_value()->offset) - start_offset;
    if (0 == range_size) {
        return;
    }
    auto search = free_ranges.find(range_size);
    if (free_ranges.end() == search) {
        LOGF("Unexpected");
    }
    auto& ranges = search->second;
    auto range_search = ranges.find(start_offset);
    if (ranges.end() == range_search) {
        LOGF("Unexpected");
    }
    ranges.erase(range_search);
    if (ranges.empty()) {
        free_ranges.erase(search);
    }
}

void gearoenix::core::gc::Gc::add_range(const Range& r)
{
    const unsigned int start_offset = r.start->get_value()->end;
    const unsigned int range_size = (r.end->get_value()->offset) - start_offset;
    if (range_size == 0) {
        return;
    }
    auto search = free_ranges.upper_bound(range_size - 1);
    if (range_size == search->first) {
        search->second[start_offset] = r;
        return;
    }
    std::map<unsigned int, Range> m;
    m[start_offset] = r;
    free_ranges.insert(search, std::make_pair(range_size, m));
}

void gearoenix::core::gc::Gc::deallocate(Object* obj)
{
    std::lock_guard<std::mutex> lg(lock);
    list::Node<Object*>* obj_node = obj->node;
    list::Node<Object*>* start_node = obj_node->get_previous();
    list::Node<Object*>* end_node = obj_node->get_next();
    remove_range(Range(start_node, obj_node));
    remove_range(Range(obj_node, end_node));
    delete obj_node;
    add_range(Range(start_node, end_node));
}

gearoenix::core::gc::Gc::Gc(unsigned int size)
    : Object(size)
    , objects(new list::List<Object*>)
{
    Object* start = new Object(0);
    Object* end = new Object(0);
    objects->add_front(start);
    objects->add_end(end);
    Range r(objects->get_front(), objects->get_end());
    std::map<unsigned int, Range> ranges;
    ranges[0] = r;
    free_ranges[size] = ranges;
}

gearoenix::core::gc::Gc::~Gc()
{
    std::lock_guard<std::mutex> lg(lock);
    for (auto node = objects->get_front(); node != nullptr; node = node->get_next()) {
        Object* obj = node->get_value();
        obj->garbage_collector = nullptr;
        delete obj;
    }
    delete objects;
    free_ranges.clear();
}

void gearoenix::core::gc::Gc::allocate(Object* obj)
{
    std::lock_guard<std::mutex> lg(lock);
#ifdef DEBUG_GC
    if (0 == obj->size) {
        LOGF("Wrong object size! (0 size)");
    }
#endif
    auto search = free_ranges.upper_bound(obj->size - 1);
#ifdef DEBUG_GC
    if (search == free_ranges.end()) {
        LOGF("Out of space in GC");
    }
    if (search->second.empty()) {
        LOGF("Unexpected");
    }
#endif
    auto range_search = search->second.begin();
    Range& range = range_search->second;
    list::Node<Object*>* start_node = range.start;
    list::Node<Object*>* end_node = range.end;
    start_node->add_as_next(obj);
    list::Node<Object*>* obj_node = start_node->get_next();
    obj->garbage_collector = this;
    obj->node = obj_node;
    obj->offset = start_node->get_value()->end;
    obj->end = (obj->size) + (obj->offset);
    remove_range(range);
    add_range(Range(obj_node, end_node));
}

gearoenix::core::list::List<gearoenix::core::gc::Object*>* gearoenix::core::gc::Gc::get_objects()
{
    return objects;
}
