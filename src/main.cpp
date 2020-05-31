#include "main.hpp"


ESP8266WebServer server(80);

float Radumfang;

void setup()
{
#ifdef DEBUG_PRINT
  Serial.begin(115200);
#endif
#ifdef DEBUG_LED
  pinMode(DEBUG_LED, OUTPUT);
#endif

  EEPROM.begin(512);
  loadSettings();

#ifdef DEBUG_PRINT
  Serial.println("size: " + String(ESP.getFlashChipSize()) + ", real size: " + String(ESP.getFlashChipRealSize()));
#endif
  SPIFFS.begin();

  startWifi();

  server.serveStatic("/", SPIFFS, "/index.html");
  server.serveStatic("/config.html", SPIFFS, "/config.html");
  server.serveStatic("/wlan.html", SPIFFS, "/wlan.html");
  server.on("/show_values.xml", showValues);
  server.on("/setting", handleSetting);
  server.on("/show_settings.xml", showSettings);
  server.on("/wlan_setting", handleWlanSetting);

  server.begin();
  setupPulse();
}

void loop()
{
  server.handleClient();
  detectPulse();
}
