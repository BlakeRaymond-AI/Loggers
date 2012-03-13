#pragma once
#include "stdafx.h"

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
 * \brief  This example shows how to perform logging to several files simultaneously,
 *         with files being created on an attribute value basis - thread identifier in this case.
 *         In the example the application creates a number of threads and registers thread
 *         identifiers as attributes. Every thread performs logging, and the sink separates
 *         log records from different threads into different files.
 */

// #define BOOST_LOG_DYN_LINK 1

#pragma once
#include "stdafx.h"
#include <boost/thread/thread.hpp>
#include <boost/log/sinks/text_multifile_backend.hpp>

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace fmt = boost::log::formatters;
namespace keywords = boost::log::keywords;

using boost::shared_ptr;

enum
{
    MULTIFILE_THREAD_COUNT = 5,
    MULTIFILE_LOG_RECORDS_TO_WRITE = 10
};

// Global logger declaration
BOOST_LOG_DECLARE_GLOBAL_LOGGER(my_logger, src::logger_mt)

// This function is executed in a separate thread
void thread_foo()
{
    BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::thread::id, boost::this_thread::get_id());
    for (unsigned int i = 0; i < MULTIFILE_LOG_RECORDS_TO_WRITE; ++i)
    {
        BOOST_LOG(my_logger::get()) << "Log record " << i;
    }
}

void multiple_files_example()
{
    try
    {
        // Create a text file sink
        typedef sinks::synchronous_sink< sinks::text_multifile_backend > file_sink;
        boost::shared_ptr< file_sink > sink(new file_sink);

        // Set up how the file names will be generated
        sink->locked_backend()->set_file_name_composer(
            fmt::stream << "logs/" << fmt::attr< boost::thread::id >("ThreadID") << ".log");

        // Set the log record formatter
        sink->locked_backend()->set_formatter(
            fmt::format("%1%: [%2%] - %3%")
                % fmt::attr< unsigned int >("Line #")
                % fmt::date_time< boost::posix_time::ptime >("TimeStamp")
                % fmt::message()
            );

        // Add it to the core
        logging::core::get()->add_sink(sink);

        // Add some attributes too
        boost::shared_ptr< logging::attribute > attr(new attrs::local_clock);
        logging::core::get()->add_global_attribute("TimeStamp", attr);
        attr.reset(new attrs::counter< unsigned int >);
        logging::core::get()->add_global_attribute("Line #", attr);

        // Create threads and make some logs
        boost::thread_group threads;
        for (unsigned int i = 0; i < MULTIFILE_THREAD_COUNT; ++i)
            threads.create_thread(&thread_foo);

        threads.join_all();

    }
    catch (std::exception& e)
    {
        std::cout << "FAILURE: " << e.what() << std::endl;
    }
}