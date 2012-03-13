// Pantheios.cpp : Defines the entry point for the console application.
//
#pragma once
#include "stdafx.h"
#include "trivial.h"
#include "custom_inserter.h"

//Process identity appended to all log messages for this program
extern "C" const char PANTHEIOS_FE_PROCESS_IDENTITY[] = "LogToConsole";

//To use the custom backend example, uncomment the preprocessor definition in stdafx

int _tmain(int argc, _TCHAR* argv[])
{
    try
    {
        pantheios::log(pantheios::debug, "Entering main(", 
			pantheios::args(argc, argv, pantheios::args::arg0FileOnly), ")");

		trivial_example();
		custom_inserter_example();        

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

