#pragma once
#include "Globals.h"
#include "Theme.h"
#include "LightControl.h"
#include "convert.h"
#include <Windows.h>

class KeyboardPreview
{
private:
	HFONT hFont;
	HDC winHdc;
	RECT* prc;
	int bufferWidth;
	int bufferHeight;
	HDC hdc;
	HBITMAP mbmp;
	HBITMAP moldbmp;
	HBRUSH backGroundBrush;
	HBRUSH keyColorBrush;
	HPEN keyColorPen;
	RECT backgroundRect;
	RECT keyRect;
	RECT textRect;
	RECT sTextRect;
	string sKeyName;
	wstring wsKeyName;
	WCHAR * wcKeyName;
	int pixelsizeX = 6;
	int pixelsizeY = pixelsizeX * 4;
	int zoom = 5;
	int colorCorrect = 30;
	// string lastKeyName = "";
	// string curKeyName = "";
	int curKeySize = 1;

	int ySpace = 3;
	int xSpace = 1;

	int TextMarginLeft = 3;
	int TextMarginTop = 1;
	int TextMarginRight = 3;
	int TextMarginBottom = 1;
	int RowSpaces[K70_COLS];
	

public:
	KeyboardPreview(HDC winHdc, RECT* prc);
	~KeyboardPreview();
	void PaintKeyboardState();
};

