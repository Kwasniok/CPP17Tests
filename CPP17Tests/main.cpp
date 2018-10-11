
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

// holds a variable of type T to set_value during the lifetime and restores the original value when destructed (RAII semantics)
// NOTE: The RAII semantics implicitly require a owning relationship of the guarded variable!
//       (I.e. The passed variable MUST BECOME READ-ONLY to the rest of the programm during the life time of this guard!)
template<typename T>
class Value_Guard
{
	T reset_value;
	T& target;
public:
	Value_Guard(T& v, T set_value) : target(v), reset_value(v) { target = set_value; }
	~Value_Guard() { target = reset_value; }
};

using Bool_Guard = Value_Guard<bool>;

// NOTE: Do NOT maipulate elements of the hidden namespace directly!
// NOTE: This is NOT THREAD-SAFE! And does NOT SUPPORT MULTIPLE MODULES (cpp files)!
namespace hidden
{
	// track weather a bad allocation should be simulated to prevent from self-blocking
	bool external_alloc = true;
	// track weather a bad allocation should be simulated to prevent from self-blocking
	bool external_dealloc = true;
	// deactivates external de-/allocation during the lifetime of scope the macro is called inside
	// and restores the initial state afterwards
#define hidden_create_lock_guards() Bool_Guard bga(external_alloc, false); Bool_Guard bgd(external_dealloc, false);

	// defines the random chance for a faked bad allocation for external allocations
	// the real world frequency is approximatly 1:random_bad_alloc_frquency
	int random_bad_alloc_frquency = 2;
	bool random_bad_alloc_enabled = true;

	void print_random_bad_alloc_status()
	{
		// prevent self-blocking
		hidden_create_lock_guards();
		//
		if (random_bad_alloc_enabled)
		{
			cout << "random bad allocations are enabled with frequency 1:" << random_bad_alloc_frquency << endl;
		}
		else
		{
			cout << "random bad allocations are disabled" << endl;
		}
	}

	vector<void*>* allocated_addresses = nullptr; // leaks on purpose!
	void message_random_bad_alloc()
	{
		// prevent self-blocking
		hidden_create_lock_guards();
		// message event
		cout << "random allocation failure" << endl;
	}
	void message_bad_alloc()
	{
		// prevent self-blocking
		hidden_create_lock_guards();
		// message event
		cout << "allocation failure" << endl;

	}
	void register_allocated_address(void* ptr)
	{
		// prevent self-blocking
		hidden_create_lock_guards();
		// create register if needed
		if (allocated_addresses == nullptr)
		{
			allocated_addresses = new vector<void*>();
		}
		// append new address to register
		allocated_addresses->push_back(ptr);
		// message event
		auto flags = cout.flags();
		cout << "registered address 0x" << hex << ptr << " to allocation register" << endl;
		cout.flags(flags);
	}
	void unregister_allocated_address(void* ptr)
	{
		// prevent self-blocking
		hidden_create_lock_guards();
		// create register if needed
		if (allocated_addresses == nullptr)
		{
			allocated_addresses = new vector<void*>();
		}
		// remove address from register if possible
		auto it = find(allocated_addresses->begin(), allocated_addresses->end(), ptr);
		auto flags = cout.flags(); // to restore the cout flags
		if (it != allocated_addresses->end())
		{
			allocated_addresses->erase(it);
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
	void print_allocated_addresses()
	{
		// prevent self-blocking
		hidden_create_lock_guards();
		// print register
		cout << "---allocated addresses---" << endl;
		auto flags = cout.flags(); // to restore the cout flags
		for (auto ptr : *allocated_addresses)
		{
			cout << "0x" << hex << ptr << endl;
		}
		cout << "---end---" << endl << endl;
		cout.flags(flags); // restore cout flags
	}
	void on_regular_program_entry()
	{
		// prevent self-blocking
		hidden_create_lock_guards();
		// message event
		cout << "register started" << endl;
	}
	void on_regular_program_exit()
	{
		// prevent self-blocking
		hidden_create_lock_guards();
		// message event
		cout << "register ended" << endl;
		// check for externaly allocated memory leaks and print them if they exists
		if (!allocated_addresses->empty())
		{
			cout << endl << "---MEMORY LEAK DETECTED---" << endl;
			cout << "the folowing addresses leaked:" << endl;
			print_allocated_addresses();
		}
	}

	// check register on regular program entry and exit (RAII semantics)
	// NOTE: Singleton behaviour is enforced!
	struct Register_Guard
	{
		// the one and only instance which matters
		static Register_Guard rg;
		bool initialized = false;
		Register_Guard()
		{
			if (!rg.initialized)
			{
				on_regular_program_entry();
				rg.initialized = true;
			}
		}
		~Register_Guard() {
			if (rg.initialized)
			{
				on_regular_program_exit();
				rg.initialized = false;
			}
		}
	};
	// the one and only instance which matters
	Register_Guard Register_Guard::rg;
}

void* operator new(size_t size)
{
	if (hidden::external_alloc)
	{
		if (hidden::random_bad_alloc_enabled)
		{
			// simulate random allocation error with frequency 1:random_bad_alloc_frquency
			auto r = chrono::high_resolution_clock::now().time_since_epoch().count() % hidden::random_bad_alloc_frquency;
			if (r == 0) {
				hidden::message_random_bad_alloc();
				throw bad_alloc();
			}
		}
		// normal external allocation
		void* ptr = malloc(size);
		if (ptr == nullptr)
		{
			hidden::message_bad_alloc();
			throw bad_alloc();
		}
		// register external allocation
		hidden::register_allocated_address(ptr);
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
	if (hidden::external_dealloc)
	{
		// unregister external deallocation
		hidden::unregister_allocated_address(ptr);
	}
	// normal ex-/internal deallocation
	free(ptr);
}
void operator delete[](void* ptr)
{
	operator delete(ptr);
}

/* another possible verison of a simple operator new override with aditional arguments
void* operator new (size_t size, const char* filename, int line) {
	void* ptr = new char[size];
	cout << "size = " << size << " filename = " << filename << " line = " << line << endl;
	return ptr;
}
#define tracked_new new(__FILE__, __LINE__)
*/

int main()
{
	// short test for direct interaction with 'hidden' module
	/*
	{
		auto ptr = reinterpret_cast<void*>(12345);
		hidden::register_allocated_address(ptr);
		hidden::print_allocated_addresses();
		hidden::unregister_allocated_address(ptr);
		hidden::print_allocated_addresses();
		hidden::unregister_allocated_address(ptr);
	}
	*/

	//  short test for real world interaction with 'hidden' module
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

	// real world example test with random
	{
		hidden::print_random_bad_alloc_status();
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
