#pragma once
#include "Globals.h"
#include "convert.h"
#include <Windows.h>
#include <string>
#include <TlHelp32.h>

class ProcessList
{
private:
	DWORD activeProcessId = -1;
	string getNameByProcessId(DWORD pid);
	
public:
	ProcessList();
	~ProcessList();
	bool FindRunningProcess(string processName, DWORD curProcessId);
	bool checkProcessChanged();
	DWORD determineActiveProcessId();
	string getActiveProcessName();
};

