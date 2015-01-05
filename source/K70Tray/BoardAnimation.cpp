#include "stdafx.h"
#include "BoardAnimation.h"
#include "Theme.h"
#include "LightControl.h"

extern RGB					ledState[K70_ROWS][K70_COLS];
extern unsigned char		g_XYk[K70_COLS][K70_ROWS];
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

void BoardAnimation::addBoardAnimation(int x, int y, vector<RGB> color) {
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
	for (map <string, vector<RGB>>::iterator it = placedAnimation.begin(); it != placedAnimation.end(); ++it) {		
		int x = getPositionOfAnimationAdress(it->first, "x") * 4;
		int y = getPositionOfAnimationAdress(it->first, "y");
		// DebugMsg("BoardAnimation at %i x %i", startX + x, startY + y);
		// DebugMsg("currentTick %i", currentTick);

		int animOnX = startX + x;
		int animOnY = startY + y;

		// out of range?
		if (animOnY < 0 && animOnY >= K70_COLS) {
			// DebugMsg("animOnY out of range %i", animOnY);
			continue;
		}
		if (animOnX < 0 && animOnX >= K70_ROWS) {
			// DebugMsg("animOnX out of range %i", animOnY);
			continue;
		}

		vector<RGB> * animation = &it->second;
		if (currentTick < animation->size()) {
			
			int keyCode = g_XYk[animOnY][animOnX];
			int realX = pLC->getXYByKeyCode(keyCode, "x");
			int realY = pLC->getXYByKeyCode(keyCode, "y");
			if (keyCode > 0 && keyCode < 255) {
				// DebugMsg("kc 0x%X, rx %i, ry %i:", keyCode, realX, realY);
				ledState[realX][realY] = ledState[realX][realY] + animation->at(currentTick);
			}
			

			// ledState[realX][realY] = animation->at(currentTick);
			/*
			// find nearest key
			

			for (int vx = -2; vx < 2; vx++) {

				int animOnY = startY + y;
				int animOnX = startX + vx + x;
				if (animOnX > 0 && animOnX <= K70_ROWS &&
					animOnY > 0 && animOnY <= K70_COLS) {
					DebugMsg("Found on %i, %i, %i", animOnY, animOnX, g_XYk[animOnY][animOnX]);
					ledState[animOnX][animOnY] = ledState[animOnX][animOnY] + animation->at(currentTick);
				}
				// DebugMsg("Set BoardAnimation %ix%i to %s ", startY + y, startX + vx + x, animation->at(currentTick).getRGBString().c_str());
				
			}
			*/


		}
		else {
			animationEndCount++;
		}
	}

	// DebugMsg("animationEndCount %i vs size %i ", animationEndCount, placedAnimation.size());
	if (animationEndCount == placedAnimation.size()) {
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