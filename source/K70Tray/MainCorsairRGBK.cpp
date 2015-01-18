#include "stdafx.h"
#include "MainCorsairRGBK.h"
#include "Resource.h"
#include <windows.h>

extern KeyboardDevice		* keyBoardDevice;
extern unsigned char		g_ledAdress[K70_KEY_MAX];
extern unsigned char		g_XYk[K70_ROWS][K70_COLS];
extern unsigned char		g_XY[K70_ROWS][K70_COLS];
extern unsigned char		g_keyCodes[K70_KEY_MAX];
extern float				g_keySizes[K70_KEY_MAX];
extern string				g_keyNames[K70_KEY_MAX];
extern int					g_iInterval;
extern Theme				* currentTheme;
extern LightControl			* pLC;
extern unsigned char		g_LEDState[K70_KEY_MAX][3], g_PrevLEDState[144][3];
extern K70RGB				ledState[K70_COLS][K70_ROWS];
extern HINSTANCE			ghInst;
extern HWND					ghDlgMain;
extern INT_PTR CALLBACK	ErrorStartup(HWND, UINT, WPARAM, LPARAM);
extern HWND				ghWnd;

MainCorsairRGBK::MainCorsairRGBK()
{
	DebugMsg("STARTED!");
	
}
MainCorsairRGBK::~MainCorsairRGBK()
{
}


bool MainCorsairRGBK::AppInit(bool justCheck)
{
	FileSystem fs;
	vector<string> allThemeFiles;
	vector<string> themeFolders = fs.GetFoldersInPath("Themes", true);
	for (vector<string>::iterator it = themeFolders.begin(); it < themeFolders.end(); it++) {
		string themeFolderName = (*it);
		string themeFileName = "Theme." + themeFolderName + ".xml";
		DebugMsg("ThemeFolder '%s' found...", themeFolderName.c_str());
		if (fs.FileExists(fs.getPath() + "//" + themeFolderName + "//"+themeFileName)) {
			allThemeFiles.push_back(themeFolderName);
			
		}
		else {
			DebugMsg("WARNING: Theme in Folder '%s' not found (should be named '%s')", themeFolderName.c_str(), themeFileName.c_str());
		}

	}

	vector<string> allLayoutFiles = fs.GetFilesInPath("Layouts", true, "Layout.*.xml");
	for (vector<string>::iterator it = allLayoutFiles.begin(); it < allLayoutFiles.end(); it++) {
		string layoutName = (*it);
		str_replace(layoutName, "Layout.", "");
		str_replace(layoutName, ".xml", "");
		*it = layoutName;

		DebugMsg("Layoutfile: %s", it->c_str());
	}

	
	if (allThemeFiles.size() < 1 && allLayoutFiles.size() < 1) {
		DebugMsg("Theme.*.xml or Layout.*.xml not found!");
		return false;
	}

	currentTheme = new Theme();

	config = new K70XMLConfig(this);
	if (!config->readConfig()) {
		DebugMsg("config.xml not found!");
		return false;
	}
	
	
	if (!justCheck) {
		for (vector<string>::iterator it = allThemeFiles.begin(); it < allThemeFiles.end(); it++) {
			string themeName = (*it);
			addThemeToDropdown(themeName);
		}

		for (vector<string>::iterator it = allLayoutFiles.begin(); it < allLayoutFiles.end(); it++) {
			string layoutName = (*it);
			addLayoutToDropdown(layoutName);
		}
	}
	
	return true;
}

void MainCorsairRGBK::AppStart(HDC keyboardPreviewHdc, RECT* keyboardPreviewRect, HDC themeInfoHdc, RECT* themeInfoRect) {
	ChangeLayout(config->getLayoutName());

	keyboardPreview = new KeyboardPreview(keyboardPreviewHdc, keyboardPreviewRect);
	themeInfo = new ThemeInfo(themeInfoHdc, themeInfoRect);

	ResetKeyboard();
	SendLEDState();
	ChangeTheme(config->getThemeName());
	
	DebugMsg("Config ok...");
	
	 // PaintPreview();

	int iDelta, iLastTick = GetTickCount(), iKey = 0;
		
	while (TRUE)
	{
		if (animateKeyBoard) {
			iDelta = GetTickCount() - iLastTick;
			// printf();
			if (iDelta < g_iInterval)
			{
				Sleep(1);
				continue;
			}
			// printf("Update Keyboard!\n", iDelta, g_iInterval);
			iLastTick = GetTickCount();

			refreshKeyboard();
			

			// RedrawWindow(ghDlgMain, NULL, NULL, RDW_INTERNALPAINT);
			SendLEDState();
			// 
		}
	}
		
}


