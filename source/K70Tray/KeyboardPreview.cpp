#include "stdafx.h"
#include "KeyboardPreview.h"

extern HINSTANCE			ghInst;
extern HWND					ghDlgMain;
extern Theme				* currentTheme;
extern unsigned char		g_XYk[K70_ROWS][K70_COLS];
extern LightControl			* pLC;
extern K70RGB				ledState[K70_COLS][K70_ROWS];


KeyboardPreview::KeyboardPreview(HDC paintHdc, RECT* paintRect)
{
	winHdc = paintHdc,
	prc = paintRect;

	hFont = CreateFont(12, 0, 0, 0,
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

	bufferWidth = prc->right - prc->left;
	bufferHeight = prc->bottom - prc->top;

	hdc = CreateCompatibleDC(winHdc);
	mbmp = CreateCompatibleBitmap(winHdc, bufferWidth, bufferHeight); // width, height, 1, bit_depth, NULL
	moldbmp = (HBITMAP)SelectObject(hdc, mbmp);

	backGroundBrush = CreateSolidBrush(RGB(0, 0, 0));
	keyColorPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));

	backgroundRect.top = 0;
	backgroundRect.left = 0;
	backgroundRect.right = bufferWidth;
	backgroundRect.bottom = bufferHeight;

	RowSpaces[K70_ROWS] = { 0 };
	RowSpaces[0] = 10;
	RowSpaces[1] = 3;
}


KeyboardPreview::~KeyboardPreview()
{
	DeleteObject(hFont);
	DeleteObject(backGroundBrush);
	
	DeleteObject(keyColorPen);
	// Doublebuffer Cleanup
	SelectObject(hdc, moldbmp);
	DeleteObject(mbmp);
	DeleteDC(hdc);
}


void KeyboardPreview::PaintKeyboardState() {
	
	FillRect(hdc, &backgroundRect, backGroundBrush);
	

	// if theme is active...
	if (currentTheme) if (currentTheme->GetActiveMap() != NULL) {

		int lastKeyCode = 255;
		int lastKeyBeginX = 0;
		for (int y = 0; y < K70_ROWS; y++)
		{

			for (int x = 0; x < K70_COLS; x++)
			{
				// keyCode holen
				int keyCode = g_XYk[y][x];

				// ist das ein neuer Key?
				if (lastKeyCode == keyCode) {
					continue;
				}


				// ein neues key hat angefangen - also text in den alten setzen!

				if (lastKeyCode != 255) {

					// key position
					long posX = long(lastKeyBeginX * curKeySize * pixelsizeX);
					long posY = long(y * curKeySize * pixelsizeY + y * ySpace);
					for (int i = 0; i < y; i++) {
						posY += RowSpaces[i];
					}

					// keysize
					int keyWidth = (x - lastKeyBeginX) * pixelsizeX - xSpace;

					keyRect.left = posX;
					keyRect.top = posY;
					keyRect.right = posX + keyWidth;
					keyRect.bottom = posY + pixelsizeY;

					// get led pos
					int realX = pLC->getXYByKeyCode(lastKeyCode, "x");
					int realY = pLC->getXYByKeyCode(lastKeyCode, "y");

					// draw key
					keyColorBrush = CreateSolidBrush(RGB(ledState[realX][realY].getR()*colorCorrect,
						ledState[realX][realY].getG()*colorCorrect,
						ledState[realX][realY].getB()*colorCorrect));

					SelectObject(hdc, keyColorPen);
					SelectObject(hdc, keyColorBrush);

					Rectangle(hdc, keyRect.left, keyRect.top, keyRect.right, keyRect.bottom);
					DeleteObject(keyColorBrush);

					// draw Text
					SelectObject(hdc, hFont);
					SetBkMode(hdc, TRANSPARENT);
					SetTextColor(hdc, 0x00000000);

					
					textRect.left = keyRect.left + TextMarginLeft;
					textRect.top = keyRect.top + TextMarginTop;
					textRect.right = keyRect.right - TextMarginRight;
					textRect.bottom = keyRect.bottom - TextMarginBottom;

					sKeyName = currentTheme->getKeyboard()->getNameByCode(lastKeyCode);
					wsKeyName = utf8_decode(sKeyName);
					wcKeyName = (WCHAR*)wsKeyName.c_str();

					int tFormat = DT_SINGLELINE | DT_BOTTOM | DT_LEFT;

					// shadow text
					SetTextColor(hdc, 0x00000000);
					sTextRect = textRect;
					sTextRect.left += 1;
					sTextRect.top += 1;
					sTextRect.right += 1;
					sTextRect.bottom += 1;
					DrawText(hdc, wcKeyName, -1, &sTextRect, tFormat);

					SetTextColor(hdc, 0x00FFFFFF);
					// TextOut(hdc, textX + 1, textY + 1, wcKeyName, wcsnlen_s(wcKeyName, 5));
					DrawText(hdc, wcKeyName, -1, &textRect, tFormat);

				}


				// key hat sich geändert..
				lastKeyBeginX = x;
				lastKeyCode = keyCode;


			}
		}

	}

	// copy buffer into winHdc
	BitBlt(winHdc, 0, 0, bufferWidth, bufferHeight, hdc, 0, 0, SRCCOPY);


	

}