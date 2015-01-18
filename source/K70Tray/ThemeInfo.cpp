#include "stdafx.h"
#include "ThemeInfo.h"

extern HINSTANCE			ghInst;
extern HWND					ghDlgMain;
extern Theme				* currentTheme;




ThemeInfo::ThemeInfo(HDC paintHdc, RECT* paintRect)
{
	winHdc = paintHdc,
	prc = paintRect;

	bufferWidth = prc->right - prc->left;
	bufferHeight = prc->bottom - prc->top;

	hdc = CreateCompatibleDC(winHdc);
	mbmp = CreateCompatibleBitmap(winHdc, bufferWidth, bufferHeight); // width, height, 1, bit_depth, NULL
	moldbmp = (HBITMAP)SelectObject(hdc, mbmp);

	normalFont = CreateFont(12, 0, 0, 0,
		FW_DONTCARE, // fnWeight
		FALSE, // fdwItalic
		FALSE, // fdwUnderline
		FALSE, // fdwStrikeOut
		DEFAULT_CHARSET, //  fdwCharSet
		OUT_OUTLINE_PRECIS, // fdwOutputPrecision
		CLIP_DEFAULT_PRECIS,  // fdwClipPrecision
		CLEARTYPE_QUALITY, // fdwQuality
		VARIABLE_PITCH, // fdwPitchAndFamily
		TEXT("Arial") // lpszFace
		);

	headlineFont = CreateFont(16, 0, 0, 0,
		FW_DONTCARE, // fnWeight
		FALSE, // fdwItalic
		FALSE, // fdwUnderline
		FALSE, // fdwStrikeOut
		DEFAULT_CHARSET, //  fdwCharSet
		OUT_OUTLINE_PRECIS, // fdwOutputPrecision
		CLIP_DEFAULT_PRECIS,  // fdwClipPrecision
		CLEARTYPE_QUALITY, // fdwQuality
		VARIABLE_PITCH, // fdwPitchAndFamily
		TEXT("Arial") // lpszFace
		);

	backGroundBrush = CreateSolidBrush(RGB(0, 0, 0));

	backgroundRect.top = 0;
	backgroundRect.left = 0;
	backgroundRect.right = bufferWidth;
	backgroundRect.bottom = bufferHeight;

	infoRect = backgroundRect;
	infoRect.left = leftPosText;
	infoRect.top = marginInfo;
	infoRect.right-= marginInfo;
	infoRect.bottom-= marginInfo;

}


ThemeInfo::~ThemeInfo()
{
	DeleteObject(headlineFont);
	DeleteObject(normalFont);
	DeleteObject(backGroundBrush);

	// Doublebuffer Cleanup
	SelectObject(hdc, moldbmp);
	DeleteObject(mbmp);
	DeleteDC(hdc);

	
}

void ThemeInfo::loadThemeImage() {
	string imageFileName = currentTheme->getImageFileName();
	if (imageFileName != string("")) {
		FileSystem fs;

		string cwd = fs.getCwd();
		string imagePath = cwd + imageFileName;
		DebugMsg("Loading Theme Image: '%s'", imagePath.c_str());

		wstring wImagePath = s2ws(imagePath);
		DeleteObject(themeImage);
		themeImage = (HBITMAP)LoadImage(ghInst, wImagePath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (themeImage == NULL) {
			DebugMsg(" - Warning: Theme image could not be loaded!");
		}
		else {
			// DebugMsg("themeImage was loaded!");
		}
	}
	else {
		if (themeImage != NULL) {
			DeleteObject(themeImage);
		}
		themeImage = NULL;
	}
}


void ThemeInfo::drawInfo() {
	
	DebugMsg("Update Theme Info Screen");
	if (currentTheme == NULL) {
		return;
	}
	if (!currentTheme->isReady()) {
		DebugMsg(" - Theme was not ready!");
		return;
	}
	
	if (oldThemeImageName != currentTheme->getImageFileName()) {
		loadThemeImage();
	}
	
	oldThemeImageName = currentTheme->getImageFileName();
	
	FillRect(hdc, &backgroundRect, backGroundBrush);
	
	// draw theme image
	if (themeImage != NULL) {

		HDC TIhdc = CreateCompatibleDC(hdc);
		HBITMAP TIBMold = (HBITMAP)SelectObject(TIhdc, themeImage);
		BitBlt(hdc, 0, 0, bufferWidth, bufferHeight, TIhdc, 0, 0, SRCCOPY);

		DeleteDC(TIhdc);
		DeleteObject(TIBMold);
	}

	#ifdef USE_GDIPULS
		Graphics g(hdc);
		SolidBrush blackBrush(Color((256/4*3)-1, 0, 0, 0));
		Rect infoRectGdi = rect2GdiRect(infoRect);
		g.FillRectangle(&blackBrush, infoRectGdi);
		g.ReleaseHDC(hdc);
	#else	
		FillRect(hdc, &infoRect, backGroundBrush);
	#endif

	

	RECT textRect = infoRect;
	textRect.top += 10;
	textRect.bottom -= 10;
	textRect.right -= 10;
	textRect.left += 10;

	drawShadowText(currentTheme->getName(), &textRect, headlineFont, DT_SINGLELINE);

	textRect.top += 0;
	drawShadowText("Author: " + currentTheme->getAuthor(), &textRect, normalFont, DT_SINGLELINE | DT_RIGHT);

	textRect.top = 40;
	drawShadowText(currentTheme->getDescription(), &textRect, normalFont, DT_EXPANDTABS | DT_WORDBREAK);

	// copy buffer into winHdc
	BitBlt(winHdc, 0, 0, bufferWidth, bufferHeight, hdc, 0, 0, SRCCOPY);
}

void ThemeInfo::drawShadowText(string sText, RECT* rect, HFONT fontH, int formatOptions) {
	wstring  wsText = utf8_decode(sText);
	WCHAR *  wcText = (WCHAR*)wsText.c_str();

	int tFormat = DT_TOP | DT_LEFT | formatOptions;

	SelectObject(hdc, fontH);
	SetBkMode(hdc, TRANSPARENT);

	// shadow text
	SetTextColor(hdc, 0x00000000);
	RECT shadowrect = (*rect);
	shadowrect.left += 1;
	shadowrect.top += 1;
	DrawText(hdc, wcText, -1, &shadowrect, tFormat);

	SetTextColor(hdc, 0x00FFFFFF);
	// TextOut(hdc, textX + 1, textY + 1, wcKeyName, wcsnlen_s(wcKeyName, 5));
	DrawText(hdc, wcText, -1, rect, tFormat);

}