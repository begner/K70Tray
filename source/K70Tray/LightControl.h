#ifndef LIGHTCONTROL_H
#define LIGHTCONTROL_H

#include "Constants.h"
#include "Globals.h"

using namespace std;

class LightControl
{
public:
	int getXYByKeyCode(unsigned char keycode, string position);
	void BuildMatrix();
	void BuildMatrixVK();
	int getLedAdressByKeyCode(unsigned char keycode);
	// int getXYByLed(int x, int y);
	int SetLedK70RGB(int led, int r, int g, int b);
	int SetXYK70RGB(int x, int y, int r, int g, int b);
};

#endif