#ifndef GEAROENIX_CORE_GC_GC_HPP
#define GEAROENIX_CORE_GC_GC_HPP
#include "cr-gc-object.hpp"
#include <map>
#include <mutex>
namespace gearoenix {
namespace core {
    namespace list {
        template <typename T>
        class List;
        template <typename T>
        class Node;
    }
    namespace gc {
        class Range;
        class Gc : public Object {
            friend class Object;

        private:
            std::map<unsigned int, std::map<unsigned int, Range>> free_ranges;
            std::mutex lock;
            // is owner of objects
            list::List<Object*>* objects;
            void remove_range(const Range& r);
            void add_range(const Range& r);
            void deallocate(Object* obj);

        protected:
        public:
            Gc(unsigned int size);
            ~Gc();
            void allocate(Object* obj);
        };
    }
}
}
#endif // GEAROENIX_CORE_GC_GC_HPP
