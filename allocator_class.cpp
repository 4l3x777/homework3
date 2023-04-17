#include "allocator_class.h"

Pool::Pool(size_t type_size, size_t allocation_capasity) {
    if(allocation_capasity == 0) throw std::invalid_argument("allocation capasity is zero");
    _allocation_capasity = allocation_capasity;
    _type_size = type_size;

    //preallocated memory
    for (size_t i = 0; i < _allocation_capasity; i++) {
        auto new_chunk = make_shared<chunk>(_type_size);
        memory_chunks_pool.push_back(new_chunk);
        memory_chunks_free_pool.push(new_chunk);
    }
    available_memory = _allocation_capasity * _type_size;
}

Pool::Pool(const Pool& other) {
    _allocation_capasity = other._allocation_capasity;
    _type_size = other._type_size;
    _allocation_capasity_grade = other._allocation_capasity_grade;

    available_memory = other.available_memory;
    memory_chunks_pool = other.memory_chunks_pool;
    memory_chunks_free_pool = other.memory_chunks_free_pool;
}

Pool::~Pool() {
    for(auto allocation = memory_chunks_pool.begin(); allocation != memory_chunks_pool.end(); ++allocation) {
        (*allocation)->clear();
    }
    memory_chunks_pool.clear();
}

void* Pool::allocate(size_t n) {
    while (available_memory < n) {
        auto new_chunk = make_shared<chunk>(_type_size);
        memory_chunks_pool.push_back(new_chunk);
        memory_chunks_free_pool.push(new_chunk);
        available_memory += _type_size;
    }

    auto current_free_chunk = memory_chunks_free_pool.top();
    memory_chunks_free_pool.pop();  
    available_memory -= current_free_chunk->size();

    while (current_free_chunk->size() < n) {
        auto next_free_chunk = memory_chunks_free_pool.top();
        memory_chunks_free_pool.pop();
        current_free_chunk->resize(current_free_chunk->size() + next_free_chunk->size());
        available_memory -= next_free_chunk->size();
        for(
            auto current_chunk = memory_chunks_pool.begin(); 
            current_chunk != memory_chunks_pool.end(); 
            ++current_chunk
        ) {
            if(*current_chunk == next_free_chunk) {
                memory_chunks_pool.erase(current_chunk);
                break;
            }
        }
    }
    return reinterpret_cast<void*>(current_free_chunk->data());
}

void Pool::deallocate(void* p, size_t n) {     
    //free chunk
    for(auto chunk = memory_chunks_pool.begin(); chunk != memory_chunks_pool.end(); ++chunk) {
        if (reinterpret_cast<void*>((*chunk)->data()) == p) {
            available_memory += n;
            memory_chunks_free_pool.push(*chunk);
            break;
        }
    }

    //free heap memory if number of free available_memory > max_avaliable_free_memory = allocation_capasity * allocation_capasity_grade * type_size    
    auto max_avaliable_free_memory = _allocation_capasity * _allocation_capasity_grade * _type_size;
    while (available_memory > max_avaliable_free_memory) {
        auto free_chunk = memory_chunks_free_pool.top();
        memory_chunks_free_pool.pop();
        available_memory -= free_chunk->size();
        for(
            auto current_chunk = memory_chunks_pool.begin(); 
            current_chunk != memory_chunks_pool.end(); 
            ++current_chunk
        ) {
            if(*current_chunk == free_chunk) {
                memory_chunks_pool.erase(current_chunk);
                break;
            }
        }
    }
}