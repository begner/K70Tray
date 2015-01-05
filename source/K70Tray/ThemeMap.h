#ifndef THEMEMAP_H
#define THEMEMAP_H

#include "Constants.h"
#include "RGB.h"
#include "Globals.h"
#include "KeyColor.h"
#include "BoardAnimation.h"
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <utility>
#include <map>


using namespace std;
class Theme;


class ThemeMap
{
private:
	Theme*				parentTheme;
	RGB					baseColor;
	RGB					onPressColor;
	bool				onPressColorSet;
	string				myMapName;
	// vector<string>		defaultKeyNames;
	// vector<RGB>			defaultKeyColors;
	// vector<string>		onPressKeyNames;
	// vector<RGB>			onPressKeyColors;
	map <unsigned int, KeyColor> keyColorMap; 
	vector<BoardAnimation>	activeBoardAnimations;
	map <string, BoardAnimation> BoardAnimationMap;
public:
	ThemeMap();
	ThemeMap(string mapname);
	~ThemeMap();
	void KeyDown(unsigned int keycode);
	void KeyUp(unsigned int keycode);
	// void SetBaseColor(RGB baseColor);
	// void SetOnPressColor(RGB color);
	// RGB GetOnPressColor();
	// void AddKeyColorDefault(string keyName, RGB keyColor);
	// void AddKeyColorKeyPress(string key, RGB color);
	void AddColorToKeyMap(string keyName, string type, RGB color, unsigned int duration);
	void ClearKeyMapColors(string keyName, string type);
	void SetSyncNameToKeyMap(string keyName, string type, string syncName);
	void SetJoinMode(string keyName, string type, string joinMode);
	void SetBoardAnimationName(string keyName, string type, string boardAnimationNameName);
	void RemoveSync(string keyName, string type);
	void Activate();
	void setName(string mapname);
	string getName();
	void setMapping(string keyName, KeyColor keycolor);
	void initialize();
	void setTheme(Theme* theme);
	Theme* getTheme();
	void Tick();
	void addBoardAnimation(BoardAnimation ba);
};

#endif