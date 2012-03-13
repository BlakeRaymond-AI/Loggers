#pragma once
#include "stdafx.h"

//It is also possible to write new inserter classes for user-defined types
struct TsPoint
{
	TsPoint(double time, double value) :_time(time), _value(value) {}
	double _time;
	double _value;
};

//The inserter class should use lazy initialization,
//since the message won't be formatted if the Pantheios core
//determines not to process the message (i.e. severity too low)
class tsPoint_inserter
{
public:
	tsPoint_inserter(TsPoint const& tspoint)
		: m_tspoint(tspoint),
		  m_value("\0")
	{}

	char const* data() const
	{
		if("\0" == m_value)
			construct_();
		return m_value.c_str();
	}
	size_t length() const
	{
		if("\0" == m_value)
			construct_();
		return m_value.size();
	}
	
private:
	void construct_() const
	{ 
		const_cast<tsPoint_inserter*>(this)->construct_();
	}
	void construct_()
	{
		if("\0" != m_value) throw std::exception();

		std::stringstream ss;
		ss << "Time: " << m_tspoint._time << ", Value: " << m_tspoint._value;
		m_value = ss.str();
	}
	
	TsPoint const& m_tspoint;
	std::string m_value;
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

//It is not necessary to define a separate inserter class; simply writing 
//a custom "shim" alone is also a solution, though less efficient.
struct MetaDataItem
{
	MetaDataItem(double start, double end, int code)
		: _start(start),
		  _end(end),
		  _code(code)
	{}
	double _start;
	double _end;
	int	   _code;
};

//The following code is mostly taken from a tutorial 
//but it must have been for an older version of the library since
//stlsoft::shim_string no longer exists, at least not under that name.
//namespace stlsoft
//{
//	inline stlsoft::shim_string<char> c_str_data_a(MetaDataItem const& item)
//	{
//		stlsoft::shim_string<char> s(101);
//		int cch = ::sprintf(s, "Start time: %f, End time: %f, Code: %d", item._start, item._end, item._code);
//		s.truncate(static_cast<size_t>(cch));
//		return s;
//	}
//
//	inline size_t c_str_len_a(MetaDataItem const& item)
//	{
//		char buf[101];
//		return static_cast<size_t>(::sprintf(s, "Start time: %f, End time: %f, Code: %d", item._start, item._end, item._code));
//	}
//}

void custom_inserter_example()
{
	//TsPoint insertion via custom inserter class
	TsPoint point(40000, 10);
	pantheios::log_DEBUG("TsPoint inserter: ", tsPoint_inserter(point));

	//MetaData insertion via custom shim (not working as it should be)
	//MetaDataItem item(40000, 40001, -1);
	//pantheios::log_DEBUG("MetaDataItem shim: ", item);
}