/*
 *          Copyright Andrey Semashev 2007 - 2010.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   main.cpp
 * \author Andrey Semashev
 * \date   30.08.2009
 *
 * \brief  An example of asynchronous logging in multiple threads.
 */

// #define BOOST_LOG_DYN_LINK 1

#pragma once
#include "stdafx.h"
#include <functional>
#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/log/utility/record_ordering.hpp>

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace fmt = boost::log::formatters;
namespace keywords = boost::log::keywords;

using boost::shared_ptr;

enum
{
    ASYNC_LOG_RECORDS_TO_WRITE = 10000,
    ASYNC_THREAD_COUNT = 2
};

BOOST_LOG_DECLARE_GLOBAL_LOGGER(test_lg, src::logger_mt)

//! This function is executed in multiple threads
void thread_fun(boost::barrier& bar)
{
    // Wait until all threads are created
    bar.wait();

    // Here we go. First, identfy the thread.
    BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::thread::id, boost::this_thread::get_id());

    // Now, do some logging
    for (unsigned int i = 0; i < ASYNC_LOG_RECORDS_TO_WRITE; ++i)
    {
        BOOST_LOG(get_test_lg()) << "Log record " << i;
    }
}

void asynchronous_logging_example()
{
    try
    {
        // Open a rotating text file
        boost::shared_ptr< std::ostream > strm(new std::ofstream("test.log"));
        if (!strm->good())
            throw std::runtime_error("Failed to open a text log file");

		cout << "Logging " << ASYNC_LOG_RECORDS_TO_WRITE << " records on " << ASYNC_THREAD_COUNT << " threads." << endl;
		cout << "Log file: Pantheios_and_Boost/Debug/Boost-Log/test.log" << endl;
		cout << "Logging... ";

        // Create a text file sink
        typedef sinks::text_ostream_backend backend_t;
        typedef sinks::ordering_asynchronous_sink< backend_t > sink_t;
        boost::shared_ptr< sink_t > sink(new sink_t(
            boost::make_shared< backend_t >(),
            // We'll apply record ordering to ensure that records from different threads go sequentially in the file
            keywords::order = logging::make_attr_ordering("Line #", std::less< unsigned int >())));

        sink->locked_backend()->add_stream(strm);

        sink->locked_backend()->set_formatter(
            fmt::format("%1%: [%2%] [%3%] - %4%")
                % fmt::attr< unsigned int >("Line #")
                % fmt::date_time< boost::posix_time::ptime >("TimeStamp")
                % fmt::attr< boost::thread::id >("ThreadID")
                % fmt::message()
            );

        // Add it to the core
        logging::core::get()->add_sink(sink);

        // Add some attributes too
        boost::shared_ptr< logging::attribute > attr(new attrs::local_clock);
        logging::core::get()->add_global_attribute("TimeStamp", attr);
        attr.reset(new attrs::counter< unsigned int >);
        logging::core::get()->add_global_attribute("Line #", attr);

        // Create logging threads
        boost::barrier bar(ASYNC_THREAD_COUNT);
        boost::thread_group threads;
        for (unsigned int i = 0; i < ASYNC_THREAD_COUNT; ++i)
            threads.create_thread(boost::bind(&thread_fun, boost::ref(bar)));

        // Wait until all action ends
        threads.join_all();

        // Flush all buffered records
        sink->stop();
        sink->feed_records(boost::posix_time::seconds(0));

		cout << "done." << endl;
    }
    catch (std::exception& e)
    {
        std::cout << "FAILURE: " << e.what() << std::endl;
    }
}
