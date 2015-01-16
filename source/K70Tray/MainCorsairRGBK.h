#pragma once

#include "Globals.h"
#include "K70XMLConfig.h"
#include "KeyboardDevice.h"
#include "LightControl.h"
#include "Theme.h"
#include "convert.h"
#include "KeyboardPreview.h"
#include <shlobj.h>		// needed for SHGetFolderPath()


class MainCorsairRGBK
{
	
private:
	void refreshKeyboard();
	int SetLedK70RGB(int led, int r, int g, int b);
	int SetXYK70RGB(int x, int y, int r, int g, int b);
	K70XMLConfig * config;
	bool animateKeyBoard = false;
	void ResetKeyboard();
	KeyboardPreview * keyboardPreview = NULL;
	// void PaintPreview();
public:

	MainCorsairRGBK();
	~MainCorsairRGBK();
	void KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam);
	bool AppInit(bool justCheck);
	void AppStart(HDC winHdc, RECT* prc);
	void SendLEDState();
	vector<string> GetXMLFiles(wstring filter);
	K70XMLConfig * getConfig();
	void ChangeTheme(string themeName);
	void ChangeLayout(string layoutName);
	void PaintKeyboardState();
	void saveConfig();
};

