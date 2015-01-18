#ifndef THEME_H
#define THEME_H

#include "K70RGB.h"
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
	bool											themeIsReady = false;
	string											author = "";
	string											description = "";
	string											imageFileName = "";
	string											name;
	Keyboard										kb;
	ThemeMap *										activeMap = NULL;
	string											activeMapName = "";
	vector<ThemeMap>								allMaps;
	unordered_map< string, vector<string> >			keySwitchGroups; // map+groupid, vector of keycodes
	unordered_map< string, vector<string> >			overlaySwitchGroups;
	bool											SwitchMapOnKeyChange();
	bool											SwitchMapOnKeyChange(bool withSpecialKeys); // dont call directly! use SwitchMapOnKeyChange()
	vector<unsigned int>							getSwitchMapInvoledKeys();
	map <string, unsigned int> syncGroups;
	map <string, unsigned int> syncGroupTick;
	bool											capsLockInUse = false;
	bool											numLockInUse = false;
	void checkSpecialKeysInUse(string keyName);
	void TickSyncGroups();
	// string capsLockMapName = "";
	// string numLockMapName = "";
public:

	Theme();
	void setName(string name);
	bool isReady();
	void setReady(bool state);
	string getName();
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
	void AddOverlaySwitch(string mapname, int groupId, vector<string>keyNamesOfGroup);
	void Tick();
	void AddSyncGroup(string name, unsigned int duration);
	unsigned int getSyncGroupTick(string syncName);
	void StartTheme();
	void setInfo(string auth, string desc, string img);
	// void setCapsLockMapName(string mapName);
	// void setNumLockMapName(string mapName);

	string getAuthor();
	string getDescription();
	string getImageFileName();

};

#endif