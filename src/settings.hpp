#ifndef SETTINGS_H
#define SETTINGS_H

#include "main.hpp"

struct Settings {
    float durchmesser;
    float massstab;
    float faktor;
    int pulse;
    byte einheit;
};

extern float Radumfang;
extern String Einheit;


void writeSettings();
void loadSettings();
void handleSetting();
void printSettings();
void handleReset();

void writeString(int start, String data);
String readString(int start);
void resetString(int index, int size);
void setDefaultSettings(byte);

#endif
