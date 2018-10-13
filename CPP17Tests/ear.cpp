
#include "ear.h"
#include "value_guard.h"
#include <iostream>
#include <chrono>

using namespace std;
using namespace ear;

using Bool_Guard = Value_Guard<bool>;

// - Definition of the Allocation Register and its Guard -

// the one and only global variable
Register_Guard Register_Guard::rg;

Register_Guard::Register_Guard()
{
	if (!rg.initialized)
	{
		reg.start();
		rg.initialized = true;
	}
}
Register_Guard::~Register_Guard() {
	if (rg.initialized)
	{
		reg.stop();
		rg.initialized = false;
	}
}

void External_Allocation_Register::start()
{
	// start recording external (de-)allocations
	external_alloc = true;
	external_dealloc = true;
	// message if register is non-empty
	if (!allocated_addresses.empty())
	{
		cout << "WARING: starting with non-empty register." << endl;
		message_allocated_addresses_status();
	}
	// message event
	cout << "register started" << endl;
}
void External_Allocation_Register::stop()
{
	// stop recording external (de-)allocations
	external_alloc = false;
	external_dealloc = false;
	// message event
	cout << "register ended" << endl;
	// check for externaly allocated memory leaks and message them if they exists
	if (!allocated_addresses.empty())
	{
		cout << endl << "---MEMORY LEAK DETECTED---" << endl;
		cout << "the folowing addresses leaked:" << endl;
		message_allocated_addresses_status();
	}
}
void External_Allocation_Register::register_allocated_address(void* ptr)
{
	// prevent self-blocking
	Bool_Guard bga(external_alloc, false);
	Bool_Guard bgd(external_dealloc, false);
	// append new address to register
	allocated_addresses.push_back(ptr);
	// message event
	auto flags = cout.flags();
	cout << "registered address 0x" << hex << ptr << " to allocation register" << endl;
	cout.flags(flags);
}
void External_Allocation_Register::unregister_allocated_address(void* ptr)
{
	// prevent self-blocking
	Bool_Guard bga(external_alloc, false);
	Bool_Guard bgd(external_dealloc, false);
	// remove address from register if possible
	auto it = find(allocated_addresses.begin(), allocated_addresses.end(), ptr);
	auto flags = cout.flags(); // to restore the cout flags
	if (it != allocated_addresses.end())
	{
		allocated_addresses.erase(it);
		// message event
		cout << "unregistered address 0x" << hex << ptr << " from allocation register" << endl;
	}
	else
	{
		// message event
		cout << "can not remove address 0x" << hex << ptr << " from allocation register" << endl;
	}
	cout.flags(flags); // restore cout flags
}
void External_Allocation_Register::message_allocated_addresses_status()
{
	// prevent self-blocking
	Bool_Guard bga(external_alloc, false);
	Bool_Guard bgd(external_dealloc, false);
	// print register
	cout << "---allocated addresses---" << endl;
	auto flags = cout.flags(); // to restore the cout flags
	for (auto ptr : allocated_addresses)
	{
		cout << "0x" << hex << ptr << endl;
	}
	cout << "---end---" << endl << endl;
	cout.flags(flags); // restore cout flags
}
void External_Allocation_Register_With_Random_Bad_Alloc::message_random_bad_alloc_status()
{
	// prevent self-blocking
	Bool_Guard bga(external_alloc, false);
	Bool_Guard bgd(external_dealloc, false);
	//
	if (random_bad_alloc_enabled)
	{
		cout << "random bad allocations are enabled with frequency 1:"
			<< random_bad_alloc_frquency
			<< endl;
	}
	else
	{
		cout << "random bad allocations are disabled" << endl;
	}
}
void External_Allocation_Register_With_Random_Bad_Alloc::message_bad_alloc()
{
	// prevent self-blocking
	Bool_Guard bga(external_alloc, false);
	Bool_Guard bgd(external_dealloc, false);
	// message event
	cout << "allocation failure" << endl;

}
void External_Allocation_Register_With_Random_Bad_Alloc::message_random_bad_alloc()
{
	// prevent self-blocking
	Bool_Guard bga(external_alloc, false);
	Bool_Guard bgd(external_dealloc, false);
	// message event
	cout << "random allocation failure" << endl;
}


// - Definition of overridden memory (de-)allocation functions -

void* operator new(size_t size)
{
	if (Register_Guard::get_register().is_external_allocation())
	{
		if (Register_Guard::get_register().is_random_bad_external_allocation())
		{
			// simulate random allocation error with frequency 1:random_bad_alloc_frquency
			auto r = chrono::high_resolution_clock::now().time_since_epoch().count()
				% Register_Guard::get_register().get_random_bad_external_allocation_frequency();
			if (r == 0) {
				Register_Guard::get_register().message_random_bad_alloc();
				throw bad_alloc();
			}
		}
		// normal external allocation
		void* ptr = malloc(size);
		if (ptr == nullptr)
		{
			Register_Guard::get_register().message_bad_alloc();
			throw bad_alloc();
		}
		// register external allocation
		Register_Guard::get_register().register_allocated_address(ptr);
		// return address to validly allocated memory
		return ptr;
	}
	else
	{
		// normal internal allocation
		void *ptr = malloc(size);
		if (ptr == nullptr)
		{
			throw bad_alloc();
		}
		return ptr;
	}
}
void* operator new[](size_t size)
{
	return operator new(size);
}
void operator delete(void* ptr)
{
	if (Register_Guard::get_register().is_external_deallocation())
	{
		// unregister external deallocation
		Register_Guard::get_register().unregister_allocated_address(ptr);
	}
	// normal ex-/internal deallocation
	free(ptr);
}
void operator delete[](void* ptr)
{
	operator delete(ptr);
}


// - Inspiration for Future Versions -

/* another possible verison of a simple operator new override with aditional arguments
void* operator new (size_t size, const char* filename, int line) {
	void* ptr = new char[size];
	cout << "size = " << size << " filename = " << filename << " line = " << line << endl;
	return ptr;
}
#define tracked_new new(__FILE__, __LINE__)
*/
