#pragma once
#include "stdafx.h"
#include <boost/log/attributes/timer.hpp>
#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/common_attributes.hpp>
#include "app_severity_levels.h"

namespace logging = boost::log;
namespace fmt = boost::log::formatters;
namespace flt = boost::log::filters;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;

void runtime_file_destination_example()
{
	string file_dest, log_msg;

	cin.get();
	cout << "Enter file destination: " << endl;
	getline(cin, file_dest, '\n');
	cout << "Enter log message: " << endl;
	getline(cin, log_msg, '\n');

	logging::init_log_to_file
	(
		keywords::file_name = file_dest,
		keywords::open_mode = std::ios::app //This configures the backend to append, rather than overwrite
	);

	cout << "Logging... ";

	src::logger lg;
	BOOST_LOG(lg) << log_msg;

	cout << "done." << endl;
}