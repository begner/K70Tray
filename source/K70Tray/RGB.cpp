#include "stdafx.h"
#include "RGB.h"


RGB operator + (RGB &c1, RGB &c2)
{
	int r = 0;
	int g = 0;
	int b = 0;

	r = c1.getR() + c2.getR(); if (r > 7) { r = 7; }
	g = c1.getG() + c2.getG(); if (g > 7) { g = 7; }
	b = c1.getB() + c2.getB(); if (b > 7) { b = 7; }

	RGB newColor;
	newColor.setRGB(r, g, b);

	// use the Cents constructor and operator+(int, int)
	return newColor;
}


void RGB::setR(int r) {
	colorR = r;
}

void RGB::setG(int g) {
	colorG = g;
}

void RGB::setB(int b) {
	colorB = b;
}
/*
void RGB::setDuration(int d) {
	duration = d;
}
*/
void RGB::setRGB(int r, int g, int  b) {
	colorR = r;
	colorB = b;
	colorG = g;
}

int RGB::getR() {
	return colorR;
}

int RGB::getG() {
	return colorG;
}

int RGB::getB() {
	return colorB;
}
/*
int RGB::getDuration() {
	return duration;
}
*/
string RGB::getRGBString() {
	
	string ret = to_string(getR()) + string(" ") + to_string(getG()) + string(" ") + to_string(getB());
	return ret;
}

