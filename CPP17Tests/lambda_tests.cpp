#include "lambda_tests.h";

void lambda_tests() {

	// empty lambda (function type: void())
	auto l0 = [] {};
	// (function type: int(int))
	auto l1 = [](int i) { return i; };
	// same as above but with suffix return type (function type: int(int))
	auto l2 = [](int i) -> int { return i; };

	// with capture
	int i = 5;
	int k = 0;
	// by reference
	auto l3 = [&i] { ++i; };
	// by value
	auto l4 = [i] { return i + 1; };
	// mixed
	auto l5 = [i, &k] { return i + (++k); };
	// all ref
	auto l6 = [&] { return (++i) + (++k); };
	// all by value
	auto l7 = [=] { return i + k; };
}
