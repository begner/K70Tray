#pragma once

#include "Globals.h"
#include <setupapi.h>
#include <cfgmgr32.h>

class KeyboardDevice
{
private:
	INT PID_K70 = 0x1B13;
	INT PID_K95 = 0x1B11;
	string deviceName = "";
	HANDLE hDevice;
	bool IsMatchingDevice(wchar_t *pDeviceID, unsigned int uiVID, unsigned int uiPID, unsigned int uiMI);
	HANDLE GetDeviceHandle(unsigned int uiVID, unsigned int uiPID, unsigned int uiMI);
	
public:
	KeyboardDevice();
	~KeyboardDevice();
	void KeyboardDevice::initDevice();
	int KeyboardDevice::deviceFound();
	HANDLE KeyboardDevice::getHandle();
	string getDeviceName();
	

};

