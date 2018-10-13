
#pragma once

#include <vector>

// GENERAL NOTE:
// The following definitions are used:
// external allocation: An allocation which is initiated by the programm / the user (within the main loop)
// internal allocation: An allocation which is initiated by the address registering mechanism itself.
//                      These allocation shall NOT be registered since they are not of interest for the user
//                      and registering them might lead to unbreakable allocation loops.
// NOTE: GLobal variables and concurrency are NOT supported!


// - Declaration of overridden memory (de-)allocation functions -

// These memory (de-)allocation functions will be overridenn to implement the allocation register:
// NOTE: They must be defined in global scope or they will only be called within code of the same namespace.
void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void*);
void  operator delete[](void*);


// - Declaration of the Allocation Register and its Guard -

// ear : external allocation register
// NOTE: This is NOT THREAD-SAFE!
namespace ear
{
	// class to control the lifetime of the register and enforce singleton behaviour
	class Register_Guard;

	// Holds a register of allocated pointers and messages information about them.
	// NOTE: This class life time is thightly coupled with Register_Guard. Hence it can only be contructed or
	//       destructed from Register_Guard.
	// NOTE: (De-)Registering of addresses should only occurr from within a memory (de-)allocation function.
	//       This is allready the case for the functions mentioned above.
	class External_Allocation_Register
	{
	protected: // prevent user access to creation or destruction of a register
		External_Allocation_Register() { }
		friend Register_Guard;
		~External_Allocation_Register() { }
		friend Register_Guard;
	private:
		// prepares the register and messages about it
		void start();
		// checks the register for memory leaks and messages about it
		void stop();
	public:
		External_Allocation_Register(const External_Allocation_Register&) = delete; // uncopyable
		External_Allocation_Register(External_Allocation_Register&&) = delete; // unmovable
		External_Allocation_Register& operator=(const External_Allocation_Register&) = delete; // uncopyable
		External_Allocation_Register& operator=(External_Allocation_Register&&) = delete; // unmovable
		// signals a memory allocation function if external allocations are currently allowed
		// NOTE: Internal allocations MUST NOT be registered or a non halting loop might occur.
		bool is_external_allocation() { return external_alloc; }
		// signals a memory deallocation function if external deallocations are currently allowed
		// NOTE: Internal deallocations MUST NOT be unregistered since they will never be registered.
		bool is_external_deallocation() { return external_dealloc; }
		// register an external allocation
		void register_allocated_address(void* ptr);
		// unregister an external allocation (i.e. an external deallocation)
		void unregister_allocated_address(void* ptr);
		// display information about all registered external allocations
		void message_allocated_addresses_status();
	private:
		// holds all externaly allocated pointers
		std::vector<void*> allocated_addresses;
	protected: // protected is necessary due to lock guards (see implementation)
		// tracks weather a allocation should be registered to prevent from self-blocking
		bool external_alloc = false;
		// tracks weather a de allocation should be registered to prevent from self-blocking
		bool external_dealloc = false;
	};

	// Register with extended functionality: Simulates bad allocations based on a random chance.
	class External_Allocation_Register_With_Random_Bad_Alloc: public External_Allocation_Register
	{
	public: // extended functionality:
		void enable_random_bad_external_allocation() { random_bad_alloc_enabled = true; }
		void disable_random_bad_external_allocation() { random_bad_alloc_enabled = false; }
		void toggle_random_bad_external_allocation() { random_bad_alloc_enabled = !random_bad_alloc_enabled; }
		// signals memory allocation functions wheather random bad allocations are currently allowed
		bool is_random_bad_external_allocation() { return random_bad_alloc_enabled; }
		// The real world chance to get a random bad allocation is approximitly 1:f .
		// (One in f allocations fails).
		void set_random_bad_external_allocation_frequency(int f) { random_bad_alloc_frquency = f; }
		// The real world chance to get a random bad allocation is approximitly 1:f .
		// (One in f allocations fails).
		int get_random_bad_external_allocation_frequency() { return random_bad_alloc_frquency; }
		// displays the current status of the random bad allocation behaviour
		void message_random_bad_alloc_status();
		// used to inform the user about a REAL bad allocation within a allocation function
		void message_bad_alloc();
		// used to inform the user about a FAKED bad allocation within a allocation function
		void message_random_bad_alloc();
	private:
		// represents the activation status of the random bad external allocation mechanism
		bool random_bad_alloc_enabled = false;
		// defines the random chance for a faked bad allocation for external allocations
		// The real world frequency is approximatly 1:random_bad_alloc_frquency.
		// (One in random_bad_alloc_frquency allocations fails).
		int random_bad_alloc_frquency = 1;
	};

	// I.  start/stop register on regular program entry/exit (RAII semantics)
	//     This is implemented via the constructor and destrcutor of a static variable.
	// II. control lifetime of global variables
	//     This is implemented via a member variable.
	// NOTE: Singleton behaviour of the register is enforced!
	class Register_Guard
	{
	public:
		// called before main() 'entry hook'
		Register_Guard();
		// called after main() 'exit hook'
		~Register_Guard();
		// returns a reference to the allocation register
		static External_Allocation_Register_With_Random_Bad_Alloc& get_register() { return rg.reg; }
	public:
		// the only global variable
		// its life time controlls the life time of the register
		static Register_Guard rg;
	private:
		// is true if a register exists (used to enfore singleton behaviour)
		bool initialized = false;
		// This guard controls the lifetime of the following register:
		// NOTE: The register must be coupled to the life time of this register guard or it might be (randomly) destroyed
		//       before this guard is destroyed which leads to undefined behaviour.
		External_Allocation_Register_With_Random_Bad_Alloc reg;
	};
}
