
#include <iostream>

// task:
// class template based on a type T
// T must provide a specific member function (signatue given, not a special function i.e. constructor/destructor)
// make sure T provides this function
// signature: T* T::clone() const

struct Clonable
{
	Clonable* clone() const { return nullptr; }
};
struct NonclonableA
{

};
struct NonclonableB
{
	NonclonableB* clone() { return nullptr; }
};
struct NonclonableC
{
	void* clone() const { return nullptr; }
};

// strategy 1:
// call the desired function in the constructor
// optional: create an assert to minimize overhead
template<typename T>
struct C
{
	~C()
	{
		const T test; // needs default constructor T::T()
		T* x = test.clone(); // must be a valid call
	};
};
// strategy 1b:
// same but with nested class
// FAILS because inner class is never instanciated
template<typename T>
struct D
{
	class D2
	{
		~D2()
		{
			const T test; // needs default constructor T::T()
			T* x = test.clone(); // must be a valid call
		};
	};
};

// strategy 2:
template<typename T>
struct E
{
	static void requirements()
	{
		T* (T::*test)() const = &T::clone;
		test;
	}
	~E()
	{
		void(*x)() = &requirements;
	}
};

int main()
{
	{
		C<Clonable> c;
		//C<NonclonableA> ya; // desired error
		//C<NonclonableB> yb; // desired error
		//C<NonclonableC> yc; // desired error
	}

	{
		D<Clonable> c;
		//D<NonclonableA> ya; // undesirably compiles!
		//D<NonclonableB> yb; // undesirably compiles!
		//D<NonclonableC> yc; // undesirably compiles!
	}

	{
		E<Clonable> c;
		//E<NonclonableA> ya; // desired error
		//E<NonclonableB> yb; // desired error
		//E<NonclonableC> yc; // desired error
	}
}
