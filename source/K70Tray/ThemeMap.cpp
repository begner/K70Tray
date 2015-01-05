#include "stdafx.h"
#include "ThemeMap.h"
#include "LightControl.h"
#include "Theme.h"

using namespace std;

extern RGB					ledState[K70_ROWS][K70_COLS];
extern LightControl			* pLC;
extern string				g_keyNames[K70_KEY_MAX];
extern unsigned char		g_keyCodes[K70_KEY_MAX];
extern unsigned char		g_ledAdress[K70_KEY_MAX];


/*
* Empty Contstructor
*/
ThemeMap::ThemeMap(string mapname)
{
	// printf("Create ThemeMap with name\n");
	setName(mapname);
	ThemeMap();
}

/*
* Constructor with Name
*/
ThemeMap::ThemeMap()
{
	BoardAnimationMap.clear();
	keyColorMap.clear();
}


ThemeMap::~ThemeMap() {
	BoardAnimationMap.clear();
	keyColorMap.clear();
}


void ThemeMap::addBoardAnimation(BoardAnimation ba) {
	DebugMsg("Add BoardAnimation with size %i", ba.getAnimationSize());
	BoardAnimationMap.insert(make_pair(ba.getName(), ba));
}

/*
* Set Name of the Map
*/
void ThemeMap::setName(string mapname) {
	myMapName = mapname;
}

/*
* Gets the Name of the map
*/
string ThemeMap::getName() {
	return myMapName;
}



/*
* Sets a Mapping of keyName & keyColor (will be done in XMLConfig!)
*/
void ThemeMap::setMapping(string keyName, KeyColor keycolor) {
	// vector<KeyColor> keyColorVector;
	// keyColorVector.push_back(keycolor);
	// keyColorMap.insert(make_pair(keyName, keyColorVector));
}

void ThemeMap::setTheme(Theme * theme) {
	parentTheme = theme;
}

Theme * ThemeMap::getTheme() {
	return parentTheme;
}


void ThemeMap::SetBoardAnimationName(string keyName, string type, string boardAnimationNameName) {

	if (keyName != string("")) {
		unsigned int keyCode = parentTheme->getKeyboard()->getCodeByName(keyName);
		keyColorMap[keyCode].setBoardAnimationName(type, boardAnimationNameName);
	}
	else {
		vector <unsigned int> allKeyCodes = parentTheme->getKeyboard()->GetAllKeyCodes();
		for (vector <unsigned int>::iterator it = allKeyCodes.begin(); it != allKeyCodes.end(); ++it) {
			unsigned int keyCode = (*it);
			string keyName = parentTheme->getKeyboard()->getNameByCode(keyCode);
			if (keyName != string(""))
			{
				SetBoardAnimationName(keyName, type, boardAnimationNameName);
			}
		}
	}
}

/*
* Clears all Colors of a keymap...
*
* if keyname == "" - means all!
* type needs to be 'default' or 'onPress' or '' for all
*/

void ThemeMap::ClearKeyMapColors(string keyName, string type) {
	// für einen bestimmten key
	
	if (keyName != string("")) {
		unsigned int keyCode = parentTheme->getKeyboard()->getCodeByName(keyName);
		// DebugMsg("Clearing Keymap for %s on %s\n", type.c_str(), keyName.c_str());
		keyColorMap[keyCode].clear(type);
	}
	else {
		vector <unsigned int> allKeyCodes = parentTheme->getKeyboard()->GetAllKeyCodes();
		for (vector <unsigned int>::iterator it = allKeyCodes.begin(); it != allKeyCodes.end(); ++it) {
			unsigned int keyCode = (*it);
			string keyName = parentTheme->getKeyboard()->getNameByCode(keyCode);
			if (keyName != string(""))
			{
				ClearKeyMapColors(keyName, type);
			}
		}
	}
}

void ThemeMap::SetSyncNameToKeyMap(string keyName, string type, string syncName) {
// für einen bestimmten key

	if (keyName != string("")) {
		unsigned int keyCode = parentTheme->getKeyboard()->getCodeByName(keyName);
		keyColorMap[keyCode].setSyncName(type, syncName);
	}
	else {
		vector <unsigned int> allKeyCodes = parentTheme->getKeyboard()->GetAllKeyCodes();
		for (vector <unsigned int>::iterator it = allKeyCodes.begin(); it != allKeyCodes.end(); ++it) {
			unsigned int keyCode = (*it);
			string keyName = parentTheme->getKeyboard()->getNameByCode(keyCode);
			if (keyName != string(""))
			{
				SetSyncNameToKeyMap(keyName, type, syncName);
			}
		}
	}
}

void ThemeMap::SetJoinMode(string keyName, string type, string joinMode) {

	if (keyName != string("")) {
		unsigned int keyCode = parentTheme->getKeyboard()->getCodeByName(keyName);
		keyColorMap[keyCode].setJoinMode(type, joinMode);
	}
	else {
		vector <unsigned int> allKeyCodes = parentTheme->getKeyboard()->GetAllKeyCodes();
		for (vector <unsigned int>::iterator it = allKeyCodes.begin(); it != allKeyCodes.end(); ++it) {
			unsigned int keyCode = (*it);
			string keyName = parentTheme->getKeyboard()->getNameByCode(keyCode);
			if (keyName != string(""))
			{
				SetJoinMode(keyName, type, joinMode);
			}
		}
	}
}

