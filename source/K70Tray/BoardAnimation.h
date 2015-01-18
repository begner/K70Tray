#ifndef BOARDANIMATION_H
#define BOARDANIMATION_H

#include "Globals.h"
#include "K70RGB.h"
#include <map>
#include <vector>



class BoardAnimation
{
private:	
	bool animationHasEnded;
	string boardAnimationName;
	map <string, vector<K70RGB>> placedAnimation;
	string getAdressOfAnimation(int x, int y);
	int getPositionOfAnimationAdress(string adress, string pos);
	int startX = 0;
	int startY = 0;
	size_t currentTick = 0;
public:
	BoardAnimation(string name);
	BoardAnimation();
	~BoardAnimation();
	string getName();
	void setName(string name);
	void addBoardAnimation(int x, int y, vector<K70RGB> colors);
	void startAt(int x, int y);
	void tick();
	bool animationEnded();
	int getAnimationSize();
};

#endif