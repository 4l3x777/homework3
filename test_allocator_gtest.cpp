#include "gtest/gtest.h"
#include "allocator_class.h"
#include "container.h"
#include <iostream>
#include <list>

using namespace std;

static size_t factorial(size_t n) {
    size_t factorial = 1;
    for (size_t i = 1; i <= n; ++i) factorial *= i;
    return factorial;
}

class TestAllocator : public ::testing::Test {
protected:
	using myallocator = Allocator<pair<const int, int>>;

	void SetUp() {}

	void TearDown() {}

};

TEST_F(TestAllocator, test_custom_container_my_allocator)
{
	std::map<int, int> map_default_allocator;
	std::map<int, int, std::less<int>, myallocator> map_my_allocator; //default allocation_capasity
	for (auto i = 0; i < 10; ++i) {
		map_default_allocator[i] = factorial(i);
		map_my_allocator[i] = factorial(i);
	}

	for (auto i = 0; i < 10; ++i) {
		ASSERT_TRUE(map_default_allocator.at(i) == map_my_allocator.at(i));
	}
}

TEST_F(TestAllocator, test_my_container_my_allocator)
{
	using data_type = pair<int,int>;
    using myallocator = Allocator<pair<const int, int>>;
    using list_type = MyList<data_type, myallocator>;
	
	std::list<data_type> default_list_default_allocator;
	MyList<data_type, myallocator> my_list_my_allocator; //default allocation_capasity
	for (auto i = 0; i < 10; ++i) {
		default_list_default_allocator.push_back(data_type(i, factorial(i)));
		my_list_my_allocator.push_back(data_type(i, factorial(i)));
	}

	auto iter1 = default_list_default_allocator.begin();
	auto iter2 = my_list_my_allocator.begin();
	while(iter1 !=  default_list_default_allocator.end() && iter2 != my_list_my_allocator.end()){
		ASSERT_TRUE((*iter1).first == (*iter2).first);
		ASSERT_TRUE((*iter1).second == (*iter2).second);
		++iter1;
		++iter2;
	}
}

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}