#ifndef THEMEMAP_H
#define THEMEMAP_H

#include "Constants.h"
#include "K70RGB.h"
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
	bool				isOverlay = false;
	bool				tickIsRunning = false;
	Theme*				parentTheme;
	K70RGB					baseColor;
	K70RGB					onPressColor;
	bool				onPressColorSet;
	string				myMapName;
	// vector<string>		defaultKeyNames;
	// vector<K70RGB>			defaultKeyColors;
	// vector<string>		onPressKeyNames;
	// vector<K70RGB>			onPressKeyColors;
	map <unsigned int, KeyColor> keyColorMap; 
	vector<BoardAnimation>	activeBoardAnimations;
	map <string, BoardAnimation> BoardAnimationMap;
public:
	ThemeMap();
	ThemeMap(string mapname);
	~ThemeMap();
	void KeyDown(unsigned int keycode);
	void KeyUp(unsigned int keycode);
	// void SetBaseColor(K70RGB baseColor);
	// void SetOnPressColor(K70RGB color);
	// K70RGB GetOnPressColor();
	// void AddKeyColorDefault(string keyName, K70RGB keyColor);
	// void AddKeyColorKeyPress(string key, K70RGB color);
	void AddColorToKeyMap(string keyName, string type, K70RGB color, unsigned int duration);
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
	bool IsTickRunning();
	void SetOverlay(bool state);
	bool GetOverlay();
};

#endif