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

void custom_inserter_example()
{
	//TsPoint insertion via custom inserter class
	TsPoint point(40000, 10);
	pantheios::log_DEBUG("TsPoint inserter: ", tsPoint_inserter(point));
}