
#include <iostream>

// Consider various conversion strategies for two types which should be convertible into each other.
// Strategy 1: (implititly or explicitly) converting copy constructors
/*
	struct Y;
	struct X
	{
		/ *explicit* / X(const Y&);
	};
	struct Y
	{
		/ *explicit* / Y(const X&);
	};
*/
// Strategy 2: (implititly or explicitly) converting operators (impossible!)
/*
	struct Y;
	struct X
	{
		/ *explicit* /operator Y() { };  // Requires full declaration of Y!
	};
	struct Y
	{
		/ *explicit* /operator X() { };
	};
*/
// Strategy 3: implicitly converting copy assignments
/*
	struct Y;
	struct X
	{
		X& operator=(const Y&);
	};
	struct Y
	{
		Y& operator=(const X&);
	};
*/
// Strategy 4: mixed approach (aka. maximal conversion functions, use with care!)
struct Y;
struct X
{
	X() { };
	X(const X&) { }; // declaring explicit is not useful (*)
	X& operator=(const X&) { return *this; };
	X& operator=(const Y&) { return *this; }; // optional for optimizations
	/*explicit*/ X(const Y&) { };
	// /*explicit*/ operator Y() { return Y(); } // optional, not possible
};
struct Y
{
	Y() { };
	Y(const Y&) { }; // declaring explicit is not useful (*)
	Y& operator=(const Y&) { return *this; };
	Y& operator=(const X&) { return *this; }; // optional for optimizations
	/*explicit*/ Y(const X&) { };
	/*explicit*/ operator X() { return X(); } // optional, An explicit copy constructor would cause an error here! (*)
};

// NOTE: Some methods are optional and even the methods not marked as optional can be replaced by other
//       in SOME calling scenarios.
// NOTE: When implementing multiple methods check for surprising inconsistencies!
// NOTE: Each method has a calling scenario were it is preferred over the other methods (if it is not declared explicit).
//       (for details see tests in main)
//       [explicit] Y(const X) dominates all EXPLICIT conversions:
//           Y y; X x(y);
//           Y y; X x; x = (X) y;
//           Y y; X x; x = X(y);
//           Y y; X x; x = static_cast<X>(y);
//       Y::operator X() dominates the IMPLICITLY converting copy constructor:
//           Y y; X x = y;
//       Y& operator=(const X&) dominates the IMPLICITLY converting copy assignment:
//           Y y; X x; x = y;

int main()
{
	// instances to convert from
	X x;
	Y y;

	// EXPLICIT conversion (converts as long as at least one conversion method is defined)
	{
		// conversion via copy constructor
		{
			// differs for each scenario:
			// A: X::X(const Y&) is declared
			//    calls copy constructor of left type from right type
			//    --> X::X(const Y&)
			// B: X::X(const Y&) not declared but Y::operator X() is declared
			//    calls converting operator of right type to left type
			//    --> Y::operator X()
			X x2(y);
			// A: Y::Y(const X&) is declared
			//    calls copy constructor of left type from right type
			//    --> Y::Y(const X&)
			// B: not possible
			Y y2(x);
		}
		// conversion via copy assignment: converts to left type via copy constructor first and then calls copy assignment for left type
		{
			// The following three notations are interchangable (in chronological order):
			// C-style cast
			{
				// differs for each scenario:
				// A: X::X(const Y&) is declared
				//    calls copy constructor of left type from right type
				//    --> X::X(const Y&), X::operator=(constX &)
				// B: X::X(const Y&) not declared but Y::operator X() is declared
				//    calls converting operator of right type to left type
				//    --> Y::operator X(), X::operator=(constX &)
				X x2; x2 = (X) y;
				// A: Y::Y(const X&) is declared
				//    calls copy constructor of left type from right type
				//    --> Y::Y(const X&), Y::operator=(constY &)
				// B: not possible
				Y y2; y2 = (Y) x;
			}
			// C++-style cast
			{
				X x2; x2 = X(y);
				Y y2; y2 = Y(x);
			}
			// C++-style 'verbose' cast
			{
				X x2; x2 = static_cast<X>(y);
				Y y2; y2 = static_cast<Y>(x);
			}
		}
	}
	// IMPLICIT conversion (converts as long as at least one non-explicit conversion method is defined)
	{
		// conversion via copy constructor with assignment syntax
		{
			// differs for each scenario:
			// A: Y::operator X() is non-explicit (and X::X(const Y&) might not be declared)
			//    converts to left type first and then calls copy constructor of left type from left type
			//    --> Y::operator X(), X::X(const X&)
			// B: Y::operator X() is explicit or not declared and X::X(const Y&) is non-explicit
			//    calls copy constructor of left type from right type
			//    --> X::X(const Y&)
			// C: Y::operator X() and X::X(const Y&) are both explicit or not declared
			//    does not compile
			X x2 = y;
			// A: not possible since X::operator Y() does not exsist
			// B: Y::Y(const X&) is non-explicit
			//    calls copy constructor of left type from right type
			//    --> Y::Y(const X&)
			// C: Y::Y(const X&) is explicit or not declared
			//    does not compile
			Y y2 = x;
		}
		// conversion via copy assignment
		{
			// differs for each scenario:
			// If X& operator=(const Y&) is declared this
			//    calls the implicitly converting copy asignment
			//    --> X& operator=(const Y&)
			// or else either of the following happens:
			// A: Y::operator X() is non-explicit (and X::X(const Y&) might not be declared)
			//    converts to left type via operator first and then calls copy assignment of left type
			//    --> Y::operator X(), X::operator=(const X&)
			// B: Y::operator X() is explicit or not declared and X::X(const Y&) is non-explicit
			//    converts to left type via constructor first and then calls copy assignment of left type
			//    --> X::X(const Y&), X::operator=(const X&)
			// C: Y::operator X() and X::X(const Y&) are both explicit or not declared
			//    does not compile
			X x2; x2 = y;
			// If Y& operator=(const X&) is declared this
			//    calls the implicitly converting copy asignment
			//    --> Y& operator=(const X&)
			// or else either of the following happens:
			// A: not possible since X::operator Y() does not exsist
			// B: Y::Y(const X&) is non-explicit
			//    converts to left type via constructor first and then calls copy assignment of left type
			//    --> Y::Y(const X&), Y::operator=(const Y&)
			// C: Y::Y(const X&) is explicit or not declared
			//    does not compile
			Y y2; y2 = x;
		}
	}
}
