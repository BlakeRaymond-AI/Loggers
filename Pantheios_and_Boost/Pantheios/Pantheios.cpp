// Pantheios.cpp : Defines the entry point for the console application.
//
#pragma once
#include "stdafx.h"

//Process identity appended to all log messages
extern "C" const char PANTHEIOS_FE_PROCESS_IDENTITY[] = "LogToConsole";

struct TsPoint
{
	TsPoint(double time, double value) :_time(time), _value(value) {}
	double _time;
	double _value;
};

//Custom inserter class for TsPoint
class tsPoint_inserter
{
public:
	tsPoint_inserter(TsPoint const& tspoint)
		: m_tspoint(tspoint),
		  m_length(0)
	{
		m_value[0] = '\0';
	}

	char const* data() const
	{
		if('\0' == m_value[0])
			construct_();
		return m_value;
	}
	size_t length() const
	{
		if('\0' == m_value[0])
			construct_();
		return m_length;
	}
	
private:
	void construct_() const
	{ 
		const_cast<tsPoint_inserter*>(this)->construct_();
	}
	void construct_()
	{
		if('\0' != m_value[0] ||
			  0 != m_length) throw std::exception();

		m_length = static_cast<size_t>(::sprintf(&m_value[0], "Time: %f, Value: %f", m_tspoint._time, m_tspoint._value));
	}
	
	TsPoint const& m_tspoint;
	size_t m_length;
	char m_value[101];
};

namespace stlsoft
{
	inline char const* c_str_data_a(tsPoint_inserter const& inserter)
	{
		return inserter.data();
	}
	inline size_t c_str_len_a(tsPoint_inserter const& inserter)
	{
		return inserter.length();
	}
}
//end custom inserter class for TsPoint

int _tmain(int argc, _TCHAR* argv[])
{
    try
    {
        pantheios::log(pantheios::debug, "Entering main(", pantheios::args(argc,
            argv, pantheios::args::arg0FileOnly), ")");

        pantheios::log_DEBUG("debug");
        pantheios::log_INFORMATIONAL("informational");
        pantheios::log_NOTICE("notice");
        pantheios::log_WARNING("warning");
        pantheios::log_ERROR("error");
        pantheios::log_CRITICAL("critical");
        pantheios::log_ALERT("alert");
        pantheios::log_EMERGENCY("EMERGENCY");

		//Wide-to-multibye inserter
		pantheios::log_DEBUG(pantheios::w2m(L"Wide message"));

		//Integer inserter
		pantheios::log_DEBUG("Integer inserter: ", pantheios::integer(222));

		//TsPoint insertion via custom inserter class
		TsPoint point(40000, 10);
		pantheios::log_DEBUG("TsPoint inserter: ", tsPoint_inserter(point));

		//Up to 32 (or so) items in a log request
		pantheios::log_DEBUG("I ", "can ", "split ", "my ", "log ",
							 "message ", "up ", "unnecessarily ");
		pantheios::log_DEBUG("Everything ends up as a narrow string \
							  so I can add any kind of data in any order",
							  pantheios::integer(13), "/", 
							  pantheios::w2m(L"AI Rocks"), "/", 
							  tsPoint_inserter(TsPoint(13.0, 13.0)));
		
		pantheios::log_WARNING("The party is about to start");

        return EXIT_SUCCESS;
    }
    catch(std::bad_alloc&)
    {
        pantheios::log_ALERT("out of memory");
    }
    catch(std::exception& x)
    {
        pantheios::log_CRITICAL("Exception: ", x);
    }
    catch(...)
    {
        pantheios::puts(pantheios::emergency, "Unexpected unknown error");
    }

    return EXIT_FAILURE;
}

