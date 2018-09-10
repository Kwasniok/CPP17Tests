#include "override_tests.h"

void override_tests() {

	{
		Base b;
		b.f();
		b.g();
		b.h();
	}
	{
		Derived d;
		d.f();
		d.g();
		d.h();
	}
	{
		const Derived d;
		d.h();
	}
}
