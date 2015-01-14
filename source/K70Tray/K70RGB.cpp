#include "stdafx.h"
#include "K70RGB.h"


K70RGB operator + (K70RGB &c1, K70RGB &c2)
{
	int r = 0;
	int g = 0;
	int b = 0;

	r = c1.getR() + c2.getR(); if (r > 7) { r = 7; }
	g = c1.getG() + c2.getG(); if (g > 7) { g = 7; }
	b = c1.getB() + c2.getB(); if (b > 7) { b = 7; }

	K70RGB newColor;
	newColor.setK70RGB(r, g, b);

	// use the Cents constructor and operator+(int, int)
	return newColor;
}


void K70RGB::setR(int r) {
	colorR = r;
}

void K70RGB::setG(int g) {
	colorG = g;
}

void K70RGB::setB(int b) {
	colorB = b;
}
/*
void K70RGB::setDuration(int d) {
	duration = d;
}
*/
void K70RGB::setK70RGB(int r, int g, int  b) {
	colorR = r;
	colorB = b;
	colorG = g;
}

int K70RGB::getR() {
	return colorR;
}

int K70RGB::getG() {
	return colorG;
}

int K70RGB::getB() {
	return colorB;
}
/*
int K70RGB::getDuration() {
	return duration;
}
*/
string K70RGB::getK70RGBString() {
	
	string ret = to_string(getR()) + string(" ") + to_string(getG()) + string(" ") + to_string(getB());
	return ret;
}

