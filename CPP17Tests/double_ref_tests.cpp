#include "double_ref_tests.h"

using namespace std;
using namespace boost::typeindex;

void double_ref_tests() {

	// In type deduction:
	// http://thbecker.net/articles/rvalue_references/section_08.html
	//A& & becomes A&
	//A& && becomes A&
	//A&& & becomes A&
	//A&& && becomes A&&


	int i;
	{
		add_ref<int&> irr(i);
		cout << "int & & --> " << type_id_with_cvr<decltype(irr)>().pretty_name() << endl;
	}
	{
		add_refref<int&> irr(i);
		cout << "int & && --> " << type_id_with_cvr<decltype(irr)>().pretty_name() << endl;
	}
	{
		add_ref<int&&> irr(i);
		cout << "int && & --> " << type_id_with_cvr<decltype(irr)>().pretty_name() << endl;
	}
	{
		add_refref<int&&> irr(move(i));
		cout << "int && && --> " << type_id_with_cvr<decltype(irr)>().pretty_name() << endl;
	}

}
