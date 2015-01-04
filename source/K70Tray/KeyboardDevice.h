#pragma once

#include "Globals.h"
#include <setupapi.h>
#include <cfgmgr32.h>

class KeyboardDevice
{
private:
	HANDLE hDevice;
	bool IsMatchingDevice(wchar_t *pDeviceID, unsigned int uiVID, unsigned int uiPID, unsigned int uiMI);
	HANDLE GetDeviceHandle(unsigned int uiVID, unsigned int uiPID, unsigned int uiMI);
public:
	KeyboardDevice();
	~KeyboardDevice();
	int KeyboardDevice::deviceFound();
	HANDLE KeyboardDevice::getHandle();
	

};

