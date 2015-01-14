// K70Tray.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "K70Tray.h"
#include "Constants.h"
#include "MainCorsairRGBK.h"
#include "KeyBoardDevice.h"
#include "K70RGB.h"
#include "ProcessList.h"
#include <stdarg.h>  
#include <string>
#include <iostream>
#include <windows.h>
#include <direct.h>
#include <usb.h>
#include <math.h>

#define PACKVERSION(major,minor) MAKELONG(minor,major)
#define MAX_LOADSTRING 100




// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
// INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK	SysmsgProc(int nCode, WPARAM wParam, LPARAM lParam);
void				AppStart();
INT_PTR CALLBACK	Main(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	ErrorStartup(HWND, UINT, WPARAM, LPARAM);
NOTIFYICONDATA		niData; 
void DebugMsg(string msg, ...);
vector<string> themeItems;
vector<string> layoutItems;
void addThemeToDropdown(string themename);
void addLayoutToDropdown(string layoutname);
void SetCurrentLayout(string layoutname);
void SetCurrentTheme(string layoutname);
DWORD GetDLLVersion(LPCTSTR lpszDllName);
void ApplicationEndCleanup();
ProcessList processList;

// global window Handler
HWND				ghWnd;

// global instance
HINSTANCE			ghInst; 
HWND				ghDlgMain;

// Keyboard Hook Stuff...
HHOOK				keyboardHook = 0;
HHOOK				sysmsgHook = 0;



// Some Global Exchange Vars... :(
Theme				* currentTheme; // Current Theme
LightControl		* pLC; // Current Light Control


unsigned char		g_XY[K70_COLS][K70_ROWS];	// XY Matrix of Led Numbers
unsigned char		g_XYk[K70_COLS][K70_ROWS];	// XY Matrix of KeyCodes
unsigned char		g_ledAdress[K70_KEY_MAX];	// Led Adress for each Key
unsigned char		g_keyCodes[K70_KEY_MAX];	// Key Codes for each Key
float				g_keySizes[K70_KEY_MAX];	// Key Sizes Array
string				g_keyNames[K70_KEY_MAX];	// Key Names Array
int					g_iInterval = K70_DEFAULT_INTERVAL; // Global Update Interval
MainCorsairRGBK		* mainCorsairK70RGBK; // Main App
KeyboardDevice		* keyBoardDevice; // Keyboard Device (Hardware stuff)
unsigned char		g_LEDState[K70_KEY_MAX][3];
unsigned char		g_PrevLEDState[144][3]; // Ledstates and Previous LedStates
K70RGB				ledState[K70_ROWS][K70_COLS] = {};


const int ID_TIMER = 1;

static HBITMAP hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_MAINBACKGROUND));


//
// INITIALISATION OF WIN32 APP
// 
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR    lpCmdLine, _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_K70TRAY, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_K70TRAY));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}


//
// Global Function - adds Message to Debug Window.
// Acts like printf:
// DebugMsg("Something: %i = %s", myInt, myString);
// 
void DebugMsg(string msg, ...) {

	msg += "\n";
	char buffer[2048];
	va_list args;
	va_start(args, msg);
	vsprintf_s(buffer, msg.c_str(), args);
	perror(buffer);
	va_end(args);
	string smsg = buffer;

	/*
	SYSTEMTIME st;
	GetLocalTime(&st);
	char bufferTime[2048] = { '\0' };

	int hour = (INT)st.wHour;
	int min = (INT)st.wMinute;
	int sec = (INT)st.wSecond;

	// buffer = smsg.c_str();
	sprintf_s(bufferTime, sizeof(bufferTime), string("%d:%d:d% " + smsg).c_str(), hour, min, sec);
	string smsgd = bufferTime;
	*/
	
	wstring wmsg = utf8_decode(smsg);

	WCHAR * newText = (WCHAR*)wmsg.c_str();
	
	// get edit control from dialog
	HWND hwndOutput = GetDlgItem(ghDlgMain, IDC_APPMESSAGES);

	// get the current selection
	DWORD StartPos, EndPos;
	SendMessage(hwndOutput, EM_GETSEL, reinterpret_cast<WPARAM>(&StartPos), reinterpret_cast<WPARAM>(&EndPos));

	// move the caret to the end of the text
	DWORD outLength = GetWindowTextLength(hwndOutput);
	SendMessage(hwndOutput, EM_SETSEL, outLength, outLength);

	// insert the text at the new caret position
	SendMessage(hwndOutput, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(newText));

	// restore the previous selection
	SendMessage(hwndOutput, EM_SETSEL, StartPos, EndPos);

	// SetFocus(GetDlgItem(ghDlgMain, IDC_FOCUSCATCH));

	// scoll to end
	SendMessage(hwndOutput, WM_VSCROLL, SB_BOTTOM, NULL);
	

	
}


