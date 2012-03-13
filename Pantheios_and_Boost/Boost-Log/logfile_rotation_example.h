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
 * \brief  An example of logging into a rotating text file.
 *         See the library tutorial for expanded comments on this code.
 *         It may also be worthwhile reading the Wiki requirements page:
 *         http://www.crystalclearsoftware.com/cgi-bin/boost_wiki/wiki.pl?Boost.Logging
 */

// #define BOOST_LOG_DYN_LINK 1

//This function will create a file sink, configure it for file rotation, and
//write 10,000 log records.  
//The rotation size specifies the maximum number of characters to be recorded 
//to a file before creating a new one.  

#pragma once
#include "stdafx.h"

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace fmt = boost::log::formatters;
namespace keywords = boost::log::keywords;

enum { LOG_RECORDS_TO_WRITE = 10000 };

const int ROTATION_SIZE = 16384; //characters
const int MAX_STORED_FILESIZE = 16*1024*1024; //bytes
const int MIN_FREE_SPACE = 100*1024*1024; //bytes
const string ROTATED_FILES_DIR = "logs/rotation-example";


void logfile_rotation_example()
{
    try
    {
        // Create a text file sink
        typedef sinks::synchronous_sink< sinks::text_file_backend > file_sink;
        boost::shared_ptr< file_sink > sink(new file_sink(
            keywords::file_name = "%Y%m%d_%H%M%S_%5N.log",      // file name pattern
            keywords::rotation_size = ROTATION_SIZE             // rotation size, in characters
            ));

        // Set up where the rotated files will be stored
        sink->locked_backend()->set_file_collector(sinks::file::make_collector(
            keywords::target = ROTATED_FILES_DIR,               // where to store rotated files
            keywords::max_size = MAX_STORED_FILESIZE,           // maximum total size of the stored files, in bytes
            keywords::min_free_space = MIN_FREE_SPACE           // minimum free space on the drive, in bytes
            ));

        // Upon restart, scan the target directory for files matching the file_name pattern
        sink->locked_backend()->scan_for_files();

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

		cout << "The main log file is in the directory \"Pantheios_and_Boost\\Boost-Log\"" << endl;
		cout << "Rotating every " << ROTATION_SIZE << " characters" << endl;
		cout << "Max size of files in rotation directory: " << MAX_STORED_FILESIZE / (1024.0 * 1024) << " MiB" << endl;
		cout << "Min free disk space in rotation directory: " << MIN_FREE_SPACE / (1024.0 * 1024) << " MiB" << endl;

		cout << "Logging " << LOG_RECORDS_TO_WRITE << " logs..." << endl;

        // Do some logging
        src::logger lg;
        for (unsigned int i = 0; i < LOG_RECORDS_TO_WRITE; ++i)
        {
            BOOST_LOG(lg) << "Some log record";
        }
		
		cout << "Done" << endl;
		cout << "Rotated logs were stored in: " << "Pantheios_and_Boost/Boost-Log/" << ROTATED_FILES_DIR << endl;

    }
    catch (std::exception& e)
    {
        std::cout << "FAILURE: " << e.what() << std::endl;
    }
}