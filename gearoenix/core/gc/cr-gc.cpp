#include "cr-gc.hpp"
#include "../../system/sys-log.hpp"
#include "../list/cr-list-node.hpp"
#include "../list/cr-list.hpp"
#include "cr-gc-range.hpp"
#define DEBUG_GC
gearoenix::core::gc::Gc::Gc(int size)
    : Object(size)
    , objects(new list::List<Object*>)
{
    Object* start = new Object(0);
    Object* end = new Object(0);
    objects->add_front(start);
    objects->add_end(end);
    Range r(start, end);
    std::map<unsigned int, Range> ranges;
    ranges[0] = r;
    free_ranges[size] = ranges;
}

gearoenix::core::gc::Gc::~Gc()
{
    std::lock_guard<std::mutex>(lock);
    for (auto node = objects->get_front(); node != nullptr; node = node->get_next()) {
        delete node->get_value();
    }
    delete objects;
    free_ranges.clear();
}

void gearoenix::core::gc::Gc::allocate(Object* obj)
{
    obj->garbage_collector = this;
    std::lock_guard<std::mutex>(lock);
    auto search = free_ranges.upper_bound(obj->size - 1);
#ifdef DEBUG_GC
    if (search == free_ranges.end()) {
        LOGF("Out of space in GC");
    }
    if (search->second.empty()) {
        LOGF("Unexpected");
    }
#endif
    auto range = search->second.begin();
    obj->offset = range->second->start->get_value()->end;
    obj->end = (obj->size) + (obj->offset);
    list::Node<Object*>* end_node = range->second->end;
    search->second.erase(range);
    range->second->start->add_as_next(obj);
    if (search->second.empty()) {
        free_ranges.erase(search);
    }
    unsigned int range_size = (obj->end) - (end_node->get_value()->start);
    if (0 == range_size) {
        return;
    }
    Range r(end_node->get_previous(), end_node);
    auto insert_search = free_ranges.find(range_size - 1);
    if (insert_search->first == range_size) {
        insert_search->second[obj->end] = r;
        return;
    }
    std::map<unsigned int, Range> m;
    m[obj->end] = r;
    free_ranges.insert(insert_search, m);
}
