
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char** args)
{
	for (int i = 0; i < argc; ++i)
		cout << "args[" << i << "] = \"" << args[i] << "\"" << endl;

	const char* ifname = nullptr;
	const char* ofname = nullptr;

	istream* isp = &cin;
	ostream* osp = &cout;

	{
		unique_ptr<ifstream> ifs;
		unique_ptr<ofstream> ofs;

		if (argc == 3)
		{
			ifname = args[1];
			ofname = args[2];
		}
		else if (argc == 2)
		{
			ofname = args[1];
		}
		else
		{
			cout << "unsupported number of arguments" << endl;
			return EXIT_FAILURE;
		}

		if (ifname)
		{
			ifs = make_unique<ifstream>(ifname);
			if (ifs->good() && ifs->is_open())
			{
				isp = ifs.get();
			}
			else
			{
				cout << "could not open inputfile \"" << ifname << "\"" << endl;
			}
		}
		if (ofname)
		{
			ofs = make_unique<ofstream>(ofname);
			if (ofs->good() && ofs->is_open())
			{
				osp = ofs.get();
				cout << "opend file \"" << ofname << "\"" << endl;
			}
			else
			{
				cout << "could not open outputfile \"" << ofname << "\"" << endl;
			}
		}

		char c;
		while (isp->good()) {
			char c = isp->get();
			if (isp->eof()) break;
			*osp << c;
		}

		osp->flush();

		isp = &cin;
		osp = &cout;
	}

	return EXIT_SUCCESS;
}
