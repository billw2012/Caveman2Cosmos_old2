#include "CvGameCoreDLL.h"

#include "BetterBTSAI.h"

#ifdef LOG_AI
int gPlayerLogLevel = 0;
int gTeamLogLevel = 0;
int gCityLogLevel = 0;
int gUnitLogLevel = 0;
#endif

// AI decision making logging

void logBBAI(char* format, ... )
{
	static CRITICAL_SECTION	cLogAccessSection;
	static bool bLogAccessCritSecInitialized = false;

	if (GC.isXMLLogging())
	{
		if ( !bLogAccessCritSecInitialized )
		{
			InitializeCriticalSectionAndSpinCount(&cLogAccessSection,4000);
			bLogAccessCritSecInitialized = true;
		}

		EnterCriticalSection(&cLogAccessSection);

		static char buf[2048];
		_vsnprintf( buf, 2048-4, format, (char*)(&format+1) );
		gDLL->logMsg("BBAI.log", buf);

		//	Echo to debugger
		strcat(buf, "\n");
		OutputDebugString(buf);

		LeaveCriticalSection(&cLogAccessSection);
	}
}