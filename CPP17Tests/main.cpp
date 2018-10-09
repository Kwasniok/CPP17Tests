
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

// a counter with a virtual pre-increment operator
struct Counter
{
	size_t c = 0;
	virtual void operator++() { ++c; }
};

// NOTE: Mind the enabled or disabled compiler optimizations when interpreting the results!
int main()
{
	// set-up output format
	cout.precision(2);

	// amount of repetitions for each operation
	size_t repetitions = 0xFFFFFFF;

	// warm-up to get the CUP into steady worik state
	for (size_t j = 0; j < repetitions; ++j);
	cout << "warmp-up done" << endl;

	// tests:
	// repeat a no-op and measure the time
	{
		auto start = std::chrono::high_resolution_clock::now();
		{
			for (size_t j = 0; j < repetitions; ++j);
		}
		auto stop = std::chrono::high_resolution_clock::now();
		auto diff = duration_cast<duration<double, nano>>(stop - start);

		cout << "variant = 'loop only'" << endl;
		cout << "repetitions = " << repetitions << endl;
		cout << "took " << diff.count() * 1E-9 << "s (" << diff.count() / repetitions << "ns per cycle)" << endl;
	}
	// repeat incrementing a counter via dereferencing and measure the time
	for (size_t j = 0; j < repetitions; ++j);
	{
		size_t* i = new size_t(0);
		auto start = std::chrono::high_resolution_clock::now();
		{
			for (size_t j = 0; j < repetitions; ++j) ++*i;
		}
		auto stop = std::chrono::high_resolution_clock::now();
		auto diff = duration_cast<duration<double, nano>>(stop - start);

		cout << "variant = 'size_t*'" << endl;
		cout << "repetitions = " << *i << endl;
		cout << "took " << diff.count() * 1E-9 << "s (" << diff.count() / repetitions << "ns per cycle)" << endl;
		delete i;
	}
	// repeat incrementing a counter via smart pointer dereferencing and measure the time
	for (size_t j = 0; j < repetitions; ++j);
	{
		auto i = make_unique<size_t>(0);
		auto start = std::chrono::high_resolution_clock::now();
		{
			for (size_t j = 0; j < repetitions; ++j) ++*i;
		}
		auto stop = std::chrono::high_resolution_clock::now();
		auto diff = duration_cast<duration<double, nano>>(stop - start);

		cout << "variant = 'unique'_ptr<size_t>" << endl;
		cout << "repetitions = " << *i << endl;
		cout << "took " << diff.count() * 1E-9 << "s (" << diff.count() / repetitions << "ns per cycle)" << endl;
	}
	// repeat incrementing a counter and measure the time
	{
		size_t i = 0;
		auto start = std::chrono::high_resolution_clock::now();
		{
			for (size_t j = 0; j < repetitions; ++j) ++i;
		}
		auto stop = std::chrono::high_resolution_clock::now();
		auto diff = duration_cast<duration<double, nano>>(stop - start);

		cout << "variant = 'size_t'" << endl;
		cout << "repetitions = " << i << endl;
		cout << "took " << diff.count() * 1E-9 << "s (" << diff.count() / repetitions << "ns per cycle)" << endl;
	}
	// repeat incrementing a counter via a dereferenced virtual function and measure the time
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
		cout << "took " << diff.count() * 1E-9 << "s (" << diff.count() / repetitions << "ns per cycle)" << endl;
		delete c;
	}
	// repeat incrementing a counter via a smart pointer dereferenced virtual function and measure the time
	{
		auto c = make_unique<Counter>();
		auto start = std::chrono::high_resolution_clock::now();
		{
			for (size_t j = 0; j < repetitions; ++j) c->operator++();
		}
		auto stop = std::chrono::high_resolution_clock::now();
		auto diff = duration_cast<duration<double, nano>>(stop - start);

		cout << "variant = 'unique_ptr<Counter>'" << endl;
		cout << "repetitions = " << c->c << endl;
		cout << "took " << diff.count() * 1E-9 << "s (" << diff.count() / repetitions << "ns per cycle)" << endl;
	}
	// repeat incrementing a counter via a (non-virtual) function and measure the time
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
		cout << "took " << diff.count() * 1E-9 << "s (" << diff.count() / repetitions << "ns per cycle)" << endl;
	}

	/* possible output with DEBUG build  without optimization (gcc, MSVC 2017 15.8.2 and C++17 on Surface Book 2):

		warmp-up done
		variant = 'loop only'
		repetitions = 268435455
		took 0.63s (2.3ns per cycle)
		variant = 'size_t*'
		repetitions = 268435455
		took 0.46s (1.7ns per cycle)
		variant = 'unique'_ptr<size_t>
		repetitions = 268435455
		took 21s (77ns per cycle)
		variant = 'size_t'
		repetitions = 268435455
		took 0.4s (1.5ns per cycle)
		variant = 'Counter*'
		repetitions = 268435455
		took 5.4s (20ns per cycle)
		variant = 'unique_ptr<Counter>'
		repetitions = 268435455
		took 20s (76ns per cycle)
		variant = 'Counter'
		repetitions = 268435455
		took 5.1s (19ns per cycle)

	possible output with RELEASE build with optimization -O2 (favor speed) (gcc, MSVC 2017 15.8.2 and C++17 on Surface Book 2):
	NOTE: Due to optimization some results might be non representative!

		warmp-up done
		variant = 'loop only'
		repetitions = 4294967295
		took 0s (0ns per cycle)
		variant = 'size_t*'
		repetitions = 4294967295
		took 7s (1.6ns per cycle)
		variant = 'unique'_ptr<size_t>
		repetitions = 4294967295
		took 6.8s (1.6ns per cycle)
		variant = 'size_t'
		repetitions = 4294967295
		took 0s (0ns per cycle)
		variant = 'Counter*'
		repetitions = 4294967295
		took 6.5s (1.5ns per cycle)
		variant = 'unique_ptr<Counter>'
		repetitions = 4294967295
		took 6.5s (1.5ns per cycle)
		variant = 'Counter'
		repetitions = 4294967295
		took 6.5s (1.5ns per cycle)

	possible output with RELEASE build with optimization -Ox (favor speed) (gcc, MSVC 2017 15.8.2 and C++17 on Surface Book 2):
	NOTE: Due to optimization some results might be non representative!

		warmp-up done
		variant = 'loop only'
		repetitions = 4294967295
		took 3.8e-07s (8.8e-08ns per cycle)
		variant = 'size_t*'
		repetitions = 4294967295
		took 7s (1.6ns per cycle)
		variant = 'unique'_ptr<size_t>
		repetitions = 4294967295
		took 6.8s (1.6ns per cycle)
		variant = 'size_t'
		repetitions = 4294967295
		took 0s (0ns per cycle)
		variant = 'Counter*'
		repetitions = 4294967295
		took 6.5s (1.5ns per cycle)
		variant = 'unique_ptr<Counter>'
		repetitions = 4294967295
		took 6.5s (1.5ns per cycle)
		variant = 'Counter'
		repetitions = 4294967295
		took 6.5s (1.5ns per cycle)

	In a RELEASE build with speed optimization enabled raw pointer and smart pointer dereferencing are equaly fast
	and add only a small overhead (about 50% in time).

	*/
}
