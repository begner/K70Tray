// K70Tray.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "K70Tray.h"
#include "Constants.h"
#include "MainCorsairRGBK.h"
#include "KeyBoardDevice.h"
#include "RGB.h"
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
void				AppStart();
INT_PTR CALLBACK	Main(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void DebugMsg(string msg, ...);
vector<string> themeItems;
vector<string> layoutItems;
void addThemeToDropdown(string themename);
void addLayoutToDropdown(string layoutname);
void SetCurrentLayout(string layoutname);
void SetCurrentTheme(string layoutname);
DWORD GetDLLVersion(LPCTSTR lpszDllName);


// global window Handler
HWND				ghWnd;

// global instance
HINSTANCE			ghInst; 
HWND				ghDlgMain;

// Keyboard Hook Stuff...
HHOOK				hHook = 0;


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
MainCorsairRGBK		* mainCorsairRGBK; // Main App
KeyboardDevice		* keyBoardDevice; // Keyboard Device (Hardware stuff)
unsigned char		g_LEDState[K70_KEY_MAX][3], g_PrevLEDState[144][3]; // Ledstates and Previous LedStates



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
	wstring wmsg(smsg.begin(), smsg.end());

	WCHAR * newText = (WCHAR*)wmsg.c_str();
	

	// get edit control from dialog
	HWND hwndOutput = GetDlgItem(ghDlgMain, IDC_APPMESSAGES);

	// get the current selection
	DWORD StartPos, EndPos;
	SendMessage(hwndOutput, EM_GETSEL, reinterpret_cast<WPARAM>(&StartPos), reinterpret_cast<WPARAM>(&EndPos));

	// move the caret to the end of the text
	int outLength = GetWindowTextLength(hwndOutput);
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
void SetCurrentTheme(string layoutname) {
	DebugMsg("Set LayoutBox to %s", layoutname.c_str());
	int index;
	for (vector<string>::iterator it = themeItems.begin(); it < themeItems.end(); it++) {
		if (*it == layoutname){
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
	DebugMsg("Set LayoutBox to %s", layoutname.c_str());
	int index;
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

	DebugMsg("App started...");

	// create MainClass
	mainCorsairRGBK = new MainCorsairRGBK();

	// Initialize LLV Keyboard Hook
	DebugMsg("Setup Keyboard Hook");
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, &LowLevelKeyboardProc, ghInst, NULL);
	if (hHook == NULL)
	{
		DebugMsg("Error - Could not set Keyboard Hook");
		return (INT_PTR)FALSE;
	}

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

	return TRUE;
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
	NOTIFYICONDATA niData;
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
	
	// Hide it (initialize)
	ShowWindow(ghDlgMain, SW_HIDE);
	UpdateWindow(ghDlgMain);

	// Lets Start...
	StartInstance();
	return true;
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
		InsertMenu(hMenu, position++, MF_BYPOSITION | MF_STRING, SYTEMTRAY_THEMESELECTBEGIN + (it - themeItems.begin()), (s2ws((*it)).c_str()));
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
	{
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
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
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

//
// Message handler for MainDLG Window
//
INT_PTR CALLBACK Main(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
		break;
	case WM_PAINT:
		hdc = BeginPaint(hDlg, &ps);
		EndPaint(hDlg, &ps);
		break;
	
	case WM_CTLCOLORSTATIC:
		// Make Transparent Backgrounds for everything but..
		// DebugMsg("WM_CTLCOLORSTATIC: %i %i", (HWND)lParam, GetDlgItem(ghDlgMain, IDC_APPMESSAGES));
		if ((HWND)lParam != GetDlgItem(ghDlgMain, IDC_APPMESSAGES)) {
			hdc = (HDC)wParam;
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			return (LRESULT)GetStockObject(NULL_BRUSH);
		}
		
		break;
	case WM_COMMAND:
		// Theme Select Change
		if (LOWORD(wParam) == IDC_THEMESELECT){
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				int selectedThemeIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				mainCorsairRGBK->ChangeTheme(themeItems[selectedThemeIndex]);
			}
		}
		// Layout Select Change
		if (LOWORD(wParam) == IDC_LAYOUTSELECT){
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				int savingLayoutIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				mainCorsairRGBK->ChangeTheme(layoutItems[savingLayoutIndex]);
			}
		}
		// Demo Button
		else if (LOWORD(wParam) == IDC_BUTTONABOUT){
			DialogBox(ghInst, MAKEINTRESOURCE(IDD_ABOUTBOX), ghWnd, About);
			UpdateWindow(hDlg);
		} 
		// Exit of Window (means hide)
		else if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			ShowWindow(ghDlgMain, SW_HIDE);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//
// Keyboard Hook -> forward to mainCorsairRGBK
//
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	mainCorsairRGBK->KeyboardHook(nCode, wParam, lParam);

	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

//
// Start the App -> forward to mainCorsairRGBK
//
void AppStart() 
{
	mainCorsairRGBK->AppStart();
}