void MainCorsairRGBK::PaintKeyboardState() {
	
	// DebugMsg("keyboardPreview");
	if (currentTheme && currentTheme->getName() != string("") && animateKeyBoard) {
		 keyboardPreview->PaintKeyboardState();
	}
	
}

void MainCorsairRGBK::refreshKeyboard()
{
	ThemeMap * am = currentTheme->GetActiveMap();
	if (am != NULL) {
		// printf("Active Map != NULL");
		if (!am->IsTickRunning())
		{
			currentTheme->Tick();
			for (int x = 0; x < K70_COLS; x++)
			{
				for (int y = 0; y < K70_ROWS; y++)
				{
					SetXYK70RGB(x, y, ledState[x][y].getR(), ledState[x][y].getG(), ledState[x][y].getB());
				}
			}
		}
	}
}

int MainCorsairRGBK::SetLedK70RGB(int led, int r, int g, int b)
{
	return pLC->SetLedK70RGB(led, r, g, b);
}
int MainCorsairRGBK::SetXYK70RGB(int x, int y, int r, int g, int b)
{
	return pLC->SetXYK70RGB(x, y, r, g, b);
}

void MainCorsairRGBK::ResetKeyboard() {
	for (int x = 0; x < K70_COLS; x++)
	{
		for (int y = 0; y < K70_ROWS; y++)
		{
			SetXYK70RGB(x, y, 0, 0, 0);
		}
	}
}

//==================================================================================================
// Send the current LED state to the keyboard.
// Code by http://www.reddit.com/user/fly-hard
//==================================================================================================
void MainCorsairRGBK::SendLEDState()
{
	// Check for changes since last time we sent the state
	if (!memcmp(g_PrevLEDState, g_LEDState, sizeof(g_LEDState)))
		return;
		
	memcpy(g_PrevLEDState, g_LEDState, sizeof(g_LEDState));

	unsigned char pkt[65] = { 0 };

	int p = 0;
	int cc = 0;
	auto getK70RGB = [&] {
		unsigned char b = g_LEDState[p][cc] | (g_LEDState[p + 1][cc] << 4);
		p += 2;
		if (p >= K70_KEY_MAX) {
			p = 0;
			cc++;
		}
		return b;
	};

	pkt[1] = 0x7F;
	pkt[2] = 0x01;
	pkt[3] = 0x3C;

	for (int i = 0; i < 0x3C; i++)
		pkt[i + 5] = getK70RGB();
	HidD_SetFeature(keyBoardDevice->getHandle(), pkt, 65);

	pkt[2]++;
	for (int i = 0; i < 0x3C; i++)
		pkt[i + 5] = getK70RGB();
	HidD_SetFeature(keyBoardDevice->getHandle(), pkt, 65);

	pkt[2]++;
	for (int i = 0; i < 0x3C; i++)
		pkt[i + 5] = getK70RGB();
	HidD_SetFeature(keyBoardDevice->getHandle(), pkt, 65);

	pkt[2]++;
	pkt[3] = 0x24;
	for (int i = 0; i < 0x24; i++)
		pkt[i + 5] = getK70RGB();
	memset(pkt + 0x24 + 5, 0, 65 - 0x24 - 5);
	HidD_SetFeature(keyBoardDevice->getHandle(), pkt, 65);

	memset(pkt, 0, 65);
	pkt[1] = 0x07;
	pkt[2] = 0x27;
	pkt[3] = 0;
	pkt[5] = 0xD8;
	HidD_SetFeature(keyBoardDevice->getHandle(), pkt, 65);

}

