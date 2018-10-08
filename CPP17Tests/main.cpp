
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct Counter
{
	size_t c = 0;
	virtual void operator++() { ++c; }
};

int main()
{
	size_t repetitions = 0xFFFFFFFF;
	for (size_t j = 0; j < repetitions; ++j);
	{
		size_t* i = new size_t(0);
		auto start = std::chrono::high_resolution_clock::now();
		{
			for (size_t j = 0; j < repetitions; ++j) ++*i;
		}
		auto stop = std::chrono::high_resolution_clock::now();
		auto diff = duration_cast<duration<double, nano>>(stop - start);

		cout << "variant = 'int*'" << endl;
		cout << "repetitions = " << *i << endl;
		cout << "took " << diff.count() * 10E-9 << "s (" << diff.count() / repetitions << "ns per cycle)" << endl;
		delete i;
	}
	{
		size_t i = 0;
		auto start = std::chrono::high_resolution_clock::now();
		{
			for (size_t j = 0; j < repetitions; ++j) ++i;
		}
		auto stop = std::chrono::high_resolution_clock::now();
		auto diff = duration_cast<duration<double, nano>>(stop - start);

		cout << "variant = 'int'" << endl;
		cout << "repetitions = " << i << endl;
		cout << "took " << diff.count() * 10E-9 << "s (" << diff.count() / repetitions << "ns per cycle)" << endl;
	}
	{
		Counter* c = new Counter;
		auto start = std::chrono::high_resolution_clock::now();
		{
			for (size_t j = 0; j < repetitions; ++j) c->operator++();
		}
		auto stop = std::chrono::high_resolution_clock::now();
		auto diff = duration_cast<duration<double, nano>>(stop - start);

		cout << "variant = 'Counter*'" << endl;
		cout << "repetitions = " << c->c << endl;
		cout << "took " << diff.count() * 10E-9 << "s (" << diff.count() / repetitions << "ns per cycle)" << endl;
		delete c;
	}
	{
		Counter c;
		auto start = std::chrono::high_resolution_clock::now();
		{
			for (size_t j = 0; j < repetitions; ++j) ++c;
		}
		auto stop = std::chrono::high_resolution_clock::now();
		auto diff = duration_cast<duration<double, nano>>(stop - start);

		cout << "variant = 'Counter'" << endl;
		cout << "repetitions = " << c.c << endl;
		cout << "took " << diff.count() * 10E-9 << "s (" << diff.count() / repetitions << "ns per cycle)" << endl;
	}

	/* possible output with DEBUG build (MSVC 2017 15.8.2 and C++17 on Surface Book 2):

		variant = 'int*'
		repetitions = 4294967295
		took 74.7538s (1.7405ns per cycle)
		variant = 'int'
		repetitions = 4294967295
		took 64.397s (1.49936ns per cycle)
		variant = 'Counter*'
		repetitions = 4294967295
		took 854.693s (19.8999ns per cycle)
		variant = 'Counter'
		repetitions = 4294967295
		took 823.146s (19.1654ns per cycle)

	*/
}
