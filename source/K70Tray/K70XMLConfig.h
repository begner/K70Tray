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
	void parseLayout(const char *);
	void parseTheme(const char *);
private:
	string layoutName = "";
	string themeName = "";
		


};

#endif