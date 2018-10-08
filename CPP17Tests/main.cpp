
#include <iostream>

#define do_verbose(expr) cout << #expr " --> "; expr; cout << endl;

using namespace std;

struct Base
{
	void f(int) { cout << "Base::f(int)"; }
};

struct Derived_without_using: public Base
{
	void f(double) { cout << "Derived_without_using::f(double)"; }
};

struct Derived_with_using: public Base
{
	using Base::f;
	void f(double) { cout << "Derived_with_using::f(double)"; }
};

int main()
{
	{
		do_verbose(Derived_without_using d);
		do_verbose(d.f(1));
		do_verbose(d.f(1.0));
	}
	{
		do_verbose(Derived_with_using d);
		do_verbose(d.f(1));
		do_verbose(d.f(1.0));
	}
}
