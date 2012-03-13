#pragma once
#include "stdafx.h"
#include "logfile_rotation_example.h"

int main()
{
	try
	{
		logfile_rotation_example();
	}
	catch(exception &e)
	{
		cout << "Unknown exception: " << e.what() << endl;
		return 1;
	}
	return 0;
}