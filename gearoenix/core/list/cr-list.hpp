#ifndef GEAROENIX_CORE_LIST_LIST_HPP
#define GEAROENIX_CORE_LIST_LIST_HPP
#include "cr-list-node.hpp"
namespace gearoenix {
namespace core {
    namespace list {
        template <class T>
        class Node<T>;
        template <class T>
        class List {
        private:
            Node<T>* front;
            Node<T>* end;

        protected:
        public:
            List();
            ~List();
            Node<T>* get_front();
            const Node<T>* get_front() const;
            Node<T>* get_end();
            const Node<T>* get_end() const;
        };
    }
}
}

template <class T>
gearoenix::core::list::List::List()
    : front(nullptr)
    , end(nullptr)
{
}

template <class T>
gearoenix::core::list::List::~List()
{
    Node<T>* node = front;
    while (node != nullptr) {
        Node<T>* tmp = node;
        node = node->child;
        delete tmp;
    }
}

template <class T>
gearoenix::core::list::Node<T>* gearoenix::core::list::List::get_front()
{
    return front;
}

template <class T>
const gearoenix::core::list::Node<T>* gearoenix::core::list::List::get_front() const
{
    return front;
}

template <class T>
gearoenix::core::list::Node<T>* gearoenix::core::list::List::get_end()
{
    return end;
}

template <class T>
const gearoenix::core::list::Node<T>* gearoenix::core::list::List::get_end() const
{
    return end;
}

#endif
