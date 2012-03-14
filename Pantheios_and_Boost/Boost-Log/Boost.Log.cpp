#pragma once
#include "stdafx.h"
#include "basic_usage_example.h"
#include "trivial_example.h"
#include "logfile_rotation_example.h"
#include "advanced_usage_example.h"
#include "asynchronous_logging_example.h"
#include "multiple_threads_example.h"
#include "multiple_files_example.h"
#include "settings_file_example.h"

//For some reason running the trivial logging example breaks the logfile 
//rotation example (and probably others), so for now we can only run one example at a time.

int main()
{
	try
	{
		cout << "Examples: " << endl;
		cout << "0: Basic usage" << endl;
		cout << "1: Trivial logging" << endl;
		cout << "2: Logfile rotation" << endl;
		cout << "3: Advanced usage" << endl;
		cout << "4: Asynchronous logging" << endl;
		cout << "5: Multiple threads" << endl;
		cout << "6: Multiple files" << endl;
		cout << "7: Settings file example" << endl;
		
		char request = '\0';

		while(request == '\0')
		{
			cin >> request;

			switch(request) {
			case '0':
				basic_usage_example();
				break;
			case '1':
				trivial_example();
				break;
			case '2':
				logfile_rotation_example();
				break;
			case '3':
				advanced_usage_example();
				break;
			case '4':
				asynchronous_logging_example();
				break;
			case '5':
				multiple_threads_example();
				break;
			case '6':
				multiple_files_example();
				break;
			case '7':
				settings_file_example();
				break;
			default:
				cout << "Not recognized";
				request = '\0';
				break;
			}
		}
	}
	catch(exception &e)
	{
		cout << "Unknown exception: " << e.what() << endl;
		return 1;
	}
	return 0;
}