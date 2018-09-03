#include "type_deduction.h"

void type_deduction() {
	const int ci = 7;

	auto x = ci;
	auto y = &ci;

	//show_type(x);
	//show_type(y);
}