//
// Adds a Themename to the Dropdown (and so to the contextmenu of tray)
//
void addThemeToDropdown(string themename)
{
	HWND hWndComboBox = GetDlgItem(ghDlgMain, IDC_THEMESELECT);
	themeItems.push_back(themename);
	SendMessage(hWndComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)s2ws(themename).c_str());
}


//
// Set current selected theme (in combobox)
//
void SetCurrentTheme(string themename) {
	DebugMsg("Set Themebox to '%s'", themename.c_str());
	int index = -1;
	for (vector<string>::iterator it = themeItems.begin(); it < themeItems.end(); it++) {
		if (*it == themename){
			index = it - themeItems.begin();
		}
	}
	DebugMsg("index %i", index);
	HWND hWndComboBox = GetDlgItem(ghDlgMain, IDC_THEMESELECT);
	SendMessage(hWndComboBox, CB_SETCURSEL, (WPARAM)index, (LPARAM)0);

}


//
// Adds a Layoutname to the Dropdown
//
void addLayoutToDropdown(string layoutname)
{
	HWND hWndComboBox = GetDlgItem(ghDlgMain, IDC_LAYOUTSELECT);
	layoutItems.push_back(layoutname);
	SendMessage(hWndComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)s2ws(layoutname).c_str());
}


//
// Set current selected layout (in combobox)
//
void SetCurrentLayout(string layoutname) {
	DebugMsg("Set LayoutBox to '%s'", layoutname.c_str());
	// DebugMsg("Size: '%i'", layoutItems.size());
	int index = -1;
	for (vector<string>::iterator it = layoutItems.begin(); it < layoutItems.end(); it++) {
		if (*it == layoutname){
			index = it - layoutItems.begin();
		}
	}
	
	DebugMsg("index %i", index);
	HWND hWndComboBox = GetDlgItem(ghDlgMain, IDC_LAYOUTSELECT);
	SendMessage(hWndComboBox, CB_SETCURSEL, (WPARAM)index, (LPARAM)0);
	
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_K70TRAY));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_K70TRAY);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
// Detects the Version of a dll
// from http://www.codeproject.com/Articles/322/Determining-the-version-number-of-the-Windows-syst
// 
DWORD GetDLLVersion(LPCTSTR lpszDllName)
{
	HINSTANCE hinstDll;
	DWORD dwVersion = 0;

	hinstDll = LoadLibrary(lpszDllName);

		if (hinstDll)
		{
			DLLGETVERSIONPROC pDllGetVersion;

			pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, "DllGetVersion");

			/*Because some DLLs may not implement this function, you
			*must test for it explicitly. Depending on the particular
			*DLL, the lack of a DllGetVersion function may
			*be a useful indicator of the version.
			*/

			if (pDllGetVersion)
			{
				DLLVERSIONINFO dvi;
				HRESULT hr;

				ZeroMemory(&dvi, sizeof(dvi));
				dvi.cbSize = sizeof(dvi);

				hr = (*pDllGetVersion)(&dvi);

				if (SUCCEEDED(hr))
				{
					dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
				}
			}

			FreeLibrary(hinstDll);
		}
	return dwVersion;
}

//
// After All initialisation was made - start the app right now :D
//
BOOL StartInstance() {

	
	DebugMsg("Search for CorsairHID.exe...");
	// detect active corsair CUE
	if (processList.FindRunningProcess("CorsairHID.exe", GetCurrentProcessId())) {
		DialogBox(ghInst, MAKEINTRESOURCE(IDD_CUERUNNING), ghWnd, ErrorStartup);
		return (INT_PTR)FALSE;
	};

	DebugMsg("Search for another instance of K70Tray.exe");
	// detects itself... :(
	if (processList.FindRunningProcess("K70Tray.exe", GetCurrentProcessId())) {
		DialogBox(ghInst, MAKEINTRESOURCE(IDD_K70RUNNING), ghWnd, ErrorStartup);
		return (INT_PTR)FALSE;
	};
	
	

	

	DebugMsg("App started...");

	// create MainClass
	mainCorsairK70RGBK = new MainCorsairRGBK();

	// Initialize LLV Keyboard Hook
	DebugMsg("Setup Keyboard Hook");
	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, &LowLevelKeyboardProc, ghInst, NULL);
	if (keyboardHook == NULL)
	{
		DebugMsg("Error - Could not set Keyboard Hook");
		return (INT_PTR)FALSE;
	}

	/*
	DebugMsg("Setup Sysmessage Hook");
	sysmsgHook = SetWindowsHookEx(WH_GETMESSAGE, &SysmsgProc, GetModuleHandle(NULL), NULL);
	if (sysmsgHook == NULL)
	{
		DebugMsg("Error - Could not set Sysmessage Hook");
		return (INT_PTR)FALSE;
	}
	*/


	// Detect Corsair Keyboard
	keyBoardDevice = new KeyboardDevice();
	if (keyBoardDevice->deviceFound() != 0) {
		DebugMsg("Device not found");
		return (INT_PTR)FALSE;
	}
	else {
		DebugMsg("Device found");
	}

	// Create Animation Thread (Wich starts the app)
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)AppStart, NULL, NULL, NULL);

	return (INT_PTR)TRUE;
}


