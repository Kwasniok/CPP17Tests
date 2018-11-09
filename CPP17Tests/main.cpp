
#include <iostream>
#include <fstream>

using namespace std;

void display_arguments(const int argc, const char *const *const args);
bool get_file_names(const int argc, const char *const *const args, const char*& ifname, const char*& ofname);

template<class In, class Out>
void transfer_stream(In& is, Out& os)
{
	for (char c; is.good(); )
	{
		c = is.get();
		if (is.eof()) break;
		os << c;
	}
}

int main(int argc, char** args)
{
	// display_arguments(argc, args);

	const char* ifname = nullptr;
	const char* ofname = nullptr;

	if (!get_file_names(argc, args, ifname, ofname))
	{
		cout << "Could not resolve io file paths. Exiting." << endl;
		return EXIT_FAILURE;
	}

	if (ofname && ifname)
	{
		ifstream ifs(ifname);
		ofstream ofs(ofname);

		transfer_stream(ifs, ofs);
	}
	else if (ofname)
	{
		ofstream ofs(ofname);

		transfer_stream(cin, ofs);
	}
	else
	{
		transfer_stream(cin, cout);
	}

	return EXIT_SUCCESS;
}

void display_arguments(const int argc, const char *const *const args)
{
	for (int i = 0; i < argc; ++i)
		cout << "args[" << i << "] = \"" << args[i] << "\"" << endl;
}

bool get_file_names(const int argc, const char *const *const args, const char*& ifname, const char*& ofname)
{
	if (argc == 3)
	{
		ifname = args[1];
		ofname = args[2];
	}
	else if (argc == 2)
	{
		ofname = args[1];
	}
	else if (argc == 1)
	{
		// do nothing
	}
	else
	{
		cout << "Uunsupported number of arguments. Use 1 or 2 file paths as arguments." << endl;
		return false;
	}
	return true;
}


