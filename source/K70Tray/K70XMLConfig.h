#ifndef XMLCONFIG_H
#define XMLCONFIG_H

#include "Globals.h"
#include "tinyxml.h"
#include "tinystr.h"
#include "Shlwapi.h"
#include "Theme.h"
#include "ThemeMap.h"
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
	int K70XMLConfig::getLastWindowPosition(string pos);
private:
	string layoutName = "";
	string themeName = "";
	void initLayout();
	void saveConfigValue(TiXmlDocument * doc, string tagName, string content);
	int windowPositionTop = 0;
	int windowPositionLeft = 0;


};

#endif