void MainCorsairRGBK::KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	
	int realCode = 0;

	if (nCode == HC_ACTION)
	{
		KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;

		

		//  DebugMsg("KeyboardHook x0%x x0%x %i", p->vkCode, p->flags, p->flags & KF_UP);

		// Enter & Alt keys fix
		if ((p->vkCode == 0xA4 || p->vkCode == 0xA5) && (p->flags == 0x20 || p->flags == 0x21))
		{
			wParam = WM_KEYDOWN;
		}
		if ((p->vkCode == 0xA4 || p->vkCode == 0xA5) && (p->flags == 0x80 || p->flags == 0x81))
		{
			wParam = WM_KEYUP;
		}

		// strg (hold), alt(hold) strg(relase) <- not detected!
		if ((p->vkCode == 0xA2) && (p->flags == 0xA0))
		{
			wParam = WM_KEYUP;
		}
		// Same on right 
		if ((p->vkCode == 0xA3) && (p->flags == 0xA1))
		{
			wParam = WM_KEYUP;
		}

				

		switch (wParam)
		{
		case WM_KEYDOWN:

			if (p->vkCode == 0xD && p->flags == 0x1) {
				realCode = p->vkCode + 1;
			}
			else {
				realCode = p->vkCode;
			}
			if (currentTheme) {
				currentTheme->KeyDown(realCode);
			}
			
			// DebugMsg("KeyDown: 0x%x", realCode);

			break;
		case WM_KEYUP:

			if (p->vkCode == 0xD && p->flags == 0x81) {
				realCode = p->vkCode + 1;
			}
			else {
				realCode = p->vkCode;
			}

			// DebugMsg("KeyUp: 0x%x", realCode);
			if (currentTheme) {
				currentTheme->KeyUp(realCode);
			}
		default:
			break;
		}
	}
	
}

void MainCorsairRGBK::ChangeLayout(string layoutName) {
	animateKeyBoard = false;
	ResetKeyboard();
	SendLEDState();

	DebugMsg("Change Layout to '%s'", layoutName.c_str());
	SetCurrentLayout(layoutName);
	if (!config->parseLayout(layoutName.c_str())) {
		DebugMsg("WARNING: Errors at loading Layout file. Check warnings above.");
		currentTheme = NULL;
	};

	pLC->BuildMatrix();
	DebugMsg("Keyboard matrix setup.");
	/*
	for (int y = K70_ROWS; y-- > 0;) // Y 1 = CTRL, Windowskey, Alt, space...
	{
		DebugMsg(int_array_to_string(g_XY[y], sizeof(g_XY[y]), 10));
	}
	*/

	pLC->BuildMatrixVK();
	DebugMsg("Virtual keyboard matrix setup.");
	/*
	for (int y = K70_ROWS; y-- > 0;) // Y 1 = CTRL, Windowskey, Alt, space...
	{
		DebugMsg(int_array_to_string(g_XYk[y], sizeof(g_XYk[y]), 10));
	}
	*/

	if (currentTheme) {
		if (currentTheme->getName() != string("")) {
			DebugMsg("Restart Theme: '%s'", currentTheme->getName().c_str());
			animateKeyBoard = true;
			currentTheme->StartTheme();
		}
	}
	

	

}

void MainCorsairRGBK::saveConfig() {
	config->saveConfig();
}

K70XMLConfig * MainCorsairRGBK::getConfig() {
	return config;
}

void MainCorsairRGBK::ChangeTheme(string themeName) {
	currentTheme->StopTheme();
	ResetKeyboard();
	SendLEDState();
	animateKeyBoard = false;
	delete currentTheme;
	currentTheme = new Theme();
	currentTheme->setName(themeName);
	DebugMsg("Change Theme to '%s'", themeName.c_str());
	SetCurrentTheme(themeName);
	config->parseTheme(themeName.c_str());
	themeInfo->drawInfo();

	DebugMsg("Theme Parsing done!");
	currentTheme->StartTheme();
	animateKeyBoard = true;
}

void MainCorsairRGBK::UpdateThemeInfo() {
	if (themeInfo) {
		themeInfo->drawInfo();
	}
	
}

