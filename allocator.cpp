#include <iostream>
#include "allocator_class.h"
#include "container.h"

template<typename T>
static void print_result(const T& container) {
    for (auto pair : container) cout << pair.first << " " << pair.second << endl;
}

static size_t factorial(size_t n) {
    size_t factorial = 1;
    for (size_t i = 1; i <= n; ++i) factorial *= i;
    return factorial;
}

static void map_standard_allocator() {
    using map_type = std::map<int, int>;
    map_type map;
    for (int i = 0; i < 10; ++i) map[i] = factorial(i);
    print_result(map);
}

static void map_my_allocator() {  
    using myallocator = Allocator<pair<const int, int>>;
    using map_type = std::map<int, int, std::less<int>, myallocator>;
    //map_type map; //default allocation capasity
    
    //test non-defailt capasity
    auto alloc_manager = myallocator(3); // (instance for easy debuging) allocation capasity is 3
    map_type map(alloc_manager); // allocation capasity is 3

    for (int i = 0; i < 10; ++i) {
        map[i] = factorial(i);
    }

    print_result(map);
}

static void mylist_standard_allocator()
{
    using data_type = pair<int,int>;
    using list_type = MyList<data_type>;
    list_type list;
    for (int i = 0; i < 10; ++i) {
        list.push_back(data_type(i,factorial(i)));
    }
    print_result(list);
}

static void mylist_my_allocator()
{
    using data_type = pair<int,int>;
    using myallocator = Allocator<pair<const int, int>>;
    using list_type = MyList<data_type, myallocator>;
    //list_type list; //default allocation capasity

    //test non-defailt capasity
    auto alloc_manager = myallocator(3); // (instance for easy debuging) allocation capasity is 3
    list_type list(alloc_manager); // allocation capasity is 3
    for (int i = 0; i < 10; ++i) {
        list.push_back(data_type(i,factorial(i)));
    }
    print_result(list);

    // Check const_iterator & iterator concepts
    //
    //for(auto it = list.begin(); it != list.end(); ++it) {
    //    *it = data_type(5,5); // good change value
    //}

    //for(auto it = list.cbegin(); it != list.cend(); ++it) {
    //    *it = data_type(5,5); // compile error const != non_const
    //}
}

int main(int argc, char *argv[]) {
    // Check allocate block of memory
    //
    //Allocator<int> allocator;
    //for (size_t i = 1; i < 3; ++i) {
    //    int* ints = allocator.allocate(i);
    //    std::fill(ints, ints + i, 0);
    //}
    
    try {
        map_my_allocator();
        map_standard_allocator();
        mylist_standard_allocator();
        mylist_my_allocator();
    }
    catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
