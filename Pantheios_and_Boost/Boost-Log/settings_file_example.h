/*
 *          Copyright Andrey Semashev 2007 - 2010.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   main.cpp
 * \author Andrey Semashev
 * \date   26.04.2008
 *
 * \brief  An example of initializing the library from a settings file.
 *         See the library tutorial for expanded comments on this code.
 *         It may also be worthwhile reading the Wiki requirements page:
 *         http://www.crystalclearsoftware.com/cgi-bin/boost_wiki/wiki.pl?Boost.Logging
 */

// #define BOOST_ALL_DYN_LINK 1

#pragma once
#include "stdafx.h"
#include <boost/log/utility/init/from_stream.hpp>
#include "app_severity_levels.h"

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;

using boost::shared_ptr;

//  Global logger declaration
BOOST_LOG_DECLARE_GLOBAL_LOGGER(settings_file_ex_lg, src::severity_logger< >)

void try_logging()
{
    src::severity_logger< >& lg = get_settings_file_ex_lg();
    BOOST_LOG_SEV(lg, normal) << "This is a normal severity record";
    BOOST_LOG_SEV(lg, notification) << "This is a notification severity record";
    BOOST_LOG_SEV(lg, warning) << "This is a warning severity record";
    BOOST_LOG_SEV(lg, error) << "This is a error severity record";
    BOOST_LOG_SEV(lg, critical) << "This is a critical severity record";
}

void settings_file_example()
{
    try
    {
        // Open the file
        std::ifstream settings("settings.txt");
        if (!settings.is_open())
        {
            std::cout << "Could not open settings.txt file" << std::endl;
			return;
        }

        // Read the settings and initialize logging library
        logging::init_from_stream(settings);

        // Add some attributes
        boost::shared_ptr< logging::attribute > attr(new attrs::local_clock);
        logging::core::get()->add_global_attribute("TimeStamp", attr);

        // Try logging
        try_logging();

        // Now enable tagging and try again
        BOOST_LOG_SCOPED_THREAD_TAG("Tag", std::string, "TAGGED");
        try_logging();

    }
    catch (std::exception& e)
    {
        std::cout << "FAILURE: " << e.what() << std::endl;
    }
}
