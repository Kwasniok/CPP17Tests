
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <chrono>

// appends std::chrono:time_points to a std::basic_stream as unix time with format "UNIX-TIME-MS-<time in ms>"
template<typename T, typename CharT, typename CharTraits = std::char_traits<CharT>>
std::basic_ostream<CharT, CharTraits>& operator<<(std::basic_ostream<CharT, CharTraits>& os, std::chrono::time_point<T> tp)
{
	return os << "UNIX-TIME-MS-" << std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();
}

class Log_Stream_Error: public std::runtime_error
{
public:
	explicit Log_Stream_Error(const std::string& msg) : runtime_error(msg) { };
	explicit Log_Stream_Error(const char* msg) : runtime_error(msg) { };
};

class Log_Stream
{
	std::ofstream ofs;
	std::string path;
	std::string name;
	bool add_timestamps = true;

public:
	Log_Stream(const std::string& path, const std::string& name);
	Log_Stream(std::string&& path, std::string&& name);
	~Log_Stream() = default;
	Log_Stream(const Log_Stream&) = delete;
	Log_Stream(Log_Stream&&) = default;
	Log_Stream& operator=(const Log_Stream&) = delete;
	Log_Stream& operator=(Log_Stream&&) = default;

	bool good() { return ofs.good(); }
	bool is_open() { return ofs.is_open(); }

	const std::string& get_path() const noexcept { return path; }
	const std::string& get_name() const noexcept { return name; }
	void set_name(const std::string& n) { name = n; }
	void set_name(std::string&& n) { name = std::move(n); }
	bool get_add_timestamps() const noexcept { return add_timestamps; }
	void set_add_timestamps(bool ats) { add_timestamps = ats; }

	template<typename T>
	friend Log_Stream& operator<<(Log_Stream& lf, const T& obj);
	friend Log_Stream& operator<<(Log_Stream& lf, std::ofstream& (*func)(std::ofstream&));
	friend Log_Stream& operator<<(Log_Stream& lf, Log_Stream&(*func)(Log_Stream&));
	friend Log_Stream& startlog(Log_Stream& lf);
	friend Log_Stream& endlog(Log_Stream& lf);
	friend Log_Stream& endl(Log_Stream& lf); // needs own endl due to deduction issues with std::endl (but std::hex might work!?)
};
Log_Stream& now(Log_Stream& lf);

Log_Stream::Log_Stream(const std::string& path, const std::string& name)
	: path(path), name(name), ofs(path) { }

Log_Stream::Log_Stream(std::string&& path, std::string&& name)
	: path(path), name(std::move(name)), ofs(path) { }

template<typename T>
Log_Stream& operator<<(Log_Stream& lf, const T& obj)
{
	lf.ofs << obj;
	return lf;
}
Log_Stream& operator<<(Log_Stream& lf, std::ofstream& (*func)(std::ofstream&))
{
	func(lf.ofs);
	return lf;
}
Log_Stream& operator<<(Log_Stream& lf, Log_Stream&(*func)(Log_Stream&))
{
	return func(lf);
}
Log_Stream& startlog(Log_Stream& lf)
{
	bool has_prefix = false;
	if (!lf.name.empty())
	{
		lf << lf.name;
		has_prefix = true;
	}
	if (lf.add_timestamps)
	{
		lf << "@" << now;
		has_prefix = true;
	}
	if (has_prefix)
	{
		lf << ": ";
	}
	return lf;
}
Log_Stream& endlog(Log_Stream& lf)
{
	lf.ofs << std::endl; // implicit flush!
	return lf;
}
Log_Stream& endl(Log_Stream& lf)
{
	lf.ofs << std::endl;
	return lf;
}
Log_Stream& now(Log_Stream& lf)
{
	return lf << std::chrono::system_clock::now();
}

class Log_Stream_Manager
{
private:
	std::map<const std::string, Log_Stream> log_streams;

public:
	Log_Stream_Manager() = default;
	~Log_Stream_Manager() = default;
public:
	void add_log(const std::string& name, const std::string& path); // throws Log_Stream_Error
	Log_Stream& get_log(const std::string name); // throws Log_Stream_Error
};

void Log_Stream_Manager::add_log(const std::string& name, const std::string& path)
{
	// check if a log stream with this name exists and add one if not
	auto it = log_streams.find(name);
	if (it == log_streams.end())
	{
		// a log stream with this name does not exist
		// a new log stream is created and added iff it is opened(/working)
		Log_Stream lf(path, name);
		lf << startlog << "Started logging: name=\"" << name << "\", path= \"" << path << "\""<< endlog;
		if (lf.is_open())
		{
			// creation of log stream was successful: add it
			log_streams.emplace(name, std::move(lf));
		}
		else
		{
			// creation of log stream was unsuccessful: report it as an excpetion
			std::string msg;
			msg += "Could not create log stream with name \"";
			msg += name;
			msg += "\" at path \"";
			msg += path;
			msg += "\".";
			throw Log_Stream_Error(msg);
		}
	}
	else {
		// there exists allready another log stream with the same name (but maybe with a different path)
		// report this to the allready existing log stream
		it->second
			<< startlog
			<< "An attempt to add another log stream with the same name (\""
			<< name
			<< "\") and path (\""
			<< path
			<< "\") was detected."
			<< endl
			<< "The current path of this file is \""
			<< it->second.get_path()
			<< "\""
			<< endlog;
	}
}

Log_Stream& Log_Stream_Manager::get_log(const std::string name)
{
	auto it = log_streams.find(name);
	if (it != log_streams.end())
	{
		return it->second;
	}
	/*
	else if(it = log_streams.find(/ *some name* /), it != log_streams.end())
	{
		// report missing log stream (once?)
		return it->second;
	}
	*/
	else
	{
		std::string msg;
		msg += "Could not find log stream with name \"";
		msg += name;
		msg += "\".";
		throw Log_Stream_Error(msg);
	}
}

Log_Stream_Manager log_man;

int main()
{
	std::string log_name("default");

	log_man.add_log(log_name, "out.txt");
	auto& log = log_man.get_log(log_name);
	log << startlog << "Some test text ..." << endlog;
}
