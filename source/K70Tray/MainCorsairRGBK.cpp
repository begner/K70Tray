#include "stdafx.h"
#include "MainCorsairRGBK.h"


extern			KeyboardDevice		* keyBoardDevice;
extern unsigned char		g_ledAdress[K70_KEY_MAX];
extern unsigned char		g_XYk[K70_COLS][K70_ROWS];
extern unsigned char		g_keyCodes[K70_KEY_MAX];
extern float				g_keySizes[K70_KEY_MAX];
extern string				g_keyNames[K70_KEY_MAX];
extern int					g_iInterval;
extern Theme				* currentTheme;
extern LightControl		* pLC;
extern unsigned char		g_LEDState[K70_KEY_MAX][3], g_PrevLEDState[144][3];
extern RGB					ledState[K70_ROWS][K70_COLS] = {};

MainCorsairRGBK::MainCorsairRGBK()
{
	DebugMsg("STARTED!");
	
}
MainCorsairRGBK::~MainCorsairRGBK()
{
}


void MainCorsairRGBK::AppStart()
{
	
	

	vector<string> allThemeFiles = GetXMLFiles(wstring(L"Theme.*.xml"));
	for (vector<string>::iterator it = allThemeFiles.begin(); it < allThemeFiles.end(); it++) {
		string themeName = (*it);
		themeName.replace(0, 6, "");
		themeName.replace(themeName.length()-4, 4, "");
		DebugMsg("Theme found: '%s'", themeName.c_str());
		addThemeToDropdown(themeName);
	}
	vector<string> allLayoutFiles = GetXMLFiles(wstring(L"Layout.*.xml"));

	for (vector<string>::iterator it = allLayoutFiles.begin(); it < allLayoutFiles.end(); it++) {
		string layoutName = (*it);
		layoutName.replace(0, 7, "");
		layoutName.replace(layoutName.length() - 4, 4, "");
		addLayoutToDropdown(layoutName);
		DebugMsg("Layout found: '%s'", layoutName.c_str());
	}


	currentTheme = new Theme();
	config = new K70XMLConfig(this);
	config->readConfig();
	
	ChangeLayout(config->getLayoutName());

	ResetKeyboard();
	SendLEDState();
	ChangeTheme(config->getThemeName());
	
	
	DebugMsg("Config ok...");
	
	
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

			SendLEDState();
		}
	}
		
}

void MainCorsairRGBK::refreshKeyboard()
{
	if (currentTheme->GetActiveMap() != NULL) {
		// printf("Active Map != NULL");
		currentTheme->Tick();
		for (int x = 0; x < K70_ROWS; x++)
		{
			for (int y = 0; y < K70_COLS; y++)
			{
				SetXYRGB(x, y, ledState[x][y].getR(), ledState[x][y].getG(), ledState[x][y].getB());
			}
		}
	}
}

int MainCorsairRGBK::SetLedRGB(int led, int r, int g, int b)
{
	return pLC->SetLedRGB(led, r, g, b);
}
int MainCorsairRGBK::SetXYRGB(int x, int y, int r, int g, int b)
{
	return pLC->SetXYRGB(x, y, r, g, b);
}

void MainCorsairRGBK::ResetKeyboard() {
	for (int x = 0; x < K70_ROWS; x++)
	{
		for (int y = 0; y < K70_COLS; y++)
		{
			SetXYRGB(x, y, 0, 0, 0);
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
	auto getrgb = [&] {
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
		pkt[i + 5] = getrgb();
	HidD_SetFeature(keyBoardDevice->getHandle(), pkt, 65);

	pkt[2]++;
	for (int i = 0; i < 0x3C; i++)
		pkt[i + 5] = getrgb();
	HidD_SetFeature(keyBoardDevice->getHandle(), pkt, 65);

	pkt[2]++;
	for (int i = 0; i < 0x3C; i++)
		pkt[i + 5] = getrgb();
	HidD_SetFeature(keyBoardDevice->getHandle(), pkt, 65);

	pkt[2]++;
	pkt[3] = 0x24;
	for (int i = 0; i < 0x24; i++)
		pkt[i + 5] = getrgb();
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

		// For reactive typing Enter & Alt keys fix

		if ((p->vkCode == 0xA4 || p->vkCode == 0xA5) && (p->flags == 0x20 || p->flags == 0x21))
		{
			wParam = WM_KEYDOWN;
		}
		if ((p->vkCode == 0xA4 || p->vkCode == 0xA5) && (p->flags == 0x80 || p->flags == 0x81))
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
			currentTheme->KeyDown(realCode);
			
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
			
			currentTheme->KeyUp(realCode);
		default:
			break;
		}
	}
	
}

void MainCorsairRGBK::ChangeLayout(string layoutName) {
	animateKeyBoard = false;
	DebugMsg("Change Layout to '%s'", layoutName.c_str());
	SetCurrentLayout(layoutName);
	config->parseLayout(layoutName.c_str());

	pLC->BuildMatrix();
	DebugMsg("Keyboard matrix setup.");
	pLC->BuildMatrixVK();
	DebugMsg("Virtual keyboard matrix setup.");
	ResetKeyboard();
	SendLEDState();
}

void MainCorsairRGBK::ChangeTheme(string themeName) {
	currentTheme->StopTheme();
	ResetKeyboard();
	SendLEDState();
	
	delete currentTheme;
	currentTheme = new Theme();

	animateKeyBoard = false;
	DebugMsg("Change Theme to '%s'", themeName.c_str());
	SetCurrentTheme(themeName);
	config->parseTheme(themeName.c_str());
	DebugMsg("Theme Parsing done!");
	currentTheme->ActivateMapByName(string("default"));
	animateKeyBoard = true;
}


vector<string> MainCorsairRGBK::GetXMLFiles(wstring filter)
{
	vector<string> themelist;

	wchar_t* cwd;
	cwd = _wgetcwd(NULL, 0);

	// MessageBox(NULL, (LPCWSTR)cwd, L"test", MB_OK);


	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;

	wchar_t sPath[2048];
	wchar_t sFileName[2048];
	
	const wchar_t *sDir = cwd;

	//Specify a file mask. *.* = We want everything! 
	wsprintf(sPath, wstring(wstring(L"%s\\") + filter).c_str(), sDir);

	if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
	{
		DebugMsg("Path not found: [%s]\n", sDir);
		return themelist;
	}

	do
	{
		//Find first file will always return "."
		//    and ".." as the first two directories. 
		if (wcscmp(fdFile.cFileName, L".") != 0
			&& wcscmp(fdFile.cFileName, L"..") != 0)
		{
			//Build up our file path using the passed in 
			//  [sDir] and the file/foldername we just found: 
			wsprintf(sPath, L"%s\\%s", sDir, fdFile.cFileName);
			wsprintf(sFileName, L"%s", fdFile.cFileName);
			
			//Is the entity a File or Folder? 
			if (fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
			{
				// DebugMsg("Directory: '%S'\n", fdFile.cFileName);
			//	ListDirectoryContents(sPath); //Recursion, I love it! 
			}
			else{
				wstring wfileName = wstring(sFileName);
				string sFilename = ws2s(sFileName);
				themelist.push_back(sFilename);
				// DebugMsg("File: '%S'\n", fdFile.cFileName);
			}

		}
	} while (FindNextFile(hFind, &fdFile)); //Find the next file. 

	FindClose(hFind); //Always, Always, clean things up! 

	return themelist;
}




