#include "stdafx.h"
#include <iomanip>
#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/barrier.hpp>

#include <boost/log/core.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>

#include <boost/log/filters/attr.hpp>

#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/utility/init/common_attributes.hpp>
#include <boost/log/attributes/constant.hpp>

#include <cassert>
#include <iostream>
#include <fstream>
#include <boost/shared_ptr.hpp>
#include <boost/log/common.hpp>
#include <boost/log/filters.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/empty_deleter.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>

#include <boost/log/sources/global_logger_storage.hpp>

using namespace std;

namespace logging = boost::log;
namespace flt = boost::log::filters;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;
namespace fmt = boost::log::formatters;

enum severity_level
{
	normal, 
	warning, 
	error,
	fatal
};

//Global logger for most messages
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(glog, src::severity_logger_mt<severity_level>)

struct TsPoint {
	TsPoint(double time, double value)
		: _time(time), _value(value)
	{}
	TsPoint(const TsPoint &rhs)
		: _time(rhs._time), _value(rhs._value)
	{}
	double _time;
	double _value;
};

ostream& operator<< (ostream& strm, TsPoint const& val)
{
	strm << "T[" << val._time << "] V[" << val._value << "]";
	return strm;
}

class AopFuncs 
{
public:
	AopFuncs(TsPoint tspoint):
	  _tspoint(tspoint)
	  {}

	void arbitrary_data_manip_func(){
		BOOST_LOG_FUNCTION();
		_tspoint._time += 1;
		data_manip_func();
		validate_data_func();
	}

private:
	void validate_data_func(){
		BOOST_LOG_FUNCTION();
		//check that nothing stupid was done to the data
		if(_tspoint._time == -3.14e100)
			BOOST_LOG_SEV(_slg, error) << "TsPoint has bad time value, fixing " << _tspoint;
		_tspoint._time = 0;
		BOOST_LOG_SEV(_slg, normal) << "Changed to 0 ";
	}

	void data_manip_func(){
		BOOST_LOG_FUNCTION();
		//do something stupid to the data
		BOOST_LOG_SEV(_slg, normal) << "Changing the time value to -3.14e100 ";
		_tspoint._time = -3.14e100;
	}

private:
	src::severity_logger<severity_level> _slg;
	TsPoint _tspoint;
};

void init_logging()
{
	string file_name = "logging_sample.log";

	logging::core::get()->add_global_attribute("Scope", attrs::named_scope());
	logging::add_common_attributes(); //LineID, TimeStamp, ProcessID, ThreadID

	typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
	boost::shared_ptr< text_sink > pSink(new text_sink);
	pSink->set_formatter(fmt::stream 
							<< fmt::attr("LineID")
							<< " [" << fmt::date_time< boost::posix_time::ptime >("TimeStamp", "%d.%m.%Y %H:%M:%S.%f")
							<< "] [" << fmt::attr< severity_level >("Severity", std::nothrow)
							<< "] [" << fmt::named_scope("Scope", keywords::iteration = fmt::reverse) 
							<< "] "
							<< fmt::message());

	text_sink::locked_backend_ptr pBackend = pSink->locked_backend();
	boost::shared_ptr< std::ostream > pStream(new ofstream(file_name));
	pBackend->add_stream(pStream);

	logging::core::get()->add_sink(pSink);
}

void always_throws()
{
	BOOST_LOG_FUNCTION();
	BOOST_LOG_SEV(glog::get(), warning) << "About to throw - flushing";
	logging::core::get()->flush();
	throw std::range_error("Big scary exception");
}

void dangerous_func(int i)
{
	BOOST_LOG_FUNCTION();
	if(i < 0) {
		string err = "i must not be negative";
		BOOST_LOG_SEV(glog::get(), error) << err;
		throw std::range_error(err);
	}
}

int main()
{
	init_logging();
	TsPoint point(12.345, 6.789);
	AopFuncs aop_funcs(point);

	BOOST_LOG_FUNCTION();
	
	BOOST_LOG_SEV(glog::get(), normal) << "Calling arbitrary data manip func";
	aop_funcs.arbitrary_data_manip_func();

	try
	{
	BOOST_LOG_SEV(glog::get(), warning) << "Calling dangerous func";
	dangerous_func(-1);
	}
	catch(std::range_error &e)
	{
		e; //supress warning
		BOOST_LOG_SEV(glog::get(), warning) << "Dangerous func failed; continuing";
	}
	cout << "About to throw" << endl;
	always_throws();

	return 0;
}



