#include "type_deduction.h"

// This document tests the ideas of item 1 to 4 of "effective modern c++". 

using namespace std;
using namespace boost::typeindex;
void type_deduction() {
	const int ci = 7;

	// item 4: get type names from the compiler
	// set-up variables
	auto x = ci;
	auto y = &ci;

	// via error message
	//show_type(x);
	//show_type(y);
	// via boost
	cout << "item_4.boost: get type names via boost library" << endl;
	cout << "decltype(x) = " << type_id_with_cvr<decltype(x)>().pretty_name() << endl;
	cout << "decltype(y) = " << type_id_with_cvr<decltype(y)>().pretty_name() << endl;
	cout << endl;
}
