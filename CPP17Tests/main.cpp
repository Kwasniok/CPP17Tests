
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
	class Register_Guard;

	//
	class External_Allocation_Register
	{
	protected:
		// prevent user access to creation of a register
		External_Allocation_Register() { }
		friend Register_Guard;
		~External_Allocation_Register() { }
		friend Register_Guard;
	public:
		External_Allocation_Register(const External_Allocation_Register&) = delete; // uncopyable
		External_Allocation_Register(External_Allocation_Register&&) = delete; // unmovable
		External_Allocation_Register& operator=(const External_Allocation_Register&) = delete; // uncopyable
		External_Allocation_Register& operator=(External_Allocation_Register&&) = delete; // unmovable
		void on_regular_program_entry();
		void on_regular_program_exit();
		bool do_external_allocation() { return external_alloc; }
		bool do_external_deallocation() { return external_dealloc; }
		void register_allocated_address(void* ptr);
		void unregister_allocated_address(void* ptr);
		void message_allocated_addresses_status();
	private:
		// holds all externaly allocated pointers
		vector<void*> allocated_addresses;
	protected: // protected is necessary due to lock guards (See below)
		// tracks weather a bad allocation should be simulated to prevent from self-blocking
		bool external_alloc = false;
		// tracks weather a bad allocation should be simulated to prevent from self-blocking
		bool external_dealloc = false;
	};

	//
	class External_Allocation_Register_With_Random_Bad_Alloc : public External_Allocation_Register
	{
	public:
		// extended functionality:
		void enable_random_bad_external_allocation() { random_bad_alloc_enabled = true; }
		void disable_random_bad_external_allocation() { random_bad_alloc_enabled = false; }
		void toggle_random_bad_external_allocation() { random_bad_alloc_enabled = !random_bad_alloc_enabled; }
		bool do_random_bad_external_allocation() { return random_bad_alloc_enabled; }
		void set_random_bad_external_allocation_frequency(int f) { random_bad_alloc_frquency = f; }
		int get_random_bad_external_allocation_frequency() { return random_bad_alloc_frquency; }
		void message_random_bad_alloc_status();
		void message_bad_alloc();
		void message_random_bad_alloc();
	private:
		// represents the activation status of the random bad external allocation mechanism
		bool random_bad_alloc_enabled = false;
		// defines the random chance for a faked bad allocation for external allocations
		// the real world frequency is approximatly 1:random_bad_alloc_frquency
		int random_bad_alloc_frquency = 1;
	};

	// I.  check register on regular program entry and exit (RAII semantics)
	// II. control lifetime of global variables
	// NOTE: Singleton behaviour is enforced!
	class Register_Guard
	{
	public:
		// the only global variable
		// its life time controlls the life time of the register
		static Register_Guard rg;
		Register_Guard();
		~Register_Guard();
		External_Allocation_Register_With_Random_Bad_Alloc& get_register() { return reg; }
	private:
		bool initialized = false;
		// controlls the lifetime of the following status variable sets
		// NOTE: They must be coupled to the life time of the register guard or they might be (randomly) destroyed
		//       before this guard is destroyed which leads to undefined behaviour.
		External_Allocation_Register_With_Random_Bad_Alloc reg;
	};
	// the one and only global variable
	Register_Guard Register_Guard::rg;

	Register_Guard::Register_Guard()
	{
		if (!rg.initialized)
		{
			reg.on_regular_program_entry();
			rg.initialized = true;
		}
	}
	Register_Guard::~Register_Guard() {
		if (rg.initialized)
		{
			reg.on_regular_program_exit();
			rg.initialized = false;
		}
	}

	void External_Allocation_Register::on_regular_program_entry()
	{
		// start recording external (de-)allocations
		external_alloc = true;
		external_dealloc = true;
		// message event
		cout << "register started" << endl;
	}
	void External_Allocation_Register::on_regular_program_exit()
	{
		// stop recording external (de-)allocations
		external_alloc = false;
		external_dealloc = false;
		// message event
		cout << "register ended" << endl;
		// check for externaly allocated memory leaks and print them if they exists
		if (!Register_Guard::rg.get_register().allocated_addresses.empty())
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
		Register_Guard::rg.get_register().allocated_addresses.push_back(ptr);
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
}

void* operator new(size_t size)
{
	if (hidden::Register_Guard::rg.get_register().do_external_allocation())
	{
		if (hidden::Register_Guard::rg.get_register().do_random_bad_external_allocation())
		{
			// simulate random allocation error with frequency 1:random_bad_alloc_frquency
			auto r = chrono::high_resolution_clock::now().time_since_epoch().count()
				     % hidden::Register_Guard::rg.get_register().get_random_bad_external_allocation_frequency();
			if (r == 0) {
				hidden::Register_Guard::rg.get_register().message_random_bad_alloc();
				throw bad_alloc();
			}
		}
		// normal external allocation
		void* ptr = malloc(size);
		if (ptr == nullptr)
		{
			hidden::Register_Guard::rg.get_register().message_bad_alloc();
			throw bad_alloc();
		}
		// register external allocation
		hidden::Register_Guard::rg.get_register().register_allocated_address(ptr);
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
	if (hidden::Register_Guard::rg.get_register().do_external_deallocation())
	{
		// unregister external deallocation
		hidden::Register_Guard::rg.get_register().unregister_allocated_address(ptr);
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
		hidden::message_allocated_addresses_status();
		hidden::unregister_allocated_address(ptr);
		hidden::message_allocated_addresses_status();
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
		auto& reg = hidden::Register_Guard::rg.get_register();
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
