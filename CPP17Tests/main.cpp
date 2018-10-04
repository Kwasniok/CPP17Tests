
#include <iostream>
#include <string>

using namespace std;

struct Employee
{
	const string first_ = "Ben";
	const string last_ = "Parker";
	const string title_ = "CEO";
	const string& first() { return first_; }
	const string& last() { return last_; }
	const string& title() { return last_; }
	int salary() { return 10; }
};

string evaluate(Employee& e)
{
	string name = e.first() + " " + e.last();
	if (e.title() == "CEO" || e.salary() > 5)
	{
		string msg = name + "is overpaid\n"; // not portable!
		cout << msg;
	}
	return name; // noexcept move (checked with MS Studio and C++17 (with DEBUG!))
}

int main() {
	Employee e;
	string s = evaluate(e);
}
