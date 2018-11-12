
#include "Log_Stream.h"
#include <iostream>

Log_Stream_Manager log_man;

int main()
{
	/*
	// basic tests
	{ // supported (one log stream per file)
		auto& log1 = log_man.add_log("error", "error.txt");
		log1 << beginlog << "Some test text ..." << endlog;

		auto& log2 = log_man.add_log("warning", "warning.txt");
		log2 << beginlog << "Some test text ..." << endlog;
	}
	{ // unsupported!!! (multiple log streams per file)
		auto& log1 = log_man.add_log("info1", "pool.txt");
		log1 << beginlog << "Some test text ... 1" << endlog;

		auto& log2 = log_man.add_log("info2", "pool.txt");
		log2 << beginlog << "Some test text ... 2" << endlog;
	}
	*/

	/*
	// speed test
	{ // log stream with time stamp 20x to 50x faster than std::cerr without time stamp
		int reps = 10000;
		{
			auto start = std::chrono::system_clock::now();
			for (int i = 0; i < reps; ++i)
			{
				std::cerr << "TEST" << std::endl;
			}
			auto stop = std::chrono::system_clock::now();
			auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);
			std::cout << "std::cerr delta t=" << diff.count() << "ns,  #cycles =" << reps << ", delta t/cylce ns = " << diff.count()/reps << std::endl;
		}
		{
			auto& log = log_man.add_log("test", "test.txt");
			auto start = std::chrono::system_clock::now();
			for (int i = 0; i < reps; ++i)
			{
				log << beginlog << "TEST" << endlog;
			}
			auto stop = std::chrono::system_clock::now();
			auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);
			std::cout << "log stream delta t=" << diff.count() << "ns,  #cycles =" << reps << ", delta t/cylce ns = " << diff.count()/reps << std::endl;
		}
	}
	*/
}
