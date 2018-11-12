
#include <iostream>
#include <fstream>
#include <map>
#include <string>

class Logger_Error: public std::runtime_error
{
public:
	explicit Logger_Error(const std::string& msg) : runtime_error(msg) { };
	explicit Logger_Error(const char* msg) : runtime_error(msg) { };
};

class Logger_File
{
	std::string path;
	std::ofstream ofs;
	std::string prefix;
	std::string postfix;

public:
	Logger_File(const std::string& path);
	~Logger_File() = default;
	Logger_File(const Logger_File&) = delete;
	Logger_File(Logger_File&&) = default;
	const Logger_File& operator=(const Logger_File&) = delete;
	Logger_File& operator=(Logger_File&&) = default;

	bool good() { return ofs.good(); }
	bool is_open() { return ofs.is_open(); }
	const std::string& get_path() const noexcept { return path; }
	const std::string& get_prefix() const noexcept { return prefix; }
	const std::string& get_postfix() const noexcept { return postfix; }

	template<typename T>
	friend Logger_File& operator<<(Logger_File&, const T&);
	friend Logger_File& operator<<(Logger_File&, Logger_File& (*)(Logger_File&));
	friend Logger_File& operator<<(Logger_File&, std::ofstream& (*)(std::ofstream&));
	friend Logger_File& endl(Logger_File& lf);
	friend Logger_File& startlog(Logger_File& lf);
	friend Logger_File& endlog(Logger_File& lf);
};

Logger_File::Logger_File(const std::string& path)
	: path(path), ofs(path) { }

template<typename T>
Logger_File& operator<<(Logger_File& lf, const T& obj)
{
	lf.ofs << obj;
	return lf;
}

Logger_File& operator<<(Logger_File& lf, Logger_File& (*func)(Logger_File&))
{
	func(lf);
	return lf;
}

// requires explicit template parameters e.g.:
// Logger_File lf;
// lf << std::endl; // won't compile!
// lf << std::endl<char, std::char_traits<char>>; // will compile
Logger_File& operator<<(Logger_File& lf, std::ofstream& (*func)(std::ofstream&))
{
	func(lf.ofs);
	return lf;
}


Logger_File& startlog(Logger_File& lf)
{
	if (!lf.prefix.empty())
	{
		lf.ofs << lf.prefix;
	}
	return lf;
}


Logger_File& endlog(Logger_File& lf)
{
	if (!lf.postfix.empty())
	{
		lf.ofs << lf.postfix;
	}
	lf.ofs << std::endl;
	return lf;
}

class Logger_Manager
{
	std::map<const std::string, Logger_File> logger_files;

public:
	Logger_Manager() = default;
	~Logger_Manager() = default;
public:
	void add_logger(const std::string& name, const std::string& path);
	Logger_File& get(const std::string name); // throws Logger_Error
};

void Logger_Manager::add_logger(const std::string& name, const std::string& path)
{
	auto it = logger_files.find(name);
	if (it == logger_files.end())
	{
		logger_files.insert({name, path});
	}
	else
	{
		if (it->second.get_path() != path)
		{
			it->second
				<< "Second logger with same name as this (\""
				<< name
				<< "\") but different path (\""
				<< path
				<< "\") was not created.\n";
		}
	}
}

Logger_File& Logger_Manager::get(const std::string logger_name)
{
	auto it = logger_files.find(logger_name);
	if (it != logger_files.end())
	{
		return it->second;
	}
	else
	{
		std::string msg;
		msg += "Could not find logger with name \"";
		msg += logger_name;
		msg += "\".";
		throw Logger_Error(msg);
	}
}

Logger_Manager log_man;
using namespace std;
int main()
{
	std::string log_name("default");
	log_man.add_logger(log_name, "out.txt");
	log_man.get("default") << "This log file is called: \"" << log_name << "\"" << std::endl<char, std::char_traits<char>>;
	log_man.get("default") << "Some test text ..." << std::endl<char, std::char_traits<char>>;
}
