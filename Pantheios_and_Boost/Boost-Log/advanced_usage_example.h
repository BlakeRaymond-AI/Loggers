/*
 *          Copyright Andrey Semashev 2007 - 2010.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   main.cpp
 * \author Andrey Semashev
 * \date   11.11.2007
 *
 * \brief  An example of in-depth library usage. See the library tutorial for expanded
 *         comments on this code. It may also be worthwhile reading the Wiki requirements page:
 *         http://www.crystalclearsoftware.com/cgi-bin/boost_wiki/wiki.pl?Boost.Logging
 */

// #define BOOST_LOG_USE_CHAR
// #define BOOST_ALL_DYN_LINK 1
// #define BOOST_LOG_DYN_LINK 1

#pragma once
#include "stdafx.h"
#include <boost/log/utility/empty_deleter.hpp>
#include <boost/log/utility/scoped_attribute.hpp>
#include "app_severity_levels.h"

namespace logging = boost::log;
namespace fmt = boost::log::formatters;
namespace flt = boost::log::filters;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;

using boost::shared_ptr;

int foo(src::logger& lg)
{
    BOOST_LOG_FUNCTION();
    BOOST_LOG(lg) << "foo is being called";
    return 10;
}

void advanced_usage_example()
{
    // This is a in-depth tutorial/example of Boost.Log usage

    // The first thing we have to do to get using the library is
    // to set up the logging sinks - i.e. where the logs will be written to.
    // Each sink is composed from frontend and backend. Frontend deals with
    // general sink behavior, like filtering (see below) and threading model.
    // Backend implements formatting and, actually, storing log records.
    // Not all frontend/backend combinations are compatible (mostly because of
    // threading models incompatibilities), but if they are not, the code will
    // simply not compile.

    // For now we only create a text output sink:
    typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
    boost::shared_ptr< text_sink > pSink(new text_sink);

    // Here synchronous_sink is a sink frontend that performs thread synchronization
    // before passing log records to the backend (the text_ostream_backend class).
    // The backend formats each record and outputs it to one or several streams.
    // This approach makes implementing backends a lot simplier, because you don't
    // need to worry about multithreading.

    {
        // The good thing about sink frontends is that they are provided out-of-box and
        // take away thread-safety burden from the sink backend implementors. Even if you
        // have to call a custom backend method, the frontend gives you a convenient way
        // to do it in a thread safe manner. All you need is to acquire a locking pointer
        // to the backend.
        text_sink::locked_backend_ptr pBackend = pSink->locked_backend();

        // Now, as long as pBackend lives, you may work with the backend without
        // interference of other threads that might be trying to log.

        // Next we add streams to which logging records should be output
        boost::shared_ptr< ostream > pStream(&clog, logging::empty_deleter());
        pBackend->add_stream(pStream);

        // We can add more than one stream to the sink backend
        boost::shared_ptr< ofstream > pStream2(new ofstream("sample.log"));
        assert(pStream2->is_open());
        pBackend->add_stream(pStream2);
    }

    // Ok, we're ready to add the sink to the logging library
    logging::core::get()->add_sink(pSink);

    // Now our logs will be written both to the console and to the file.
    // Let's do a quick test and output something. We have to create a logger for this.
    src::logger lg;

    // And output...
    BOOST_LOG(lg) << "Hello, World!";

    // Nice, huh? That's pretty much equivalent to writing the string to both the file
    // and the console. Now let's define the different way of formatting log records.
    // Each logging record may have a number of attributes in addition to the
    // message body itself. By setting up formatter we define which of them
    // will be written to log and in what way they will look there.
    pSink->locked_backend()->set_formatter(fmt::stream
        << fmt::attr("LineID") // First an attribute "LineID" is written to the log
        << " [" << fmt::date_time< boost::posix_time::ptime >("TimeStamp", "%d.%m.%Y %H:%M:%S.%f")
        << "] [" << fmt::attr< severity_level >("Severity", nothrow)
        << "] [" << fmt::time_duration< boost::posix_time::time_duration >("Uptime")
        << "] [" // then this delimiter separates it from the rest of the line
        << fmt::if_(flt::has_attr("Tag"))
           [
               fmt::stream << fmt::attr< string >("Tag") // then goes another attribute named "Tag"
                                               // Note here we explicitly stated that its type
                                               // should be string. We could omit it just
                                               // like we did it with the "LineNumber", but in this case
                                               // library would have to detect the actual attribute
                                               // type in run time which has the following consequences:
                                               // - On the one hand, the attribute would have been output
                                               //   even if it has another type (not string).
                                               // - On the other, this detection does not come for free
                                               //   and will result in performance decrease.
                                               //
                                               // In general it's better you to specify explicitly which
                                               // type should an attribute have wherever it is possible.
                                               // You may specify an MPL sequence of types if the attribute
                                               // may have more than one type. And you will have to specify
                                               // it anyway if the library is not familiar with it (see
                                               // boost/log/type_dispatch/standard_types.hpp for the list
                                               // of the supported out-of-box types).
                << "] [" // yet another delimiter
           ]
        << fmt::named_scope("Scope", keywords::iteration = fmt::reverse) << "] "
        << fmt::message()); // here goes the log record text

/*
    // There is an alternative way of specifying formatters
    pSink->locked_backend()->set_formatter(
        fmt::format("%1% @ %2% [%3%] >%4%< Scope: %5%: %6%")
            % fmt::attr("LineID")
            % fmt::date_time< boost::posix_time::ptime >("TimeStamp", "%d.%m.%Y %H:%M:%S.%f")
            % fmt::time_duration< boost::posix_time::time_duration >("Uptime")
            % fmt::attr< string >("Tag")
            % fmt::named_scope("Scope", keywords::iteration = fmt::reverse, keywords::scope_depth = 2)
            % fmt::message());
*/

    // Now the sink will output in the following format:
    // 1 [Current time] [Tag value] Hello World!
    // The output will be the same for all streams we add to the sink. If you want something different,
    // you may create another sink for that purpose.

    // Now we're going to set up the attributes.
    // Remember that "LineID" attribute in the formatter? There is a counter
    // attribute in the library that increments or decrements the value each time
    // it is output. Let's create it with a starting value 1.
    boost::shared_ptr< logging::attribute > pCounter(new attrs::counter< unsigned int >(1));

    // Since we intend to count all logging records ever made by the application,
    // this attribute should clearly be global.
    logging::core::get()->add_global_attribute("LineID", pCounter);

    // And similarly add a time stamp
    boost::shared_ptr< logging::attribute > pTimeStamp(new attrs::local_clock());
    logging::core::get()->add_global_attribute("TimeStamp", pTimeStamp);

    // And an up time stopwatch
    BOOST_LOG_SCOPED_THREAD_ATTR("Uptime", attrs::timer);

    // Attributes may have two other scopes: thread scope and source scope. Attributes of thread
    // scope are output with each record made by the thread (regardless of the logger object), and
    // attributes of the source scope are output with each record made by the logger. On output
    // all attributes of global, thread and source scopes are merged into a one record and passed to
    // the sinks as one view. There is no difference between attributes of different scopes from the
    // sinks' perspective.

    // Let's also track the execution scope from which the records are made
    boost::shared_ptr< logging::attribute > pNamedScope(new attrs::named_scope());
    logging::core::get()->add_thread_attribute("Scope", pNamedScope);

    // We can mark the current execution scope now - it's the 'main' function
    BOOST_LOG_FUNCTION();

    // Let's try out the counter attribute and formatting
    BOOST_LOG(lg) << "Some log line with a counter";
    BOOST_LOG(lg) << "Another log line with the counter";

    // Ok, remember the "Tag" attribute we added in the formatter? It is absent in these
    // two lines above, so it is empty in the output. Let's try to tag some log records with it.
    {
        BOOST_LOG_NAMED_SCOPE("Tagging scope");

        // Here we add a temporary attribute to the logger lg.
        // Every log record being written in the current scope with logger lg
        // will have a string attribute "Tag" with value "Tagged line" attached.
        BOOST_LOG_SCOPED_LOGGER_TAG(lg, "Tag", string, "Tagged line");

        // The above line is roughly equivalent to the following:
        // attrs::constant< string > TagAttr("Tagged line");
        // logging::scoped_attribute _ =
        //     logging::add_scoped_logger_attribute(lg, "Tag", TagAttr);

        // Now these lines will be highlighted with the tag
        BOOST_LOG(lg) << "Some tagged log line";
        BOOST_LOG(lg) << "Another tagged log line";
    }

    // And this line is not highlighted anymore
    BOOST_LOG(lg) << "Now the tag is removed";

    // Now let's try to apply filtering to the output. Filtering is based on
    // attributes being output with the record. One of the common filtering use cases
    // is filtering based on the record severity level. We've already defined severity levels.
    // Now we can set the filter. A filter is essentially a functor that returns a
    // boolean value that tells whether to write the record or not.
    pSink->set_filter(
        flt::attr< severity_level >("Severity", nothrow) >= warning // Write all records with "warning" severity or higher
        || flt::attr< string >("Tag", nothrow).begins_with("IMPORTANT")); // ...or specifically tagged

    // The "attr" placeholder here acts pretty much like the "attr" placeholder in formatters, except
    // that it requires the attribute type (or types in MPL-sequence) to be specified.
    // In case of a single string or wstring type of attribute the "attr" placeholder
    // provides a number of extended predicates which include "begins_with", "ends_with", "contains"
    // and "matches" (the last one performs RegEx matching).
    // There are other placeholders to be used for filter composition in the "boost/log/filters"
    // directory. Additionally, you are not restricted to them and may provide your own filtering
    // functors.

    // It must be noted that filters may be applied on per-sink basis and/or globally.
    // Above we set a filter for this particular sink. Had we another sink, the filter would
    // not influence it. To set a global filter one should call the set_filter method of the
    // logging system like that:
    // logging::core::get()->set_filter(...);

    // Now, to set logging severity we could use our previously created logger "lg".
    // But no make it more convenient and efficient there is a special extended logger class.
    // Its implementation may serve as an example of extending basic library functionality.
    // You may add your specific capabilities to the logger by deriving your class from it.
    src::severity_logger< severity_level > slg;

    // These two lines test filtering based on severity
    BOOST_LOG_SEV(slg, normal) << "A normal severity message, will not pass to the output";
    BOOST_LOG_SEV(slg, error) << "An error severity message, will pass to the output";

    {
        // Next we try if the second condition of the filter works
        // We mark following lines with a tag
        BOOST_LOG_SCOPED_THREAD_TAG("Tag", string, "IMPORTANT MESSAGES");

        // We may omit the severity and use the shorter BOOST_LOG macro. The logger "slg"
        // has the default severity that may be specified on its construction. We didn't
        // do it, so it is 0 by default. Therefore this record will have "normal" severity.
        // The only reason this record will be output is the "Tag" attribute we added above.
        BOOST_LOG(slg) << "Some really urgent line";
    }

    pSink->reset_filter();

    // And moreover, it is possible to nest logging records. For example, this will
    // be processed in the order of evaluation:
    BOOST_LOG(lg) << "The result of foo is " << foo(lg);

	cout << endl << "Log records written to Pantheios_and_Boost/Debug/Boost.Log.log" << endl;
}