#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "Globals.h"
#include "Constants.h"
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class Keyboard
{

private:
	vector<string> pressedKeys;
	void RemoveDuplicatedOnPressedKeys();
public:
	Keyboard();
	void KeyDown(unsigned int keycode);
	void KeyUp(unsigned int keycode);
	vector<string> getAllPressedKeys();
	int getCodeByName(string keyname);
	string getNameByCode(unsigned int keycode);
	vector <string> GetAllKeyNames();
	vector <unsigned int> GetAllKeyCodes();
	bool KeynameExist(string keyName);
	float getSizeByCode(unsigned int keycode);
};

#endif