#pragma once

#include "Globals.h"
#include "K70XMLConfig.h"
#include "KeyboardDevice.h"
#include "LightControl.h"
#include "Theme.h"
#include "convert.h"
#include <shlobj.h>		// needed for SHGetFolderPath()


class MainCorsairRGBK
{
	
private:
	void refreshKeyboard();
	int SetLedRGB(int led, int r, int g, int b);
	int SetXYRGB(int x, int y, int r, int g, int b);
	K70XMLConfig * config;
	bool animateKeyBoard = false;
	void ResetKeyboard();
public:

	MainCorsairRGBK();
	~MainCorsairRGBK();
	void KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam);
	void AppStart();
	void SendLEDState();
	vector<string> GetXMLFiles(wstring filter);
	void ChangeTheme(string themeName);
	void ChangeLayout(string layoutName);
};

