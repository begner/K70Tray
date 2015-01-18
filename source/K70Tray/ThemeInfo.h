#pragma once

#include "Globals.h"
#include "Resource.h"
#include "Theme.h"
#include "convert.h"
#include <Windows.h>
#include "FileSystem.h"
#include <objidl.h>
#ifdef USE_GDIPULS
	#include <gdiplus.h>
#endif

using namespace std;
#ifdef USE_GDIPULS
	using namespace Gdiplus;
#endif

class ThemeInfo
{
private:
	HDC winHdc;
	RECT* prc;
	int bufferWidth;
	int bufferHeight;
	HDC hdc;
	HBITMAP mbmp;
	HBITMAP moldbmp;
	HBRUSH backGroundBrush;
	RECT backgroundRect;
	HFONT normalFont, headlineFont;
	RECT infoRect;
	INT	leftPosText = 300;
	int marginInfo = 10;
	HBITMAP themeImage = NULL;
	string oldThemeImageName = "";
	void loadThemeImage();
	void drawShadowText(string sText, RECT* rect, HFONT fontH, int formatOptions);

public:
	
	ThemeInfo(HDC winHdc, RECT* prc);
	~ThemeInfo();
	void drawInfo();
};

