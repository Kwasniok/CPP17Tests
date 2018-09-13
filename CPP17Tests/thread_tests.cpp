#include "thread_tests.h"

#include <iostream>

using namespace std;

void f(int* i, mutex* m);

// missing:
//     try_lock
//     timed try_lock
//     recursive_mutex
//     multiple simultaious locks via lock/try_lock(lck1, lck2, lck3, ..., lckn)) [remember to use defer!]

void thread_tests() {

	mutex m;
	int i = 0;

	thread t1(bind(f, &i, &m));
	thread t2(bind(f, &i, &m));
	t1.join();
	t2.join();

	cout << "total = " << i << endl;
}

void f(int* i, mutex* m) {
	{
		unique_lock lck(*m, std::defer_lock); // defer means wait for explicit lock command

		if (lck.try_lock()) { // fails save
			++(*i);
			cout << "Thread with id " << this_thread::get_id() << ": Hello from " << *i << "!" << endl;
		} else {
			// lock failed
		}
	}
}