LRESULT CALLBACK SysmsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (mainCorsairK70RGBK) {
		DebugMsg("SYSMSG!");
	}

	return CallNextHookEx(sysmsgHook, nCode, wParam, lParam);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	ghInst = hInstance; // Store instance handle in our global variable


	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ghWnd = hWnd;



	// Notification Icon (System Tray)
	// from http://www.codeproject.com/Articles/4768/Basic-use-of-Shell_NotifyIcon-in-Win32

	// zero the structure - note: Some Windows funtions
	// require this but I can't be bothered to remember
	// which ones do and which ones don't.
	
	ZeroMemory(&niData, sizeof(NOTIFYICONDATA));


	// get Shell32 version number and set the size of the
	// structure note: the MSDN documentation about this is
	// a little dubious(see bolow) and I'm not at all sure
	// if the code bellow is correct
	ULONGLONG ullVersion = GetDLLVersion(_T("Shell32.dll"));

	if (ullVersion >= MAKEDLLVERULL(6, 0, 0, 0))
		niData.cbSize = sizeof(NOTIFYICONDATA);

	else if (ullVersion >= MAKEDLLVERULL(5, 0, 0, 0))
		niData.cbSize = NOTIFYICONDATA_V2_SIZE;

	else niData.cbSize = NOTIFYICONDATA_V1_SIZE;

	// the ID number can be any UINT you choose and will
	// be used to identify your icon in later calls to
	// Shell_NotifyIcon
	niData.uID = IDI_K70TRAY;

	// state which structure members are valid
	// here you can also choose the style of tooltip
	// window if any - specifying a balloon window:
	// NIF_INFO is a little more complicated 
	niData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

	// load the icon note: you should destroy the icon
	// after the call to Shell_NotifyIcon
	niData.hIcon =
		(HICON)LoadImage(hInstance,
		MAKEINTRESOURCE(IDI_K70TRAY),
		IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),
		LR_DEFAULTCOLOR);

	// set the window you want to recieve event messages
	niData.hWnd = hWnd;

	// set the message to send
	// note: the message value should be in the
	// range of WM_APP through 0xBFFF
	niData.uCallbackMessage = SYTEMTRAY_NOTIFYICON;

	// Create Tray Icon
	Shell_NotifyIcon(NIM_ADD, &niData);

	
	// Create Main Window (Dialoge)
	ghDlgMain = CreateDialog(ghInst, MAKEINTRESOURCE(IDD_MAINWIN), ghWnd, Main);
	SendMessage(ghDlgMain, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(ghInst, MAKEINTRESOURCE(IDI_K70TRAY)));
	SendMessage(ghDlgMain, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(ghInst, MAKEINTRESOURCE(IDI_K70TRAY)));
	

	

	int ret = SetTimer(ghDlgMain, ID_TIMER, 30, NULL);
	if (ret == 0) {
		// MessageBox(hWnd, "Could not SetTimer()!", "Error", MB_OK | MB_ICONEXCLAMATION);
	}


	// Hide it (initialize)
	ShowWindow(ghDlgMain, SW_HIDE);
	UpdateWindow(ghDlgMain);

	// Lets Start...
	return StartInstance();
}

