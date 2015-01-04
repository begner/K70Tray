// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <cstdlib>
#include <locale>
#include <string>
#include <sstream>

#include <hidsdi.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#include <shellapi.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment (lib, "Setupapi.lib")
#pragma comment (lib, "hid.lib")



// TODO: reference additional headers your program requires here
