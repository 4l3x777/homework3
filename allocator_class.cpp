#include "allocator_class.h"

Pool::Pool(size_t allocation_capasity) {
    if(allocation_capasity == 0) throw std::invalid_argument("allocation capasity is zero");
    _allocation_capasity = allocation_capasity;
}

Pool::Pool(const Pool& other) {
    _allocation_capasity = other._allocation_capasity;
}

Pool::~Pool() {
    for(auto allocation = memory_chunks_pool.begin(); allocation != memory_chunks_pool.end(); ++allocation) {
        (*allocation).clear();
    }
    memory_chunks_pool.clear();
}

void* Pool::allocate(size_t n) {
    while (n > available_memory) {
        for (size_t i = 0; i < _allocation_capasity; i++) {
            memory_chunks_pool.push_back(chunk(n, 0));
            free_chunks_pointers.push((memory_chunks_pool.rbegin())->data());
        }
        available_memory = _allocation_capasity * n;
    }

    available_memory -= n;
    auto free_chunk = free_chunks_pointers.top();
    free_chunks_pointers.pop();
    return reinterpret_cast<void*>(free_chunk);
}

void Pool::deallocate(void* p, size_t n) {     
    //free chunk
    for(auto chunk = memory_chunks_pool.begin(); chunk != memory_chunks_pool.end(); ++chunk) {
        if (reinterpret_cast<void*>((*chunk).data()) == p) {
            free_chunks_pointers.push(chunk->data());
            available_memory += n;
            break;
        }
    }

    //free heap memory if number of free chunks > max_avaliable_free_chunks = allocation_capasity * allocation_capasity_grade        
    auto max_avaliable_free_chunks = _allocation_capasity * _allocation_capasity_grade;
    while (free_chunks_pointers.size() > max_avaliable_free_chunks) {
        auto free_chunk = free_chunks_pointers.top();
        free_chunks_pointers.pop();
        for(auto chunk = memory_chunks_pool.begin(); chunk != memory_chunks_pool.end(); ++chunk) {
            if ((*chunk).data() == free_chunk) {
                memory_chunks_pool.erase(chunk);
                break;
            }
        }
    }
}