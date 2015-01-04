#include "stdafx.h"
#include "LightControl.h"

extern unsigned char		g_LEDState[K70_KEY_MAX][3], g_PrevLEDState[K70_KEY_MAX][3];
extern unsigned char		g_XY[K70_COLS][K70_ROWS];
extern unsigned char		g_XYk[K70_COLS][K70_ROWS];
extern unsigned char		g_ledAdress[K70_KEY_MAX];
extern unsigned char		g_keyCodes[K70_KEY_MAX];
extern float				g_keySizes[K70_KEY_MAX];

int LightControl::getLedAdressByKeyCode(unsigned char keycode) {
	int ret;

	for (int i = 0; i < sizeof(g_keyCodes); i++)
	{
		if (g_keyCodes[i] == keycode) {
			ret = g_ledAdress[i];
		}
	}
	return ret;
}

int LightControl::getXYByKeyCode(unsigned char keycode, string position) {
	int ret;

	for (int y = K70_COLS; y--> 0;) // Y 0 = CTRL, Windowskey, Alt, space...
	{
		for (int x = 0; x < K70_ROWS; x++)
		{
			if (g_XYk[y][x] == keycode) {
				if (position == string("x")) {
					ret = x;
				}
				if (position == string("y")) {
					ret = y;
				}
			}
		}
	}
	return ret;
}





void LightControl::BuildMatrix()
{
	// Construct XY lookup table
	auto keys = g_ledAdress;
	auto sizes = g_keySizes;

	//for (int y = 0; y < 7; y++) // Y 0 = Brightness key, winlock & mute
	for (int y = K70_COLS; y--> 0;) // Y 0 = CTRL, Windowskey, Alt, space...
	{
		unsigned char key;
		int size = 0;

		for (int x = 0; x < K70_ROWS; x++)
		{
			if (size == 0)
			{
				auto sizef = *sizes++;
				if (sizef < 0)
				{
					float fsizef;
					fsizef = -sizef * 4;
					size = (int)fsizef;
					key = 255;
				}
				else
				{
					key = *keys++;
					float fsizef;
					fsizef = sizef * 4;
					size = (int)fsizef;
				}
			}

			g_XY[y][x] = key;
			size--;
		}

		if (*keys++ != 255 || *sizes++ != 0)
			return;
	}
}

void LightControl::BuildMatrixVK()
{
	// Construct XY lookup table
	auto keys = g_keyCodes;
	auto sizes = g_keySizes;

	//for (int y = 0; y < 7; y++) // Y 1 = F1, F2... 
	for (int y = K70_COLS; y--> 0;) // Y 1 = CTRL, Windowskey, Alt, space...
	{
		unsigned char key;
		int size = 0;

		for (int x = 0; x < K70_ROWS; x++)
		{
			if (size == 0)
			{
				auto sizef = *sizes++;
				if (sizef < 0)
				{
					float fsizef;
					fsizef = -sizef * 4;
					size = (int)fsizef;
					key = 255;
				}
				else
				{
					key = *keys++;
					float fsizef;
					fsizef = sizef * 4;
					size = (int)fsizef;
				}
			}

			g_XYk[y][x] = key;
			size--;
		}

		if (*keys++ != 255 || *sizes++ != 0)
			return;
	}
}

int LightControl::SetLedRGB(int led, int r, int g, int b)
{
	/*if (x < 0)
	x = 0;
	else if (x > 91)
	x = 91;

	if (y < 0)
	y = 0;
	else if (y > 6)
	y = 6;

	// Converts Y & X to led number
	int led = g_XY[y][x];
	*/
	if (r < 0)
		r = 0;
	else if (r > 7)
		r = 7;

	if (g < 0)
		g = 0;
	else if (g > 7)
		g = 7;

	if (b < 0)
		b = 0;
	else if (b > 7)
		b = 7;

	if (led < 0 || led >= 144)
		return 0;

	// Minus 7 reverts so r = 6 will be r = 1
	g_LEDState[led][0] = 7 - r;
	g_LEDState[led][1] = 7 - g;
	g_LEDState[led][2] = 7 - b;

	return 0;
}


int LightControl::SetXYRGB(int x, int y, int r, int g, int b)
{
	if (x < 0)
	x = 0;
	else if (x > 91)
	x = 91;

	if (y < 0)
	y = 0;
	else if (y > 6)
	y = 6;

	// Converts Y & X to led number
	int led = g_XY[y][x];

	return SetLedRGB(led, r, g, b);
}
