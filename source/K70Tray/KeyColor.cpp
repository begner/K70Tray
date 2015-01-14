#include "stdafx.h"
#include "KeyColor.h"



KeyColor::KeyColor()
{

}

KeyColor::~KeyColor()
{
	defaultColor.clear();
	onPressColor.clear();
	onReleaseColor.clear();
}

void KeyColor::clear(string type) {
	if (type == string("default") || type == string("")) {
		defaultColor.clear();
	}
	if (type == string("onPress") || type == string("")) {
		onPressColor.clear();
	}
	if (type == string("onRelease") || type == string("")) {
		onReleaseColor.clear();
	}
}

void KeyColor::setJoinMode(string type, string joinMode) {
	if (type == string("onPress") || type == string("")) {
		onReleaseJoinMode = joinMode;
	}
	if (type == string("onRelease") || type == string("")) {
		onReleaseJoinMode = joinMode;
	}
	
}

void KeyColor::addColor(string type, K70RGB color, unsigned int duration) {
	
	// duration auflösen!
	// int duration = color.getDuration() + 1;
	
	K70RGB newColor; 
	newColor.setK70RGB(color.getR(), color.getG(), color.getB());
	
	for (unsigned int i = 0; i < duration; i++) {
		if (type == string("default")) {
			defaultColor.push_back(newColor);
		}
		if (type == string("onPress")) {
			onPressColor.push_back(newColor);
		}
		if (type == string("onRelease")) {
			onReleaseColor.push_back(newColor);
		}
	}
}

void KeyColor::setKeyDown(bool state) {
	bool oldState = keyDown;

	keyDown = state;
	if (oldState != keyDown) {
		if (keyDown == false) {
			// keyup
			
			justReleased = true;
			
			releaseTickCountDown = onReleaseColor.size(); // will be removed before tick!
			
			// DebugMsg("RELEASE %i\n", releaseTickCountDown);
		}
		else {
			justPressed = true;
			
		}
		
		tick = 0;
	}
	
}

bool KeyColor::getJustPressed() {
	bool oldstate = justPressed;
	justPressed = false;
	return oldstate;
}

bool KeyColor::getJustReleased() {
	bool oldstate = justReleased;
	justReleased = false;
	return oldstate;
}


void KeyColor::setSyncName(string type, string syncName) {
	// printf("set syncname '%s' to '%s'\n", type.c_str(), syncName.c_str());
	if (type == string("default")) {
		// printf("set default syncname to '%s'\n", syncName.c_str());
		defaultSyncName = syncName;
	}
	if (type == string("onPress")) {
		// printf("set onPress syncname to '%s'\n", syncName.c_str());
		onPressSyncName = syncName;
	}

}

void KeyColor::setBoardAnimationName(string type, string boardAnimationNameName) {
	if (type == string("onPress")) {
		DebugMsg("set onPress BoardAnimationName '%s' => '%s'", type.c_str(), boardAnimationNameName.c_str());
		onPressBoardAnimationName = boardAnimationNameName;
	}
	if (type == string("onRelease")) {
		// DebugMsg("set onRelease BoardAnimationName '%s' => '%s'", type.c_str(), boardAnimationNameName.c_str());
		// printf("set onPress syncname to '%s'\n", syncName.c_str());
		onReleaseBoardAnimationName = boardAnimationNameName;
	}
}

string KeyColor::getSyncName() {
	string ret = defaultSyncName;

	if (keyDown) {
		// DebugMsg("Keydown - %i\n", onPressColor.size());
		if (onPressColor.size() > 0) {
			ret = onPressSyncName;
		}
	}
	return ret;
}

string KeyColor::getBoardAnimationNameOnPress() {

	return onPressBoardAnimationName;
}

string KeyColor::getBoardAnimationNameOnRelease() {
	return onReleaseBoardAnimationName;
}



void KeyColor::Tick() {
	CountDownReleaseAnimation();
		
	vector<K70RGB> * vec = getVectorByKeyState();
	if (tick >= vec->size() - 1) {
		tick = 0;
	}
	else {
		tick++;
	}
}

void KeyColor::CountDownReleaseAnimation() {
	if (releaseTickCountDown > 0) {
		// DebugMsg("releaseColorCountDown: %i\n", releaseTickCountDown);
		releaseTickCountDown--;
	}

}



/*
set an external tick!
resets tick and durationTicks - and called ticks for external tick int.
*/
void KeyColor::setFrame(unsigned int synctick) {
	CountDownReleaseAnimation();

	vector<K70RGB> * vec = getVectorByKeyState();
	if (vec->size() > 0) {
		tick = synctick % vec->size();
	}
}

K70RGB KeyColor::getColor() {
	K70RGB color;
	vector<K70RGB> * vec = getVectorByKeyState();
	if (vec->size() > 0 && vec->size() >= tick) {
		color = vec->at(tick);
	}

	// printf("releaseTickCountDown: %i\n", releaseTickCountDown);
	if (onReleaseColor.size() > 0 && releaseTickCountDown > 0) {
		K70RGB releaseColor;
		vector<K70RGB> * orcvec = &onReleaseColor;

		releaseColor = orcvec->at(onReleaseColor.size() - releaseTickCountDown);
		if (onReleaseJoinMode == string("add")) {
			color = color + releaseColor;
		}
		else {
			color = releaseColor;
		}
		
	}
	
	return color;
}

vector<K70RGB> * KeyColor::getVectorByKeyState() {
	vector<K70RGB> * vec = &defaultColor;

	// DebugMsg("- - %i\n", onPressColor.size());
	if (keyDown && onPressColor.size() > 0) {
		// DebugMsg("- - - Keydown\n");
		vec = &onPressColor;
	}
	
	return vec;
}