void ThemeMap::RemoveSync(string keyName, string type) {

	if (keyName != string("")) {
		unsigned int keyCode = parentTheme->getKeyboard()->getCodeByName(keyName);
		keyColorMap[keyCode].setSyncName(type, "");
	}
	else {
		vector <unsigned int> allKeyCodes = parentTheme->getKeyboard()->GetAllKeyCodes();
		for (vector <unsigned int>::iterator it = allKeyCodes.begin(); it != allKeyCodes.end(); ++it) {
			unsigned int keyCode = (*it);
			string keyName = parentTheme->getKeyboard()->getNameByCode(keyCode);
			if (keyName != string(""))
			{
				RemoveSync(keyName, type);
			}
		}
	}
}



/*
* Adds a Color to a theme Map...
*
* if keyname == "" - means all!
* type needs to be 'default' or 'onPress'
*/
void ThemeMap::AddColorToKeyMap(string keyName, string type, RGB color, unsigned int duration) {

	if (keyName != string("")) {
		unsigned int keyCode = parentTheme->getKeyboard()->getCodeByName(keyName);
		// printf("Map Key: '%s' (%i) / '%s' -> %s\n", keyName.c_str(), keyCode, type.c_str(), color.getRGBString().c_str());
		keyColorMap[keyCode].addColor(type, color, duration);
	}
	else {
		vector <unsigned int> allKeyCodes = parentTheme->getKeyboard()->GetAllKeyCodes();
		for (vector <unsigned int>::iterator it = allKeyCodes.begin(); it != allKeyCodes.end(); ++it) {
			unsigned int keyCode = (*it);
			string keyName = parentTheme->getKeyboard()->getNameByCode(keyCode);
			if (keyName != string(""))
			{
				AddColorToKeyMap(keyName, type, color, duration);
			}
		}
	}
	// printf("keyColorMap size %i \n", keyColorMap.size());

}



void ThemeMap::KeyDown(unsigned int keyCode) {
	map <unsigned int, KeyColor>::iterator it;
	it = keyColorMap.find(keyCode);

	KeyColor * kc = &it->second;
	kc->setKeyDown(true);
}

void ThemeMap::KeyUp(unsigned int keyCode) {
	DebugMsg("ThemeMap - Key Up: int=%i hex=0x%X\n", keyCode, keyCode);
	map <unsigned int, KeyColor>::iterator it;
	it = keyColorMap.find(keyCode);

	KeyColor * kc = &it->second;
	kc->setKeyDown(false);
}

void ThemeMap::Activate() {
	DebugMsg("Activate Map '%s'\n", getName().c_str());
}

void ThemeMap::Tick() {

	// printf("keyColorMap size %i \n", keyColorMap.size());
	for (map <unsigned int, KeyColor>::iterator it = keyColorMap.begin(); it != keyColorMap.end(); ++it) {
		unsigned int keyCode = it->first;
		KeyColor * kc = &it->second;
		
		string syncName = kc->getSyncName();

		if (syncName != string("")) {
			kc->setFrame(parentTheme->getSyncGroupTick(syncName));
		}
		else {
			kc->Tick();
		}

		RGB curColor = kc->getColor();

		int px = pLC->getXYByKeyCode(keyCode, string("x"));
		int py = pLC->getXYByKeyCode(keyCode, string("y"));

		// printf("Set Led on %ix%i of Key 0x%X to %s'\n", px, py, keyCode, curColor.getRGBString().c_str());
		ledState[px][py].setR(curColor.getR());
		ledState[px][py].setG(curColor.getG());
		ledState[px][py].setB(curColor.getB());
		


		
		// copy Board Animations to active vector
		if (kc->getJustReleased()) {
			
			string baor = kc->getBoardAnimationNameOnRelease();
			// DebugMsg("getJustReleased: Board Animation '%s'", baor.c_str());
			if (baor != string("")) {
				// DebugMsg("Board Animation size %i", BoardAnimationMap.find(baor)->second.getAnimationSize());
				
				BoardAnimation newBA = BoardAnimationMap.find(baor)->second;
				newBA.startAt(px, py);
				activeBoardAnimations.push_back(newBA);
			}
			
		}
		if (kc->getJustPressed()) {
			string baop = kc->getBoardAnimationNameOnPress();
			// DebugMsg("getJustPressed: Board Animation '%s'", baop.c_str());
			if (baop != string("")) {
				BoardAnimation newBA = BoardAnimationMap.find(baop)->second;
				newBA.startAt(px, py);
				activeBoardAnimations.push_back(newBA);
			}
		}
		
	}
	
	
	vector<BoardAnimation>::iterator it = activeBoardAnimations.begin();
	while (it != activeBoardAnimations.end())
	{
		// execute activeBoardAnimations
		it->tick();
	
		// delete finished BoardAnimations
		if (it->animationEnded()) 
		{
			it = activeBoardAnimations.erase(it);
		}
		else
		{
			++it;
		}
		
	}
	



}
