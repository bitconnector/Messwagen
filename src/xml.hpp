#ifndef XML_H
#define XML_H

#include "main.hpp"
#include "settings.hpp"
#include "wlan.hpp"

#define PULSE_TIMEOUT 10000

void showValues();
void showSettings();
void detectPulse();
void setupPulse();

extern volatile unsigned long zeit1, zeit2;
extern bool h;
extern long Umdrehungen;
extern struct Settings settings;

String millis2time();
float calcSpeed();
float calcDistance();
float addScale(float);

#endif
