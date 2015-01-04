#pragma once
#include "Globals.h"
#include "convert.h"
#include <Windows.h>
#include <string>
#include <TlHelp32.h>

class ProcessList
{
public:
	ProcessList();
	~ProcessList();
	bool FindRunningProcess(string processName);
};

