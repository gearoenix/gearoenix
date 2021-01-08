#ifndef GEAROENIX_CORE_LIST_LIST_HPP
#define GEAROENIX_CORE_LIST_LIST_HPP
#include "gx-cr-list-node.hpp"

namespace gearoenix::core::list {
template <typename T>
struct List {
    friend struct Node<T>;

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
    void add_front(const T& val);
    void add_end(const T& val);
};
}

template <typename T>
gearoenix::core::list::List<T>::List()
    : front(nullptr)
    , end(nullptr)
{
}

template <typename T>
gearoenix::core::list::List<T>::~List()
{
    Node<T>* node = front;
    while (node != nullptr) {
        Node<T>* tmp = node;
        node = node->next;
        delete tmp;
    }
}

template <typename T>
gearoenix::core::list::Node<T>* gearoenix::core::list::List<T>::get_front()
{
    return front;
}

template <typename T>
const gearoenix::core::list::Node<T>* gearoenix::core::list::List<T>::get_front() const
{
    return front;
}

template <typename T>
gearoenix::core::list::Node<T>* gearoenix::core::list::List<T>::get_end()
{
    return end;
}

template <typename T>
const gearoenix::core::list::Node<T>* gearoenix::core::list::List<T>::get_end() const
{
    return end;
}

template <typename T>
void gearoenix::core::list::List<T>::add_front(const T& val)
{
    if (front != nullptr) {
        front->add_as_previous(val);
    } else {
        front = new Node<T>(val, this);
        end = front;
    }
}

template <typename T>
void gearoenix::core::list::List<T>::add_end(const T& val)
{
    if (end != nullptr) {
        end->add_as_next(val);
    } else {
        end = new Node<T>(val, this);
        front = end;
    }
}

#endif
