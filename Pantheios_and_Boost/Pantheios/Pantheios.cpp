// Pantheios.cpp : Defines the entry point for the console application.
//
#pragma once
#include "stdafx.h"

extern "C" const char PANTHEIOS_FE_PROCESS_IDENTITY[] = "LogToConsole";

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

