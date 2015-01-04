#include "stdafx.h"
#include "ProcessList.h"


ProcessList::ProcessList()
{
}


ProcessList::~ProcessList()
{
}


bool ProcessList::FindRunningProcess(string processName) {
	DWORD pid = 0;

	// Create toolhelp snapshot.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	ZeroMemory(&process, sizeof(process));
	process.dwSize = sizeof(process);

	// Walkthrough all processes.
	if (Process32First(snapshot, &process))
	{
		do
		{
			// Compare process.szExeFile based on format of name, i.e., trim file path
			// trim .exe if necessary, etc.
			DebugMsg("Process: '%s' found!", ws2s(process.szExeFile).c_str());
			if (processName == ws2s(process.szExeFile)) {
				return true;
			}
		} while (Process32Next(snapshot, &process));
	}

	CloseHandle(snapshot);
	

	return false;


	

}
