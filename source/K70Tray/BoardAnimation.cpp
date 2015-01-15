#include "stdafx.h"
#include "BoardAnimation.h"
#include "Theme.h"
#include "Keyboard.h"
#include "LightControl.h"

extern K70RGB					ledState[K70_COLS][K70_ROWS];
extern unsigned char		g_XYk[K70_ROWS][K70_COLS];
extern Theme				* currentTheme; // Current Theme
extern LightControl			* pLC; // Current Light Control


BoardAnimation::BoardAnimation(string name)
{
	boardAnimationName = name;
}

BoardAnimation::BoardAnimation()
{
	
}

BoardAnimation::~BoardAnimation()
{
	placedAnimation.clear();
}

void BoardAnimation::addBoardAnimation(int x, int y, vector<K70RGB> color) {
	string adress = getAdressOfAnimation(x, y);
	placedAnimation.insert(make_pair(adress, color));
}

string BoardAnimation::getName() {
	return boardAnimationName;
}
void BoardAnimation::setName(string name) {
	boardAnimationName = name;
}

string BoardAnimation::getAdressOfAnimation(int x, int y) {
	char xChar[10];
	_itoa_s(x, xChar, 10);
	char yChar[10];
	_itoa_s(y, yChar, 10);
	return string(xChar) + "/" + string(yChar);
}

int BoardAnimation::getPositionOfAnimationAdress(string adress, string coordinate) {
	size_t pos = adress.find("/");
	
	if (coordinate == string("x")) {
		string xS = adress.substr(0, pos);
		return atoi(xS.c_str());
	}
	else {
		string yS = adress.substr(pos+1);
		return atoi(yS.c_str());
	}
}

void BoardAnimation::startAt(int x, int y) {
	startX = x;
	startY = y;
	currentTick = 0;
	animationHasEnded = false;
}


void BoardAnimation::tick() {
	// DebugMsg("BoardAnimation Tick at %i with size of %i", currentTick, placedAnimation.size());
	int animationEndCount = 0;
	for (map <string, vector<K70RGB>>::iterator it = placedAnimation.begin(); it != placedAnimation.end(); ++it) {		
		int x = getPositionOfAnimationAdress(it->first, "x") * 4;
		int y = getPositionOfAnimationAdress(it->first, "y");
		// DebugMsg("BoardAnimation at %i x %i", startX + x, startY + y);
		// DebugMsg("currentTick %i", currentTick);

		int animOnX = startX + x;
		int animOnY = startY + y;

		// out of range?
		if ((animOnY < 0 || animOnY > K70_ROWS - 1) || (animOnX < 0 || animOnX > K70_COLS - 1)) {
			// set as ended - and continue to next item
			animationEndCount++;
			continue;
		}

		vector<K70RGB> * animation = &it->second;
		if (currentTick < animation->size()) {
			
			int keyCode = g_XYk[animOnY][animOnX];
			if (keyCode > 0 && keyCode < 255) {
				int realX = pLC->getXYByKeyCode(keyCode, "x");
				int realY = pLC->getXYByKeyCode(keyCode, "y");
				// if (currentTick == 0) {
					// DebugMsg("Board Animation on %i->%i %i->%i 0x%X->%s", animOnX, realX, animOnY, realY, keyCode, currentTheme->getKeyboard()->getNameByCode(keyCode).c_str());
				// }
			
				// DebugMsg("kc 0x%X, rx %i, ry %i:", keyCode, realX, realY);
				ledState[realX][realY] = ledState[realX][realY] + animation->at(currentTick);
			}
		}
		else {
			animationEndCount++;
		}
	}

	// DebugMsg("animationEndCount %i vs size %i ", animationEndCount, placedAnimation.size());
	if (animationEndCount >= placedAnimation.size()) {
		animationHasEnded = true;
	}
	currentTick++;
}

bool BoardAnimation::animationEnded() {
	return animationHasEnded;
}

int BoardAnimation::getAnimationSize() {
	return placedAnimation.size();
}