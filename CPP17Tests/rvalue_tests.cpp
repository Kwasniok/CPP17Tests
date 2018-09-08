#include "rvalue_tests.h"

using namespace std;
using namespace boost::typeindex;

struct Test {
	int count = 0;
	Test() { cout << "constructor: default (" << count << ")" << endl;};
	Test(const Test&) { cout << "constructor: copy (" << ++count << ")" << endl; };
	Test(Test&&) { cout << "constructor: move (" << ++count << ")" << endl; };
	Test& operator=(const Test&) { cout << "assignment: copy" << endl; return *this; };
	Test& operator=(Test&&) { cout << "assignment: move" << endl; return *this; };
	~Test() { cout << "destructor (" << count << ")" << endl; };
};

Test get_test() { return Test(); };

void rvalue_tests() {

	// http://thbecker.net/articles/rvalue_references/section_05.html
	// Things that are declared as rvalue reference can be lvalues or rvalues.
	// The distinguishing criterion is: if it has a name, then it is an lvalue. Otherwise, it is an rvalue.

	{
		Test&& r = Test();
		cout << type_id_with_cvr<decltype(r)>().pretty_name() << endl;
		Test t = r;
	}
	{
		Test&& r = Test();
		Test t = static_cast<Test&&>(r);
	}
	{
		Test t = get_test();
	}
}

