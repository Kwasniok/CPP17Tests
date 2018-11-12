
#include "Log_Stream.h"

Log_Stream_Manager log_man;

int main()
{
	{ // supported (one log stream per file)
		auto& log1 = log_man.add_log("error", "error.txt");
		log1 << startlog << "Some test text ..." << endlog;

		auto& log2 = log_man.add_log("warning", "warning.txt");
		log2 << startlog << "Some test text ..." << endlog;
	}
	{ // unsupported!!! (multiple log streams per file)
		auto& log1 = log_man.add_log("info1", "pool.txt");
		log1 << startlog << "Some test text ... 1" << endlog;

		auto& log2 = log_man.add_log("info2", "pool.txt");
		log2 << startlog << "Some test text ... 2" << endlog;
	}
}
