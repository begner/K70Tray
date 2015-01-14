#ifndef K70RGB_H
#define K70RGB_H

#include <string>
#include <stdio.h>
#include <iostream>

using namespace std;

class K70RGB {
	int colorR = 0, colorG = 0, colorB = 0; 
	// int duration = 1;
public:
	int getR();
	int getG();
	int getB();
	// int getDuration();

	void setR(int);
	void setG(int);
	void setB(int);
	// void setDuration(int);
	void setK70RGB(int, int, int);
	string getK70RGBString();
	friend K70RGB operator + (K70RGB &c1, K70RGB &c2);
};



#endif