#ifndef XML_H
#define XML_H

#include "main.hpp"

void showValues();
void showSettings();
void detectPulse();
void setupPulse();

extern volatile unsigned long zeit1, zeit2;
extern bool h;
extern long Umdrehungen;

String millis2time();
float calcSpeed();
float calcDistance();

#endif
