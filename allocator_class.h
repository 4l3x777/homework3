#pragma once
#include <iostream>
#include <memory>
#include <map>
#include <list>
#include <vector>
#include <stack>

static constexpr size_t DEFAULT_ALLOCATION_CAPASITY = 10;

using namespace std;

class Pool {
public:
    using chunk = vector<uint8_t>;
private:
    list<shared_ptr<chunk>> memory_chunks_pool;
    size_t available_memory {0};
    stack<shared_ptr<chunk>> memory_chunks_free_pool;
public:
    size_t _type_size{0};
    size_t _allocation_capasity {0};
    size_t _allocation_capasity_grade {3};
    
    Pool(size_t type_size, size_t allocation_capasity = DEFAULT_ALLOCATION_CAPASITY);
    Pool(const Pool& other);
    ~Pool();
    void* allocate(size_t n);
    void deallocate(void* p, size_t n);
};

template <class T>
struct Allocator {
    using value_type = T;
    using Traits = std::allocator_traits<Allocator<T>>;
#if !defined _MSC_VER    
    using size_type = typename std::allocator<T>::size_type;
    using difference_type = typename std::allocator<T>::difference_type;
    using pointer = typename std::allocator<T>::pointer;
    using const_pointer = typename std::allocator<T>::const_pointer;
    using reference = typename std::allocator<T>::reference;
    using const_reference = typename std::allocator<T>::const_reference;
#endif

    Allocator(size_t allocation_capasity = DEFAULT_ALLOCATION_CAPASITY) {
       _Pool = make_shared<Pool>(sizeof(T), allocation_capasity);
    }

    ~Allocator() {
        _Pool.reset();
    }

    explicit Allocator(Pool& pool) : _Pool(&pool) {}
    template<class U> Allocator(const Allocator<U>& other) : _Pool(other._Pool) {}

    T* allocate(size_t n) { return (T*)_Pool->allocate(n * sizeof(T)); }
    void deallocate(T* p, size_t n) { _Pool->deallocate(p, n * sizeof(T)); }

    template<class U, class... Args> void construct(U* p, Args&&... args) { std::allocator<T>().construct(p, std::forward<Args>(args)...); }
    template<class U> void destroy(U* p) { std::allocator<T>().destroy(p); }
    template<class U> struct rebind { using other = Allocator<U>; };
 
    shared_ptr<Pool> _Pool { nullptr };
};

template<class T, class U> bool operator==(const Allocator<T>& lhs, const Allocator<U>& rhs) { return lhs._Pool == rhs._Pool; }
template<class T, class U> bool operator!=(const Allocator<T>& lhs, const Allocator<U>& rhs) { return !(lhs == rhs); }