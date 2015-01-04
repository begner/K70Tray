#ifndef RGB_H
#define RGB_H

#include <string>
#include <stdio.h>
#include <iostream>

using namespace std;

class RGB {
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
	void setRGB(int, int, int);
	string getRGBString();
	friend RGB operator + (RGB &c1, RGB &c2);
};



#endif