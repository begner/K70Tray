#include "stdafx.h"
#include "Theme.h"
#include "ThemeMap.h"


using namespace std;


Theme::Theme() {
	// DebugMsg("Theme Constructor!");
	activeMap = NULL;
	activeMapName = "";
}

Theme::~Theme() {
	allMaps.clear();
	keySwitchGroups.clear();
	syncGroups.clear();
	syncGroupTick.clear();
	overlaySwitchGroups.clear();
	activeMapName = "";
	activeMap = NULL;
}

bool Theme::isReady() {
	return themeIsReady;
}

void Theme::setReady(bool state) {
	themeIsReady = state;
}

void Theme::setName(string themeName) {
	name = themeName;
}

string Theme::getName() {
	return name;
}

void Theme::setInfo(string auth, string desc, string img) {
	author = auth;
	description = desc;
	imageFileName = img;
}

string Theme::getAuthor() {
	return author;
}

string Theme::getDescription() {
	return description;
}

string Theme::getImageFileName() {
	return imageFileName;
}

void Theme::KeyDown(unsigned int keycode) {
	// printf("Theme: KeyDown %i\n", keycode);
	
	
	kb.KeyDown(keycode);
	
	
	if (activeMap) {
		activeMap->KeyDown(keycode);
		SwitchMapOnKeyChange();
	}


	
}

void Theme::KeyUp(unsigned int keycode) {
	// printf("Theme: KeyUp %i\n", keycode);
	kb.KeyUp(keycode);
	
	if (activeMap) {
		
		activeMap->KeyUp(keycode);
		SwitchMapOnKeyChange();
	}

	
}



bool Theme::SwitchMapOnKeyChange() {
	return SwitchMapOnKeyChange(true);
}


void Theme::checkSpecialKeysInUse(string keyName) {
	if (keyName == string("CapsLockActivated")) {
		capsLockInUse = true;
	}
	if (keyName == string("NumLockActivated")) {
		numLockInUse = true;
	}
}


// dont call directly! use SwitchMapOnKeyChange()
bool Theme::SwitchMapOnKeyChange(bool withSpecialKeys) {
	
	vector<string> keysPressed;
	keysPressed = kb.getAllPressedKeys();
	if (withSpecialKeys) {
		// caps lock?
		if (capsLockInUse) {
			if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0) {
				keysPressed.push_back("CapsLockActivated");
			}
		}
		
		// num lock?
		if (numLockInUse) {
			if ((GetKeyState(VK_NUMLOCK) & 0x0001) != 0) {
				keysPressed.push_back("NumLockActivated");
			}
		}
	}

	sort(keysPressed.begin(), keysPressed.end());
	
	bool switchedMap = false;
	/*
	DebugMsg("--------------------------------------");
	for (vector<string>::iterator it = keysPressed.begin(); it != keysPressed.end(); ++it) {
		DebugMsg(" - KeyPressed: '%s'", (*it).c_str());
	}
	*/

	for (unordered_map<string, vector<string> >::iterator it = keySwitchGroups.begin(); it != keySwitchGroups.end(); ++it) {
		string combinedMapAndId = it->first;

		// DebugMsg("Try %s", combinedMapAndId.c_str());
		string mapname = combinedMapAndId.substr(0, combinedMapAndId.find("_-_-_-"));

		vector<string> keyMap = it->second;
		/*
		for (vector<string>::iterator it = keyMap.begin(); it != keyMap.end(); ++it) {
			DebugMsg(" - KeyMap: '%s'", (*it).c_str());
		}
		*/
		
		// this is EVIL!  Dont access a map inside switcher!
		// ThemeMap * testMap = GetMapByName(mapname);

		

		int sizeA = keysPressed.size();
		int sizeB = keyMap.size();

		if (sizeA == sizeB) {
			// DebugMsg("!!! Size Equal!");
			bool are_equal = true;
			for (int i = 0; i < sizeB; i++) {
				if (keysPressed.at(i) != keyMap.at(i))
				{
					are_equal = false;
					break;
				}
			}

			if (are_equal) {
				DebugMsg("KeyPresses are equal: '%s'", mapname.c_str());
				ActivateMapByName(mapname);
				switchedMap = true;
			}
		}
		// DebugMsg("--------------------------------------");
	}

	
	return switchedMap;
}


void Theme::AddKeySwitchGroup(string mapname, int groupId, vector<string>keyNamesOfGroup) {
	vector<string> keysOfGroupCodes;
	
	string myMapId = string(mapname + "_-_-_-" + to_string(groupId));
	
	// DebugMsg("AddKeySwitchGroup '%s' (size:%i)", myMapId.c_str(), keyNamesOfGroup.size());
	// convert in codes...
	for (vector<string>::iterator it = keyNamesOfGroup.begin(); it != keyNamesOfGroup.end(); ++it) {
		// DebugMsg(" - Key '%s'", (*it).c_str());
		string keyName = (*it);
		checkSpecialKeysInUse(keyName);
		keysOfGroupCodes.push_back(*it);
	}

	sort(keysOfGroupCodes.begin(), keysOfGroupCodes.end());
	keySwitchGroups.insert(make_pair(myMapId, keysOfGroupCodes));
}


void Theme::AddOverlaySwitch(string mapname, int groupId, vector<string>keyNamesOfGroup) {
	vector<string> keysOfGroupCodes;

	string myMapId = string(mapname + "_-_-_-" + to_string(groupId));

	// convert in codes...
	for (vector<string>::iterator it = keyNamesOfGroup.begin(); it != keyNamesOfGroup.end(); ++it) {
		keysOfGroupCodes.push_back(*it);
	}

	sort(keysOfGroupCodes.begin(), keysOfGroupCodes.end());
	overlaySwitchGroups.insert(make_pair(myMapId, keysOfGroupCodes));
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
	if (activeMapName == mapname) {
		return;
	}
	ThemeMap * myMap = GetMapByName(mapname);

	if (!myMap) {
		DebugMsg("WARNING: Map '%s' not found", mapname.c_str());
		return;
	}

	if (!activeMap) {
		myMap->Activate();
		activeMap = myMap;
		activeMapName = mapname;
	}
	else if (activeMap->getName() != mapname) {
		myMap->Activate();
		activeMap = myMap;
		activeMapName = mapname;
	}
	

}

ThemeMap * Theme::GetMapByName(string mapname) {
	int mapsize = allMaps.size();
	DebugMsg("mapsize %i", mapsize);
	if (mapsize > 0) {

		ThemeMap * myMapPtr = &allMaps.front();
	
		for (vector<ThemeMap>::iterator it = allMaps.begin(); it != allMaps.end(); ++it) {
			if (it->getName() == mapname) {
				myMapPtr = &(*it);
				break;
			}
		}

		return myMapPtr;
	
	}
	else {
		return NULL;
	}
}

void Theme::Tick() {

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
