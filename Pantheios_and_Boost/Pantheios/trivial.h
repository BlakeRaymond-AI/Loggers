#pragma once
#include "stdafx.h"

void trivial_example() 
{
		//The logging suite needs no explicit initialization.  
		//We are using the WindowsConsole stock back-end in this example.

		//Different severity levels, in ascending order
		pantheios::log_DEBUG("debug");
        pantheios::log_INFORMATIONAL("informational");
        pantheios::log_NOTICE("notice");
        pantheios::log_WARNING("warning");
        pantheios::log_ERROR("error");
        pantheios::log_CRITICAL("critical");
        pantheios::log_ALERT("alert");
        pantheios::log_EMERGENCY("EMERGENCY");

		//All log messages are stored internally as strings, 
		//so other data types must be converted.  This is usually done
		//using "inserters."  Several stock inserters are available, 
		//including:

		//Wide-to-multibye inserter
		pantheios::log_DEBUG(pantheios::w2m(L"Wide message"));

		//Integer inserter
		pantheios::log_DEBUG("Integer inserter: ", pantheios::integer(222));

		//You can add up to 32 items in a log request.  
		//All items get concatenated efficiently (often with no memory allocation)
		//by the Pantheios core.
		pantheios::log_DEBUG("I ", "can ", "split ", "my ", "log ",
							 "message ", "up ", "unnecessarily ");
		pantheios::log_DEBUG("Everything ends up as a narrow string so I can add any kind of data in any order/",
							  pantheios::integer(13), "/", 
							  pantheios::w2m(L"AI Rocks"));
		pantheios::log_DEBUG("Notice that I can't \
							 use escape characters normally in these strings");

		pantheios::log_WARNING("The party is about to start");
}