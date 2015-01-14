#include "stdafx.h"
#include "Theme.h"
#include "ThemeMap.h"


using namespace std;


Theme::Theme() {
	// DebugMsg("Theme Constructor!\n");
	activeMap = NULL;
}

Theme::~Theme() {
	allMaps.clear();
	keySwitchGroups.clear();
	syncGroups.clear();
	syncGroupTick.clear();

}


void Theme::setName(string themeName) {
	name = themeName;
}

string Theme::getName() {
	return name;
}


void Theme::KeyDown(unsigned int keycode) {
	// printf("Theme: KeyDown %i\n", keycode);
	
	
	kb.KeyDown(keycode);
	
	SwitchMapOnKeyChange();
	if (activeMap) {
		activeMap->KeyDown(keycode);
	}


	
}

void Theme::KeyUp(unsigned int keycode) {
	// printf("Theme: KeyUp %i\n", keycode);
	kb.KeyUp(keycode);
	
	if (activeMap) {
		SwitchMapOnKeyChange();
		activeMap->KeyUp(keycode);
	}

	
}



bool Theme::SwitchMapOnKeyChange() {
	return SwitchMapOnKeyChange(true);
}

// dont call directly! use SwitchMapOnKeyChange()
bool Theme::SwitchMapOnKeyChange(bool withSpecialKeys) {

	vector<string> keysPressed;
	keysPressed = kb.getAllPressedKeys();

	if (withSpecialKeys) {
		// caps lock?
		if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0) {
			keysPressed.push_back("CapsLockActivated");
		}

		// num lock?
		if ((GetKeyState(VK_NUMLOCK) & 0x0001) != 0) {
			keysPressed.push_back("NumLockActivated");
		}
	}


	sort(keysPressed.begin(), keysPressed.end());
	
	bool switchedMap = false;

	for (unordered_map<string, vector<string> >::iterator it = keySwitchGroups.begin(); it != keySwitchGroups.end(); ++it) {
		string combinedMapAndId = it->first;
		string mapname = combinedMapAndId.substr(0, combinedMapAndId.find("_-_-_-"));

		vector<string> keyMap = it->second;
		/*
		string keyPressString = "";
		for (vector<unsigned int>::iterator it = keysPressed.begin(); it != keysPressed.end(); ++it) {
			keyPressString = keyPressString + ", " + to_string(*it);
		}
		string keyMapString = "";
		for (vector<unsigned int>::iterator it = keyMap.begin(); it != keyMap.end(); ++it) {
			keyMapString = keyMapString + ", " + to_string(*it);
		}
		*/
		int sizeA = keysPressed.size();
		int sizeB = keyMap.size();

		if (sizeA == sizeB) {
			bool are_equal = true;
			for (int i = 0; i < sizeB; i++) {
				if (keysPressed[i] != keyMap[i])
				{
					are_equal = false;
					break;
				}
			}

			if (are_equal) {
				DebugMsg("KeyPresses are equal!");
				ActivateMapByName(mapname);
				switchedMap = true;
			}
		}
	}

	
	return switchedMap;
}


void Theme::AddKeySwitchGroup(string mapname, int groupId, vector<string>keyNamesOfGroup) {
	vector<string> keysOfGroupCodes;
	
	string myMapId = string(mapname + "_-_-_-" + to_string(groupId));
	
	// convert in codes...
	for (vector<string>::iterator it = keyNamesOfGroup.begin(); it != keyNamesOfGroup.end(); ++it) {
		keysOfGroupCodes.push_back(*it);
	}

	sort(keysOfGroupCodes.begin(), keysOfGroupCodes.end());
	keySwitchGroups.insert(make_pair(myMapId, keysOfGroupCodes));
}

Keyboard * Theme::getKeyboard() {
	Keyboard * keyptr = &kb;
	return keyptr;
}


void Theme::AddMap(ThemeMap map) {
	map.setTheme(this);
	allMaps.push_back(map);
}

void Theme::StopTheme() {
	activeMap = NULL;
}


ThemeMap * Theme::GetActiveMap() {
	return activeMap;
}

void Theme::StartTheme() {
	DebugMsg("Start Theme...");
	if (!SwitchMapOnKeyChange()) {
		ActivateMapByName("default");
	};
}

void Theme::ActivateMapByName(string mapname) {
	
	ThemeMap * myMap = GetMapByName(mapname);

	if (!myMap) {
		DebugMsg("WARNING: Map '%s' not found", mapname.c_str());
		return;
	}

	if (!activeMap) {
		myMap->Activate();
		activeMap = myMap;
	}
	else if (activeMap->getName() != mapname) {
		myMap->Activate();
		activeMap = myMap;
	}
	

}

ThemeMap * Theme::GetMapByName(string mapname) {
	ThemeMap myMap = allMaps.front();
	ThemeMap * myMapPtr = &myMap;

	for (vector<ThemeMap>::iterator it = allMaps.begin(); it != allMaps.end(); ++it) {
		if (it->getName() == mapname) {
			myMapPtr = &(*it);
		}
	}

	return myMapPtr;
}

void Theme::Tick() {
	/*
	if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0)
		DebugMsg("Caps Lock ON!");
	else
		DebugMsg("Caps Lock OFF!");
		*/
	// "Caps Lock ON!"
	if (activeMap != NULL) {
		
		TickSyncGroups();
		activeMap->Tick();
	}
}


void Theme::AddSyncGroup(string name, unsigned int duration) {
	// printf("Add SyncGroup '%s' with duration of %i\n", name.c_str(), duration);
	syncGroups.insert(make_pair(name, duration));
}

unsigned int Theme::getSyncGroupTick(string syncName) {
	return syncGroupTick[syncName];
}

void Theme::TickSyncGroups() {
	for (map <string, unsigned int>::iterator it = syncGroups.begin(); it != syncGroups.end(); ++it) {
		string syncName = it->first;
		unsigned int curtick = syncGroupTick[syncName];
		unsigned int limit = it->second - 1;
		if (curtick >= limit) {
			// printf("Sync Tick Group '%s' = 0!", syncName.c_str());
			syncGroupTick[syncName] = 0;
		}
		else {
			syncGroupTick[syncName] = syncGroupTick[syncName]++;
		}
		// printf("Sync '%s' is now on %i\n", syncName.c_str(), syncGroupTick[syncName]);
	}
}
/*
void Theme::setCapsLockMapName(string mapName) {
	DebugMsg("set CapsLock MapName %s", mapName.c_str());
	capsLockMapName = mapName;
}

void Theme::setNumLockMapName(string mapName) {
	DebugMsg("set NumLock MapName %s", mapName.c_str());
	numLockMapName = mapName;
}
*/
