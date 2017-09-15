#ifndef GEAROENIX_CORE_GC_RANGE_HPP
#define GEAROENIX_CORE_GC_RANGE_HPP
namespace std {
class mutex;
}
namespace gearoenix {
namespace core {
    namespace list {
        template <typename T>
        class List;
        template <typename T>
        class Node;
    }
    namespace gc {
        class GcObject;
        class Range {
        private:
            list::Node<GcObject>* start;
            list::Node<GcObject>* end;
            Range(list::Node<GcObject>* start, list::Node<GcObject>* end);

        protected:
        public:
            ~Range();
        };
    }
}
}
#endif // GEAROENIX_CORE_GC_RANGE_HPP
