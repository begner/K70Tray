#ifndef BOARDANIMATION_H
#define BOARDANIMATION_H

#include "Globals.h"
#include "RGB.h"
#include <map>
#include <vector>



class BoardAnimation
{
private:	
	bool animationHasEnded;
	string boardAnimationName;
	map <string, vector<RGB>> placedAnimation;
	string getAdressOfAnimation(int x, int y);
	int getPositionOfAnimationAdress(string adress, string pos);
	int startX = 0;
	int startY = 0;
	int currentTick = 0;
public:
	BoardAnimation(string name);
	BoardAnimation();
	~BoardAnimation();
	string getName();
	void setName(string name);
	void addBoardAnimation(int x, int y, vector<RGB> colors);
	void startAt(int x, int y);
	void tick();
	bool animationEnded();
	int getAnimationSize();
};

#endif