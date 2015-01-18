#ifndef XMLCONFIG_H
#define XMLCONFIG_H

#include "Globals.h"
#include "tinyxml.h"
#include "tinystr.h"
#include "Shlwapi.h"
#include "Theme.h"
#include "ThemeMap.h"
#include "FileSystem.h"
#include "K70RGB.h"

#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <utility>


using namespace std;

class MainCorsairRGBK;


class K70XMLConfig
{
public:
	K70XMLConfig(MainCorsairRGBK * mainref);
	bool readConfig();
	bool readTheme();
	string getLayoutName();
	string getThemeName();
	bool parseLayout(const char *);
	void parseTheme(const char *);
	void saveConfig();
	int getLastWindowPosition(string pos);
	void setLastWindowPosition(int posX, int posY);
private:
	string layoutName = "";
	string themeName = "";
	void initLayout();
	void saveConfigValue(TiXmlDocument * doc, string tagName, string content);
	int windowPositionTop = -1;
	int windowPositionLeft = -1;
	vector<vector<string>> getKeyGroupsOfMap(TiXmlElement* mapNode);
	void parseMap(
		ThemeMap * myMap,
		TiXmlElement* mapNode,
		map <string, vector<string>> sharedKeyGroupMap,
		map <string, vector<K70RGB>> sharedAnimationMap,
		map <string, BoardAnimation> sharedBoardAnimationMap);


};

#endif