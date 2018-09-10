#pragma once

#include <iostream>

// A new version is added in its own INLINED namespace BEFORE the old one
inline namespace NEW {
	inline void f() { std::cout << "NEW::f()" << std::endl; };
};

namespace OLD {
	inline void f() { std::cout << "OLD::f()" << std::endl; };
};

void inline_namespace_tests();
