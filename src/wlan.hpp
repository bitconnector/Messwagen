#ifndef WLAN_H
#define WLAN_H

#include "main.hpp"
#include "settings.hpp"

extern bool client;
extern String ssid;
extern String password;

void handleWlanSetting();
void genSSID();
void startWifi();
void printWlan();

#endif
