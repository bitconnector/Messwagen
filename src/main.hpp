#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <EEPROM.h>
#define Sensor 2
//#define CLIENT

#include "settings.hpp"

extern ESP8266WebServer server;

extern String ssid;
extern String password;
extern byte mode;

extern String Einheit;
extern int EinheitM;
extern bool h;
extern long Umdrehungen;
extern volatile unsigned long zeit1, zeit2;

extern float Raddurchmesser;
extern float Massstab;
extern float Faktor;
extern int PulseproUmdrehung;

extern float Radumfang;


#endif
