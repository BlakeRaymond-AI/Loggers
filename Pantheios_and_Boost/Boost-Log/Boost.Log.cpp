#pragma once
#include "stdafx.h"
#include "trivial_example.h"
#include "logfile_rotation_example.h"

//For some reason running the trivial logging example breaks the logfile 
//rotation example, so for now we can only run one example at a time.

int main()
{
	try
	{
		cout << "Examples: " << endl;
		cout << "0: Trivial logging" << endl;
		cout << "1: Logfile rotation" << endl;
		
		char request = '\0';

		while(request == '\0')
		{
			cin >> request;

			switch(request) {
			case '0':
				trivial_example();
				break;
			case '1':
				logfile_rotation_example();
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