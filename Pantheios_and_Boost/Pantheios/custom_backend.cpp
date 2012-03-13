#pragma once
#include "stdafx.h"
#include <pantheios/backend.h>

//Custom backends must implement the following interface:
//int pantheios_be_init(
//  char const* processIdentity
//, void*       reserved
//, void**      ptoken
//);
//void pantheios_be_uninit(
//  void* token
//);
//int pantheios_be_logEntry(
//  void*       feToken
//, void*       beToken
//, int         severity
//, char const* entry
//, size_t      cchEntry
//);

#ifdef USE_CUSTOM_BACKEND

//Backend initialization
PANTHEIOS_CALL(int) pantheios_be_init(	PAN_CHAR_T const* processIdentity,
										void* reserved,
										void** ptoken)

{
	*ptoken = strdup(processIdentity);
	return (NULL == *ptoken)
		? PANTHEIOS_INIT_RC_OUT_OF_MEMORY
		: PANTHEIOS_INIT_RC_SUCCESS;
}

//Backend uninitialization
PANTHEIOS_CALL(void) pantheios_be_uninit(void* token)
{
	free(token);
}

//Backend logging function
PANTHEIOS_CALL(int) pantheios_be_logEntry(	void* feToken,
											void* beToken,
											int severity,
											PAN_CHAR_T const* entry,
											size_t cchEntry)
{
	char const* processIdentity = (char const*)beToken;
	// "<processIdentity>[<severity-code>]: <message>"
	cout << processIdentity << '[' << severity << ']' << ": " << entry << endl;
	return 0;
}

#endif //USE_CUSTOM_BACKEND