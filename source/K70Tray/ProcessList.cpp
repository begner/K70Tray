#include "stdafx.h"
#include "ProcessList.h"


ProcessList::ProcessList()
{
}


ProcessList::~ProcessList()
{
}




string ProcessList::getActiveProcessName() {
	checkProcessChanged();
	return getNameByProcessId(activeProcessId);
}

bool ProcessList::checkProcessChanged() {
	DWORD newPid = determineActiveProcessId();
	// DebugMsg("checkProcessChanged to %i", newPid);
	if (activeProcessId != newPid) {
		activeProcessId = newPid;
		// DebugMsg("ActiveProcess Changed to %i", activeProcessId);
		return true;
	}
	return false;
}

DWORD ProcessList::determineActiveProcessId() {
	DWORD threadid = 0;
	HWND curWin = GetForegroundWindow();
	
	// DWORD procid;
	 GetWindowThreadProcessId(curWin, &threadid);
	// DebugMsg("threadid %i", procid);
	return threadid;
}


string ProcessList::getNameByProcessId(DWORD pid) {
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
			// DebugMsg("Process: '%s' found!", ws2s(process.szExeFile).c_str());
			if (process.th32ProcessID == pid) {
				return ws2s(process.szExeFile).c_str();
			}
		} while (Process32Next(snapshot, &process));
	}

	CloseHandle(snapshot);
	return "";
}

bool ProcessList::FindRunningProcess(string processName, DWORD curProcessId) {
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
			// DebugMsg("Process: '%s' found!", ws2s(process.szExeFile).c_str());
			if (ws2s(process.szExeFile) == processName && process.th32ProcessID != curProcessId) {
				return true;
			}
		} while (Process32Next(snapshot, &process));
	}

	CloseHandle(snapshot);
	

	return false;


	

}
