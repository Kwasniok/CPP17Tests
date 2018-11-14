
#include <iostream>
#include <vector>
#include <algorithm>

// stream a container
template<typename OStream, typename Container>
OStream& operator<<(OStream& os, const Container& c)
{
	for (const auto& e :c)
	{
		os << e << " ";
	}
	return os;
}

// stateless unary predicate
template<typename T>
bool is_odd(T x)
{
	return x % 2 == 0;
}

// stateless unary predicate (simulateing a statefull)
template<auto n>
auto is_divisible_by = [](auto x) { return x % n == 0; };

// statefull unary predicate (good example)
template<typename T>
struct Is_divisible_by
{
	T n;
	Is_divisible_by(T n) : n(n) { };
	bool operator()(T x) { return x % n == 0; }
};

// statefull unary predicate (BAD example!)
template<typename T>
struct Is_divisible_by_bad
{
	T n;
	// This counter can not be trusted (no guarentee in the standard).
	// The execution order might change or it might be copied.
	size_t count = 0;
	Is_divisible_by_bad(T n) : n(n) { };
	bool operator()(T x) { return count++ % n == 0; }
};

using namespace std;

// example usages of the predicates
int main() {

	{
		cout << "unary predicate test (simple):" << endl;
		int xs[] = {0,1,2,3,4,5,6,7,8,9};
		cout << "xs : " << xs << endl;
		for (auto x : xs)
		{
			cout << x << " is odd: " << (is_odd(x)?"true":"false") << endl;
		}
	}
	{
		cout << "unary predicate test (remove_if(is_odd)):" << endl;
		vector<int> xs{0,1,2,3,4,5,6,7,8,9};
		cout << "xs : " << xs << endl;
		for (auto x : xs)
		{
			cout << x << " is odd: " << (is_odd(x)?"remove":"stay") << endl;
		}
		auto it = remove_if(xs.begin(), xs.end(), is_odd<int>); // shift 'to be removed' elements to the end
		xs.erase(it, xs.end()); // earse 'to be removed' elements
		cout << "xs : " << xs << endl;
	}
	{
		cout << "unary predicate test (remove_if(is_divisible_by<3>):" << endl;
		vector<int> xs{0,1,2,3,4,5,6,7,8,9};
		cout << "xs : " << xs << endl;
		for (auto x : xs)
		{
			cout << x << " is odd: " << (is_divisible_by<3>(x)?"remove":"stay") << endl;
		}
		auto it = remove_if(xs.begin(), xs.end(), is_divisible_by<3>); // shift 'to be removed' elements to the end
		xs.erase(it, xs.end()); // earse 'to be removed' elements
		cout << "xs : " << xs << endl;
	}
	{
		cout << "unary predicate test (remove_if(Is_divisible_by(3)):" << endl;
		vector<int> xs{0,1,2,3,4,5,6,7,8,9};
		cout << "xs : " << xs << endl;
		Is_divisible_by idb(3);
		for (auto x : xs)
		{
			cout << x << " is odd: " << (idb(x)?"remove":"stay") << endl;
		}
		auto it = remove_if(xs.begin(), xs.end(), Is_divisible_by(3)); // shift 'to be removed' elements to the end
		xs.erase(it, xs.end()); // earse 'to be removed' elements
		cout << "xs : " << xs << endl;
	}
	{
		cout << "unary predicate test (remove_if(Is_divisible_by_bad(3)):" << endl;
		vector<int> xs{0,1,2,3,4,5,6,7,8,9};
		cout << "xs : " << xs << endl;
		Is_divisible_by_bad idb(3);
		for (auto x : xs)
		{
			cout << x << " is odd: " << (idb(x)?"remove":"stay") << endl;
		}
		auto it = remove_if(xs.begin(), xs.end(), Is_divisible_by_bad(3)); // shift 'to be removed' elements to the end
		xs.erase(it, xs.end()); // earse 'to be removed' elements
		cout << "xs : " << xs << endl;
	}
}