//
// Created and Show ContextMenu for Tray
//
void ShowContextMenu()
{
	int position = 0; // position of menuitem
	HMENU hMenu = CreatePopupMenu(); // menu

	InsertMenu(hMenu, position++, MF_BYPOSITION | MF_STRING, IDM_MAINWIN, (LPCWSTR)L"Open K70Tray");
	InsertMenu(hMenu, position++, MF_BYPOSITION | MF_STRING, IDM_ABOUT, (LPCWSTR)L"About");
	InsertMenu(hMenu, position++, MF_MENUBARBREAK, 0, NULL);

	// Adding Themes
	for (vector<string>::iterator it = themeItems.begin(); it < themeItems.end(); it++) {
		UINT uFlags = MF_BYPOSITION | MF_STRING;
		if ((*it) == currentTheme->getName()) {
			uFlags = uFlags | MF_CHECKED;
		}
		InsertMenu(hMenu, position++, uFlags, SYTEMTRAY_THEMESELECTBEGIN + (it - themeItems.begin()), (s2ws((*it)).c_str()));
	}
	InsertMenu(hMenu, position++, MF_MENUBARBREAK, 0, NULL);
	InsertMenu(hMenu, position++, MF_BYPOSITION | MF_STRING, IDM_EXIT, (LPCWSTR)L"Exit");

	// Show that thing!
	SetForegroundWindow(ghWnd);

	// get Mouse Position
	POINT pt;
	GetCursorPos(&pt);

	// Show Menu
	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_VERNEGANIMATION, pt.x, pt.y, 0, ghWnd, NULL);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
