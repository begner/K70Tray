#include "stdafx.h"
#include "Keyboard.h"

using namespace std;

extern string				g_keyNames[K70_KEY_MAX];
extern unsigned char		g_keyCodes[K70_KEY_MAX];
extern float				g_keySizes[K70_KEY_MAX];


Keyboard::Keyboard()
{

}


vector <unsigned int> Keyboard::GetAllKeyCodes() {
	vector <unsigned int> ret;

	for (int i = 0; i < K70_KEY_MAX; i++) {
		ret.push_back(g_keyCodes[i]);
	}

	return ret;
}

vector <string> Keyboard::GetAllKeyNames() {
	vector <string> ret;

	for (int i = 0; i < K70_KEY_MAX; i++) {
		ret.push_back(g_keyNames[i]);
	}

	return ret;
}

bool Keyboard::KeynameExist(string keyName) {
	int keyCode = getCodeByName(keyName);
	return (keyCode != -1);
}

void Keyboard::KeyDown(unsigned int keycode) {
	// printf("Keyboard::KeyDown %X\n", keycode);
	pressedKeys.push_back(getNameByCode(keycode));
	RemoveDuplicatedOnPressedKeys();
	return;
}

void Keyboard::KeyUp(unsigned int keycode) {
	// printf("Keyboard::KeyUp %X\n", keycode);
	pressedKeys.erase(remove(pressedKeys.begin(), pressedKeys.end(), getNameByCode(keycode)), pressedKeys.end());
	RemoveDuplicatedOnPressedKeys();
	return;
}

void Keyboard::RemoveDuplicatedOnPressedKeys() {
	sort(pressedKeys.begin(), pressedKeys.end());
	pressedKeys.erase(unique(pressedKeys.begin(), pressedKeys.end()), pressedKeys.end());

	// printf("-------------------\n");
	// for (vector<unsigned int>::iterator it = pressedKeys.begin(); it != pressedKeys.end(); ++it) {
		// printf("  - Key Pressed: %i\n", *it);
	// }
}

string Keyboard::getNameByCode(unsigned int keycode) {
	string ret = string("");
	for (unsigned int i = 0; i < K70_KEY_MAX; i++) {
		if (g_keyCodes[i] == keycode) {
			ret = g_keyNames[i];
		}
	}
	return ret;
}

int Keyboard::getCodeByName(string keyname) {
	
	int ret = -1;
	for (unsigned int i = 0; i < K70_KEY_MAX; i++) {
		if (g_keyNames[i] == keyname) {
			
			ret = g_keyCodes[i];
		}
	}
	return ret;
}

float Keyboard::getSizeByCode(unsigned int keycode) {
	float ret = 0;
	for (unsigned int i = 0; i < K70_KEY_MAX; i++) {
		if (g_keyCodes[i] == keycode) {
			ret = g_keySizes[i];
		}
	}
	return ret;
}




vector<string> Keyboard::getAllPressedKeys() {
	return pressedKeys;
}