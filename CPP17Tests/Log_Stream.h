#pragma once

#include <fstream>
#include <string>
#include <map>
#include <chrono>

// appends std::chrono:time_points to a std::basic_stream as unix time with format "unix-time-ns-<unix time in ns>"
// TODO: override this function for correct std::chrono::time_point formatting
template<typename T, typename CharT, typename CharTraits = std::char_traits<CharT>>
std::basic_ostream<CharT, CharTraits>& operator<<(std::basic_ostream<CharT, CharTraits>& os, std::chrono::time_point<T> tp)
{
	return os << "unix-time-ns-" << std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();
}

// basic exception for log stream related runtime errors
class Log_Stream_Error: public std::runtime_error
{
public:
	explicit Log_Stream_Error(const std::string& msg) : runtime_error(msg) { };
	explicit Log_Stream_Error(const char* msg) : runtime_error(msg) { };
};

// handles the log stream
// move-only object
class Log_Stream
{
	// holds output file
	std::ofstream ofs;
	// stores path to output file
	const std::string path;
	std::string name;
	bool add_timestamps = true;

public:
	// path: path to log file (e.g. "error.txt") cannot be
	// name: name of log stream (e.g. "error")
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
	friend Log_Stream& beginlog(Log_Stream& lf);
	friend Log_Stream& endlog(Log_Stream& lf);
	friend Log_Stream& endl(Log_Stream& lf); // needs own endl due to deduction issues with std::endl (but std::hex might work!?)
};

Log_Stream& now(Log_Stream& lf);

// treat output of objects like a ofstream
template<typename T>
Log_Stream& operator<<(Log_Stream& lf, const T& obj)
{
	lf.ofs << obj;
	return lf;
}
// treat manipulators like a ofstream (does not work for std::endl due to overload issues)
Log_Stream& operator<<(Log_Stream& lf, std::ofstream& (*func)(std::ofstream&));
// manipulator hook for log streams
Log_Stream& operator<<(Log_Stream& lf, Log_Stream&(*func)(Log_Stream&));
// call this at the beginnig of each log
// might append a tag, a time stamp etc.
Log_Stream& beginlog(Log_Stream& lf);
// call at the end of each log
// appends a new line and flushes the stream
Log_Stream& endlog(Log_Stream& lf);
// special endl function equivalent ot std::endl for log streams
// (std::endl does not work with log streams due to overloading issues)
Log_Stream& endl(Log_Stream& lf);

// manages log streams
// there is allways only one log stream for each name
class Log_Stream_Manager
{
	std::map<const std::string, Log_Stream> log_streams;
public:
	~Log_Stream_Manager();
	// add a log stream with a given name and path
	// returns a reference to a log stream with the given name
	// if one allready exists no new stream will be created even if the paths differ!
	// throws Log_Stream_Error
	Log_Stream& add_log(const std::string& name, const std::string& path);
	// returns a stream with the given name
	// throws Log_Stream_Error if none exists
	Log_Stream& get_log(const std::string& name);
	// removes a log stream with the given name if possible
	bool remove_log(const std::string& name);
};
