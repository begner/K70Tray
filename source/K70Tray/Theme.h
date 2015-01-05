#ifndef THEME_H
#define THEME_H

#include "RGB.h"
#include "Keyboard.h"
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <map>


using namespace std;

class ThemeMap;



class Theme
{
	
private: 
	Keyboard										kb;
	ThemeMap *										activeMap = NULL;
	vector<ThemeMap>								allMaps;
	unordered_map< string, vector<unsigned int> >	keySwitchGroups; // map+groupid, vector of keycodes
	void											SwitchMapOnKeyChange();
	vector<unsigned int>							getSwitchMapInvoledKeys();
	map <string, unsigned int> syncGroups;
	map <string, unsigned int> syncGroupTick;
	
	void TickSyncGroups();

public:

	Theme();
	~Theme();
	void KeyDown(unsigned int keycode);
	void KeyUp(unsigned int keycode);
	Keyboard * getKeyboard();
	void ActivateMapByName(string mapname);
	void StopTheme();
	ThemeMap * GetActiveMap();
	void AddMap(ThemeMap map);
	
	ThemeMap * GetMapByName(string mapname);
	void AddKeySwitchGroup(string mapname, int groupId, vector<string>keyNamesOfGroup);
	void Tick();
	void AddSyncGroup(string name, unsigned int duration);
	unsigned int getSyncGroupTick(string syncName);
	
};

#endif