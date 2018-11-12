
#include "Log_Stream.h"

Log_Stream::Log_Stream(const std::string& path, const std::string& name)
	: path(path), name(name), ofs(path) { }

Log_Stream::Log_Stream(std::string&& path, std::string&& name)
	: path(path), name(std::move(name)), ofs(path) { }

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

Log_Stream& Log_Stream_Manager::add_log(const std::string& name, const std::string& path)
{
	// check if a log stream with this name exists and add one if not
	auto it = log_streams.find(name);
	if (it == log_streams.end())
	{
		// a log stream with this name does not exist
		// a new log stream is created and added iff it is opened(/working)
		Log_Stream lf(path, name);
		lf << startlog << "Started logging: name=\"" << name << "\", path=\"" << path << "\""<< endlog;
		if (lf.is_open())
		{
			// creation of log stream was successful: add it
			auto res = log_streams.emplace(name, std::move(lf));
			// return refernce to this log stream
			return res.first->second;
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
		// return refernce to this log stream
		return it->second;
	}
}

Log_Stream& Log_Stream_Manager::get_log(const std::string name)
{
	// search for log stream
	auto it = log_streams.find(name);
	if (it != log_streams.end())
	{
		// found stream: return it
		return it->second;
	}
	/*
	else if(it = log_streams.find(name of fallback log stream), it != log_streams.end()) // try fallback
	{
		// report missing fallback log stream (once?)
		return it->second;
	}
	*/
	else
	{
		// report missing log stream as exception
		std::string msg;
		msg += "Could not find log stream with name \"";
		msg += name;
		msg += "\".";
		throw Log_Stream_Error(msg);
	}
}
