
#include <iostream>
#include "ear.h"

using namespace std;
using namespace ear;

// - Tests -

int main()
{
	// short test for direct interaction with 'ear' module
	/*
	{
		auto ptr = reinterpret_cast<void*>(12345);
		ear::register_allocated_address(ptr);
		ear::message_allocated_addresses_status();
		ear::unregister_allocated_address(ptr);
		ear::message_allocated_addresses_status();
		ear::unregister_allocated_address(ptr);
	}
	*/

	//  short test for real world interaction with 'ear' module
	/*
	{
		int* ip = new int(0);
		int* ip2 = new int(0);
		delete ip;
	} // whoops! ip2 leaked!
	*/

	// short program to test the accuracy of the real frequency
	/*
	{
		int c = 0;
		int reps = 1000000;
		for (int i = 0; i < reps; ++i)
		{
			auto r = chrono::high_resolution_clock::now().time_since_epoch().count() % random_bad_alloc_frquency;
			if (r == 0) ++c;
		}
		cout << "real frequency: " << double(c) / reps << endl;
	}
	*/

	// real world example test with random bad allocations
	{
		auto& reg = ear::Register_Guard::get_register();
		reg.set_random_bad_external_allocation_frequency(3);
		reg.enable_random_bad_external_allocation();
		reg.message_random_bad_alloc_status();
		try
		{
			auto ip = make_unique<int>(3);
			std::vector<int> v{1,2,3};
			int* ipta = new int[5];
		} catch (bad_alloc e)
		{
			cout << "main(): bad_alloc catched" << endl;
		}
	}
}
