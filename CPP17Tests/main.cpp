
#include <iostream>

#define do_verbose(expr) cout << #expr " --> "; expr; cout << endl;

using namespace std;


struct Base
{
	void f(int) { cout << "Base::f(int)"; }
};
// case 1: public inheritance
struct Derived_public_without_using: public Base
{
	void f(double) { cout << "Derived_public_without_using::f(double)"; }
};
struct Derived_public_with_using: public Base
{
	using Base::f;
	void f(double) { cout << "Derived_public_with_using::f(double)"; }
};
// case 2: private inheritance
struct Derived_private_without_using: private Base
{
	void f(double) { cout << "Derived_private_without_using::f(double)"; }
};
struct Derived_private_with_using: private Base
{
	using Base::f;
	void f(double) { cout << "Derived_private_with_using::f(double)"; }
};

int main()
{
	{
		do_verbose(Derived_public_without_using d);
		do_verbose(d.f(1));
		do_verbose(d.f(1.0));
	}
	cout << endl;
	{
		do_verbose(Derived_public_with_using d);
		do_verbose(d.f(1));
		do_verbose(d.f(1.0));
	}
	cout << endl << endl;
	{
		do_verbose(Derived_private_without_using d);
		do_verbose(d.f(1));
		do_verbose(d.f(1.0));
	}
	cout << endl;
	{
		do_verbose(Derived_private_with_using d);
		do_verbose(d.f(1));
		do_verbose(d.f(1.0));
	}
	cout << endl;
}
