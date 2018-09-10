#pragma once

#include <iostream>

struct Base {
	virtual void f() { std::cout << "Base::f" << std::endl; };
	virtual void g() { std::cout << "Base::g" << std::endl; };
};

// allways add 'override' to override or 'final' to seal
struct Derived : public Base {
	virtual void f() override { std::cout << "Derived::f" << std::endl; };
	virtual void g() final { std::cout << "Derived::g" << std::endl; };
};

void override_tests();
