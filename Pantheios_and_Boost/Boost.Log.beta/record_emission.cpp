/*
 *          Copyright Andrey Semashev 2007 - 2012.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   record_emission.cpp
 * \author Andrey Semashev
 * \date   22.03.2009
 *
 * \brief  This code measures performance of log record emission
 */

// #define BOOST_LOG_USE_CHAR
// #define BOOST_ALL_DYN_LINK 1
// #define BOOST_LOG_DYN_LINK 1
#define BOOST_NO_DYN_LINK 1

#include "stdafx.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/barrier.hpp>

#include <boost/log/core.hpp>
#include <boost/log/common.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>

#include <boost/log/filters/attr.hpp>

#include <boost/log/attributes/scoped_attribute.hpp>

#include <boost/log/sources/logger.hpp>

enum config
{
    RECORD_COUNT = 1000000,
    THREAD_COUNT = 3,
    SINK_COUNT = 1
};

namespace logging = boost::log;
namespace flt = boost::log::filters;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;

using namespace std;

enum severity_level
{
    normal,
    warning,
    error
};

const int NUM_RECORDS = 1000000;

void do_logs_no_attrs(boost::shared_ptr<sinks::synchronous_sink< sinks::text_ostream_backend > > sink)
{
	boost::shared_ptr< logging::core > core = logging::core::get();
	core->add_sink(sink);

	src::logger lg;

	for(size_t i=0; i<NUM_RECORDS; ++i) {
		BOOST_LOG(lg) << "Shoop da woop";
	}
	core->remove_sink(sink);
}

void do_logs_basic_attrs(boost::shared_ptr<sinks::synchronous_sink< sinks::text_ostream_backend > > sink)
{
	boost::shared_ptr< logging::core > core = logging::core::get();
	core->add_sink(sink);

	src::logger lg;
	lg.add_attribute("LineID", attrs::counter< unsigned int >(1));
	lg.add_attribute("TimeStamp", attrs::local_clock());
	lg.add_attribute("Scope", attrs::named_scope());
	for(size_t i=0; i<NUM_RECORDS; ++i) {
		BOOST_LOG(lg) << "Shoop da woop";
	}

	core->remove_sink(sink);
}

void simple_test_no_attrs()
{
	string file_name = "simple_test_no_attrs.log";

	boost::shared_ptr< std::ostream > o_stream(new std::ofstream(file_name));

	//One thread, two text stream backends to file.
	boost::shared_ptr< logging::core > core = logging::core::get();

	boost::shared_ptr< sinks::text_ostream_backend > autof_backend =
		boost::make_shared< sinks::text_ostream_backend >();
	autof_backend->add_stream(o_stream);
	autof_backend->auto_flush(true);

	boost::shared_ptr< sinks::text_ostream_backend > noautof_backend =
		boost::make_shared< sinks::text_ostream_backend >();
	noautof_backend->add_stream(o_stream);
	noautof_backend->auto_flush(false);

	typedef sinks::synchronous_sink< sinks::text_ostream_backend > sink_t;
	boost::shared_ptr< sink_t > autof_sink(new sink_t(autof_backend));
	boost::shared_ptr< sink_t > noautof_sink(new sink_t(noautof_backend));

	typedef boost::date_time::microsec_clock< boost::posix_time::ptime > time;
	
	boost::posix_time::ptime start = time::universal_time(), 
						     interm, 
							 end;
	cout << "Auto flush on...";
	do_logs_no_attrs(autof_sink);
	cout << " done." << endl;
	interm = time::universal_time();
	cout << "Auto flush off...";
	do_logs_no_attrs(noautof_sink);
	cout << " done." << endl;
	end = time::universal_time();

	unsigned long long duration1 = (interm - start).total_microseconds(), 
					   duration2 = (end - interm).total_microseconds();

	cout << "Results: " << endl;
	cout << "Auto flush on: \n\t" << duration1 << " us ("
		 << std::fixed << std::setprecision(4) 
		 << static_cast< double >(NUM_RECORDS) / (static_cast< double >(duration1) / 1000000.0)
         << " records per second, " 
		 << (static_cast< double >(duration1) / 1000.0) / static_cast< double >(NUM_RECORDS)  
		 << " ms per record)" << std::endl;
	cout << "Auto flush off: \n\t" << duration2 << " us ("
		 << std::fixed << std::setprecision(4) 
		 << static_cast< double >(NUM_RECORDS) / (static_cast< double >(duration2) / 1000000.0)
         << " records per second, " 
		 << (static_cast< double >(duration2) / 1000.0) / static_cast< double >(NUM_RECORDS)  
		 << " ms per record)" << std::endl;
}

void simple_test_basic_attrs()
{
	string file_name = "simple_test_basic_attrs.log";

	boost::shared_ptr< std::ostream > o_stream(new std::ofstream(file_name));

	//One thread, two text stream backends to file.
	boost::shared_ptr< logging::core > core = logging::core::get();

	boost::shared_ptr< sinks::text_ostream_backend > autof_backend =
		boost::make_shared< sinks::text_ostream_backend >();
	autof_backend->add_stream(o_stream);
	autof_backend->auto_flush(true);

	boost::shared_ptr< sinks::text_ostream_backend > noautof_backend =
		boost::make_shared< sinks::text_ostream_backend >();
	noautof_backend->add_stream(o_stream);
	noautof_backend->auto_flush(false);

	typedef sinks::synchronous_sink< sinks::text_ostream_backend > sink_t;
	boost::shared_ptr< sink_t > autof_sink(new sink_t(autof_backend));
	boost::shared_ptr< sink_t > noautof_sink(new sink_t(noautof_backend));


	typedef boost::date_time::microsec_clock< boost::posix_time::ptime > time;
	
	boost::posix_time::ptime start = time::universal_time(), 
						     interm, 
							 end;
	cout << "Auto flush on...";
	do_logs_basic_attrs(autof_sink);
	cout << " done." << endl;
	interm = time::universal_time();
	cout << "Auto flush off...";
	do_logs_basic_attrs(noautof_sink);
	cout << " done." << endl;
	end = time::universal_time();

	unsigned long long duration1 = (interm - start).total_microseconds(), 
					   duration2 = (end - interm).total_microseconds();

	cout << "Results: " << endl;
	cout << "Auto flush on: \n\t" << duration1 << " us ("
		 << std::fixed << std::setprecision(4) 
		 << static_cast< double >(NUM_RECORDS) / (static_cast< double >(duration1) / 1000000.0)
         << " records per second, " 
		 << (static_cast< double >(duration1) / 1000.0) / static_cast< double >(NUM_RECORDS)  
		 << " ms per record)" << std::endl;
	cout << "Auto flush off: \n\t" << duration2 << " us ("
		 << std::fixed << std::setprecision(4) 
		 << static_cast< double >(NUM_RECORDS) / (static_cast< double >(duration2) / 1000000.0)
         << " records per second, " 
		 << (static_cast< double >(duration2) / 1000.0) / static_cast< double >(NUM_RECORDS)  
		 << " ms per record)" << std::endl;
}

int main(int argc, char* argv[])
{
	cout << "No attributes: " << endl;
    simple_test_no_attrs();
	cout << "Basic attributes: " << endl;
	simple_test_basic_attrs();
    return 0;
}
