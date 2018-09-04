#include "type_deduction.h"

// This document tests the ideas of item 1 to 4 of "effective modern c++". 

using namespace std;
using namespace boost::typeindex;

// functions for item 1
template<typename T>
constexpr void deduce_type_reference(T& param) {
	cout << "\t              T = " << type_id_with_cvr<T>().pretty_name() << endl;
	cout << "\tdecltype(param) = " << type_id_with_cvr<decltype(param)>().pretty_name() << endl;
};

template<typename T>
constexpr void deduce_type_pointer(T* param) {
	cout << "\t              T = " << type_id_with_cvr<T>().pretty_name() << endl;
	cout << "\tdecltype(param) = " << type_id_with_cvr<decltype(param)>().pretty_name() << endl;
};

template<typename T>
constexpr void deduce_type_universal_reference(T&& param) {
	cout << "\t              T = " << type_id_with_cvr<T>().pretty_name() << endl;
	cout << "\tdecltype(param) = " << type_id_with_cvr<decltype(param)>().pretty_name() << endl;
};

template<typename T>
constexpr void deduce_type_pass_by_value(T param) {
	cout << "\t              T = " << type_id_with_cvr<T>().pretty_name() << endl;
	cout << "\tdecltype(param) = " << type_id_with_cvr<decltype(param)>().pretty_name() << endl;
};

template<typename T, size_t N>
constexpr size_t size_of_array(T(&)[N]) noexcept {
	return N;
}

void type_deduction() {

	// item 1:
	// template type deduction
	int i = 3;
	const int ci = i;
	int& ir = i;
	const int& cir = ci;
	int* ip = &i;
	const int* cip = &ci;
	const char ccstr[5] = "Test";
	void func(int);

	cout << "reminder: In the following paragraph the constness refers allways to the top level constness of the mentioned type. So in this manner `const int*` is non-const!" << endl << endl;
	// case 1a:
	cout << "item_1.case_1a: template type deduction via reference" << endl;
	cout << "summary: T is the const-preserved non-ref type " << endl << 
		    "         & declatype(param) is T&." << endl;
	cout << "int -->" << endl;
	deduce_type_reference(i);
	cout << "const int -->" << endl;
	deduce_type_reference(ci);
	cout << "int& -->" << endl;
	deduce_type_reference(ir);
	cout << "const int& -->" << endl;
	deduce_type_reference(cir);
	cout << "int* -->" << endl;
	deduce_type_reference(ip);
	cout << "const int* -->" << endl;
	deduce_type_reference(cip);
	cout << "const char[] -->" << endl;
	deduce_type_reference(ccstr);
	cout << "void ()(int) -->" << endl;
	deduce_type_reference(func);
	cout << endl;
	// case 1b:
	cout << "item_1.case_1b: template type deduction via pointer" << endl;
	cout << "summary: T is the non-pointer type " << endl <<
		"         & declatype(param) is T*." << endl <<
		"reminder: Pointer types only!;" << endl;
	cout << "int* -->" << endl;
	deduce_type_pointer(ip);
	cout << "const int* -->" << endl;
	deduce_type_pointer(cip);
	cout << "const char[] -->" << endl;
	deduce_type_pointer(ccstr);
	cout << "void ()(int) -->" << endl;
	deduce_type_pointer(func);
	cout << endl << endl;

	// case 2a:
	cout << "item_1.case_2a: template type deduction via universal reference called with lvalues" << endl;
	cout << "summary: T is the const-preserved refernece type " << endl << 
		"         & declatype(param) is T." << endl;
	cout << "lvalue int -->" << endl;
	deduce_type_universal_reference(i);
	cout << "lvalue const int -->" << endl;
	deduce_type_universal_reference(ci);
	cout << "lvalue int& -->" << endl;
	deduce_type_universal_reference(ir);
	cout << "lvalue const int& -->" << endl;
	deduce_type_universal_reference(cir);
	cout << "lvalue int* -->" << endl;
	deduce_type_universal_reference(ip);
	cout << "lvlaue const int* -->" << endl;
	deduce_type_universal_reference(cip);
	cout << "lvalue const char[] -->" << endl;
	deduce_type_universal_reference(ccstr);
	cout << "lvalue void ()(int) -->" << endl;
	deduce_type_universal_reference(func);
	cout << endl;
	// case 2b:
	cout << "item_1.case_2b: template type deduction via universal reference called with rvalues" << endl;
	cout << "summary: T is the const-preserved non-refernece type " << endl << 
	        "         & declatype(param) is T&&." << endl;
	cout << "rvalue int -->" << endl;
	deduce_type_universal_reference(move(i));
	cout << "rvalue const int -->" << endl;
	deduce_type_universal_reference(move(ci));
	cout << "rvalue int& -->" << endl;
	deduce_type_universal_reference(move(ir));
	cout << "rvalue const int& -->" << endl;
	deduce_type_universal_reference(move(cir));
	cout << "rvalue int* -->" << endl;
	deduce_type_universal_reference(move(ip));
	cout << "rvlaue const int* -->" << endl;
	deduce_type_universal_reference(move(cip));
	cout << "rvalue const char[] -->" << endl;
	deduce_type_universal_reference(move(ccstr));
	cout << "rvalue void ()(int) -->" << endl;
	deduce_type_universal_reference(move(func));
	cout << endl << endl;

	// case 3:
	cout << "item_1.case_3: template type deduction via pass by value" << endl;
	cout << "summary: T is the non-const non-refernece type (basic type)" << endl << 
		    "         & declatype(param) is T." << endl << 
		    "remember: int * and const int * are basic types!" << endl;
	cout << "int -->" << endl;
	deduce_type_pass_by_value(i);
	cout << "const int -->" << endl;
	deduce_type_pass_by_value(ci);
	cout << "int& -->" << endl;
	deduce_type_pass_by_value(ir);
	cout << "const int& -->" << endl;
	deduce_type_pass_by_value(cir);
	cout << "int* -->" << endl;
	deduce_type_pass_by_value(ip);
	cout << "const int* -->" << endl;
	deduce_type_pass_by_value(cip);
	cout << "const char[] -->" << endl;
	deduce_type_pass_by_value(ccstr);
	cout << "void ()(int) -->" << endl;
	deduce_type_pass_by_value(func);
	cout << endl << endl;

	// built-in array ref
	int array[3];
	cout << "The array has " << size_of_array(array) << " elements" << endl << endl;


	// item 4:
	// get type names from the compiler
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

void func(int) {}
