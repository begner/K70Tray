#ifndef KEYCOLOR_H
#define KEYCOLOR_H

#include "Constants.h"
#include "Globals.h"
// #include "OutputConsole.h"
#include "RGB.h"
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <vector>

class KeyColor
{
	bool justReleased = false;
	bool justPressed = false;
	bool		keyDown = false;
	unsigned int	tick = 0;
	vector<RGB> * getVectorByKeyState();

	vector<RGB> defaultColor;
	string		defaultSyncName = "";


	vector<RGB> onPressColor;
	string		onPressJoinMode = "overwrite";
	string		onPressSyncName = "";
	string		onPressBoardAnimationName = "";

	vector<RGB>		onReleaseColor;
	unsigned int	releaseTickCountDown = 0;
	void			CountDownReleaseAnimation();
	string			onReleaseJoinMode = "overwrite";
	string			onReleaseBoardAnimationName = "";
public: 
	KeyColor();
	~KeyColor();
	void clear(string type);
	void addColor(string type, RGB color, unsigned int duration);
	void setKeyDown(bool state);
	void Tick(); 
	void setFrame(unsigned int synctick);
	RGB getColor();
	// void setSyncPulse(string type, int pulse);
	string getSyncName();
	void setSyncName(string type, string syncName);
	void setJoinMode(string type, string joinMode);
	void setBoardAnimationName(string type, string boardAnimationNameName);

	string getBoardAnimationNameOnPress();
	string getBoardAnimationNameOnRelease();
	bool getJustReleased();
	bool getJustPressed();

};



#endif

