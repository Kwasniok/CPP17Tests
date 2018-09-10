#include "override_tests.h"

void override_tests() {

	{
		Base b;
		b.f();
		b.g();
	}
	{
		Derived d;
		d.f();
		d.g();
	}
}