// Message handler for the Base-Window (which we do not use - but needed)
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	
	switch (message)
	{/*
	case WM_SYSCOMMAND:
		// DebugMsg("WndProc WM_SYSCOMMAND %X", LOWORD(wParam));

		
		if (LOWORD(wParam) == SC_MONITORPOWER){
			DebugMsg("WndProc SC_MONITORPOWER %X", lParam);
			currentTheme->StopTheme();
		}
		return 0;
		break;
		*/
	case SYTEMTRAY_NOTIFYICON:
		switch (lParam)
		{
		case WM_LBUTTONDBLCLK:
			ShowWindow(ghDlgMain, SW_RESTORE);
			break;
		case WM_RBUTTONDOWN:
		case WM_CONTEXTMENU:
			ShowContextMenu();
			break;
		}
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_MAINWIN:
			ShowWindow(ghDlgMain, SW_SHOW);
			break;
		case IDM_ABOUT:
			DialogBox(ghInst, MAKEINTRESOURCE(IDD_ABOUTBOX), ghWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			if (wmId >= SYTEMTRAY_THEMESELECTBEGIN && wmId < SYTEMTRAY_THEMESELECTBEGIN + themeItems.size())
			{
				int themeId = wmId - SYTEMTRAY_THEMESELECTBEGIN;
				DebugMsg("Switch Theme by ContextMenu to %i", themeId);
				mainCorsairK70RGBK->ChangeTheme(themeItems[themeId]);

			}	
			else
			{
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		ApplicationEndCleanup();
	
		break;
	//power management code here
	case WM_POWERBROADCAST:
		DebugMsg("WM_POWERBROADCAST %i %i", wParam, lParam);


		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//
// Message handler for about box.
// 
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


void ApplicationEndCleanup() {
	Shell_NotifyIcon(NIM_DELETE, &niData);
	PostQuitMessage(0);
}

//
// Message handler for about box.
// 
INT_PTR CALLBACK ErrorStartup(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			ApplicationEndCleanup();
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//
// Message handler for MainDLG Window
//
INT_PTR CALLBACK Main(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{


	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		/*
		case WM_SYSCOMMAND:
		{
			DebugMsg("WM_SYSCOMMAND %X", LOWORD(wParam));


			if (LOWORD(wParam) == SC_MONITORPOWER){
				DebugMsg("SC_MONITORPOWER %X", lParam);
				currentTheme->StopTheme();
			}
			return 0;
		}
		break;
		*/
	case WM_INITDIALOG:
		// DebugMsg("INITDIALOG!");
		// SendMessage(GetDlgItem(ghDlgMain, IDC_APPMESSAGES), EM_SETLIMITTEXT, (WPARAM)0, (LPARAM)0);
		return (INT_PTR)TRUE;
		break;
	case WM_TIMER:
	{
		mainCorsairK70RGBK->PaintKeyboardState();
		
		if (processList.checkProcessChanged()) {
			DebugMsg("Process Changed to: '%s'", processList.getActiveProcessName().c_str());
		}
	}
	break;
	case WM_SETFOCUS:
	case WM_KILLFOCUS:
	{
		/*
		DebugMsg("SET FOCUS");
		if (LOWORD(wParam) == IDC_THEMESELECT){
			DebugMsg("TO IDC_THEMESELECT");
		}
		*/
	}
	break;
	case WM_PAINT:
	{
		
		HDC hdcInst, hdcBitmap;
		PAINTSTRUCT ps;
		BITMAP bp;
		RECT r;

		
		hdcInst = BeginPaint(hDlg, &ps);

		// Create a memory device compatible with the above DC variable

		hdcBitmap = CreateCompatibleDC(hdcInst);

		// Select the new bitmap

		SelectObject(hdcBitmap, hBitmap);

		GetObject(hBitmap, sizeof(bp), &bp);

		// Get client coordinates for the StretchBlt() function

		GetClientRect(hDlg, &r);

		// stretch bitmap across client area

		BitBlt(hdcInst, 0, 0, bp.bmWidth, bp.bmHeight, hdcBitmap, 0, 0, SRCCOPY);

		// Cleanup

		DeleteDC(hdcBitmap);
		EndPaint(hDlg, &ps);
		
	}
		break;
	
	case WM_CTLCOLORSTATIC:
	{
		// Make Transparent Backgrounds for everything but..
		// DebugMsg("WM_CTLCOLORSTATIC: %i %i", (HWND)lParam, GetDlgItem(ghDlgMain, IDC_APPMESSAGES));
		if ((HWND)lParam != GetDlgItem(ghDlgMain, IDC_APPMESSAGES)) {
			HDC hdc = (HDC)wParam;
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			return (LRESULT)GetStockObject(NULL_BRUSH);
		}
	}
		break;
	case WM_COMMAND:
	{
		// Theme Select Change
		if (LOWORD(wParam) == IDC_THEMESELECT){
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				SetFocus(hDlg); 
				int selectedThemeIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				mainCorsairK70RGBK->ChangeTheme(themeItems[selectedThemeIndex]);
				
			}
			else if (HIWORD(wParam) == CBN_SETFOCUS) {
			
			}
		}
		// Layout Select Change
		if (LOWORD(wParam) == IDC_LAYOUTSELECT){
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				SetFocus(hDlg);
				int savingLayoutIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				mainCorsairK70RGBK->ChangeLayout(layoutItems[savingLayoutIndex]);
			}
			else if (HIWORD(wParam) == CBN_SETFOCUS) {
	
			}

		}
		// About
		else if (LOWORD(wParam) == IDC_BUTTONABOUT){
			DialogBox(ghInst, MAKEINTRESOURCE(IDD_ABOUTBOX), ghWnd, About);
			UpdateWindow(hDlg);
		} 
		// Clear Console
		else if (LOWORD(wParam) == IDC_CLEARCONSOLE){
			RECT rect;
			HWND hctrl = GetDlgItem(ghDlgMain, IDC_KEYBOARDPREVIEW);
			GetClientRect(hctrl, &rect);
			MapWindowPoints(hctrl, ghDlgMain, (POINT *)&rect, 2);
			InvalidateRect(ghDlgMain, &rect, TRUE);


			// InvalidateRect(GetDlgItem(ghDlgMain, IDC_KEYBOARDPREVIEW), NULL, FALSE);
			HWND hwndOutput = GetDlgItem(ghDlgMain, IDC_APPMESSAGES);
			SendMessage(hwndOutput, WM_SETTEXT, (WPARAM)0, LPARAM(L""));
			UpdateWindow(hDlg);
		}
		// Theme Reload
		else if (LOWORD(wParam) == IDC_THEMERELOAD){
			HWND hwndOutput = GetDlgItem(ghDlgMain, IDC_THEMESELECT);
			int selectedThemeIndex = SendMessage(hwndOutput, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
			mainCorsairK70RGBK->ChangeTheme(themeItems[selectedThemeIndex]);
		}
		// Exit of Window (means hide)
		else if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			ShowWindow(ghDlgMain, SW_HIDE);
			return (INT_PTR)TRUE;
		}
	}
		break;
	}
	return (INT_PTR)FALSE;
}

//
// Keyboard Hook -> forward to mainCorsairK70RGBK
//
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (mainCorsairK70RGBK) {
		mainCorsairK70RGBK->KeyboardHook(nCode, wParam, lParam);
	}
	

	return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

//
// Start the App -> forward to mainCorsairK70RGBK
//
void AppStart()
{
	RECT rcClient;
	HWND pArea = GetDlgItem(ghDlgMain, IDC_KEYBOARDPREVIEW);
	HDC hdc = GetDC(pArea);

	GetClientRect(pArea, &rcClient);
	mainCorsairK70RGBK->AppStart(hdc, &rcClient);

	ReleaseDC(pArea, hdc);
}