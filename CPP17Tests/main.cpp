
#include <iostream>
#include <complex>
using namespace std;

class Base
{
public:
	// add explicit defaults if one is overridden
	Base() = default;
	Base(const Base&) = default;
	Base(Base&&) = default;
	Base& operator=(const Base&) = default;
	Base& operator=(Base&&) = default;
	virtual ~Base() { } // allways add a virtual destructur when using the dynamic types!
	virtual void f( int );
	virtual void f( double );
	virtual void g( int i = 10 );
};

void Base::f( int )
{
	cout << "Base::f(int)" << endl;
}
void Base::f( double )
{
	cout << "Base::f(double)" << endl;
}
void Base::g( int i )
{
	cout << "Base::g() " << i << endl;
}

class Derived : public Base
{
public:
	// add explicit defaults if one is overridden
	Derived() = default;
	Derived(const Derived&) = default;
	Derived(Derived&&) = default;
	Derived& operator=(const Derived&) = default;
	Derived& operator=(Derived&&) = default;
	virtual ~Derived() { } // allways add a virtual destructur when using the dynamic types!
	using Base::f; // or Derived::f will hide Base::f and no overload happens
	void f( complex<double> ); // NO virtual function! avoid overloading a virtual function in the derived class: override only!
	void g( int i = 50 ) override; // use uniform defaults only!!!
};

void Derived::f( complex<double> )
{
	cout << "Derived::f(complex)" << endl;
}
void Derived::g( int i )
{
	cout << "Derived::g() " << i << endl;
}
int main() {
	Base b;
	Derived d;
	unique_ptr<Base> pb = make_unique<Derived>();
	b.f(1.0);
	d.f(1.0);
	pb->f(1.0);
	b.g();
	d.g();
	pb->g();
}
