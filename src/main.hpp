#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <EEPROM.h>
#define Sensor 2
//#define CLIENT

#include "settings.hpp"
#include "xml.hpp"
#include "wlan.hpp"

//#define DEBUG_LED 2
//#define DEBUG_PRINT

extern ESP8266WebServer server;


// extern int EinheitM;

// extern float Raddurchmesser;
// extern float Massstab;
// extern float Faktor;
// extern int PulseproUmdrehung;
extern struct Settings settings;


#endif
