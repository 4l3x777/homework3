#pragma once

#include <iostream>
#include <memory>

using namespace std;

template <typename T, class A = std::allocator<T>>
class MyList
{
public:
    struct Node {
        T _data;
        Node* _next_node;    
    };

    MyList(const A& allocator = A()) : _allocator_node(const_cast<A&>(allocator)) {};
    ~MyList();
    void push_back(const T& data);

    class Iterator {
    public:
        Iterator(const MyList* list, const Node* node) : _container(const_cast<MyList*>(list)), _current_node(const_cast<Node*>(node)) {};
        void operator++();
        const T& operator*();
        const Iterator& operator=(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
    private:
        MyList* _container;
        MyList::Node* _current_node;
    };
    friend class Iterator;

    Iterator begin() const;
    Iterator end() const;
    const Iterator cbegin() const;
    const Iterator cend() const;
private:
    void delete_node(Node* node);

    Node* _head {nullptr};
    Node* _tail {nullptr};
    Node* _end {nullptr};

    using rebind_allocator = typename std::allocator_traits<A>::template rebind_alloc<Node>;
    rebind_allocator _allocator_node;
};


////MyList

template <typename T, class A>
MyList<T, A>::~MyList() {
    auto current_node = _head;
    while (current_node != nullptr) {
        auto next_node = current_node->_next_node;
        delete_node(current_node);
        current_node = next_node;
    }
} 

template <typename T, class A>
void MyList<T, A>::delete_node(Node* node)
{
    _allocator_node.destroy(node);
    _allocator_node.deallocate(node, 1);
}

template <typename T, class A>
void MyList<T, A>::push_back(const T& data) {
    if (_tail == nullptr && _head == nullptr) {
        _end = _allocator_node.allocate(1);
        _allocator_node.construct(_end, Node{T(), nullptr});
        _head = _end;
        _tail = _end;
    }
    
    Node* node = _allocator_node.allocate(1);
    _allocator_node.construct(node, Node{data, nullptr});
    if (_tail != _end) _tail->_next_node = node;
    _tail = node;
    _tail->_next_node = _end;
    if (_head == _end) _head = node;
}

template <typename T, class A>
typename MyList<T, A>::Iterator MyList<T, A>::begin() const {
    return MyList<T, A>::Iterator(this, this->_head);
}

template <typename T, class A>
typename MyList<T, A>::Iterator MyList<T, A>::end() const {
    return MyList<T, A>::Iterator(this, this->_end);
}

template <typename T, class A>
const typename MyList<T, A>::Iterator MyList<T, A>::cbegin() const {
    return const_cast<const typename MyList<T, A>::Iterator&>(Iterator(this, this->_head));
}

template <typename T, class A>
const typename MyList<T, A>::Iterator MyList<T, A>::cend() const {
    return const_cast<const typename MyList<T, A>::Node*>(Iterator(this, this->_end));
}

//Mylist::Iterator

template <typename T, class A>
bool MyList<T, A>::Iterator::operator!=(const Iterator& other) const {
    if (_current_node != other._current_node) return true;
        return false;
}

template <typename T, class A>
const typename MyList<T, A>::Iterator& MyList<T, A>::Iterator::operator=(const Iterator& other) const {
    _container = other._container;
    _current_node = other._current_node;
    return *this;
}

template <typename T, class A>
void MyList<T, A>::Iterator::operator++(){
    if (_current_node != nullptr) _current_node = _current_node->_next_node;
}

template <typename T, class A>
const T& MyList<T, A>::Iterator::operator*() {
    return reinterpret_cast<const T&>(_current_node->_data);
}