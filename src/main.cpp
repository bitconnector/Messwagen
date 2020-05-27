#include "main.hpp"

byte mode = 0;
String ssid = "";
String password = "";

ESP8266WebServer server(80);
String Einheit = "km/h";
int EinheitM = 0;
bool h = 1;
long Umdrehungen;
volatile unsigned long zeit1, zeit2;

float Raddurchmesser = 4; //Standartwerte für spur N
float Massstab = 160;
float Faktor = 2;
int PulseproUmdrehung = 2;

float Radumfang;

String millis2time()
{
  String Time = "";
  unsigned long ss;
  byte mm, hh;
  ss = millis() / 1000;
  hh = ss / 3600;
  mm = (ss - hh * 3600) / 60;
  ss = (ss - hh * 3600) - mm * 60;
  if (hh < 10)
    Time += "0";
  Time += (String)hh + ":";
  if (mm < 10)
    Time += "0";
  Time += (String)mm + ":";
  if (ss < 10)
    Time += "0";
  Time += (String)ss;
  return Time;
}

float calcSpeed()
{
  int Zeit = zeit1 - zeit2;
  if (Zeit < millis() - zeit1)
    Zeit = millis() - zeit1; //Zeit verlängern, wenn der letzte Puls zu lange her war
  if (Zeit > 30000)
    return 0; //Timeout für pulse länger als 30Sekunden
  float geschw = Radumfang / Zeit;
  geschw *= Massstab; //Multiplizieren mit dem Massstab
  geschw *= Faktor;   //Multiplizieren mit dem Faktor
  switch (EinheitM)
  {
  case 1: //mph
    geschw *= 2.236936;
    break;
  case 2:  //m/s
    break; //passt alles da die einheit mm/ms
  default: //km/h
    geschw *= 3.6;
    break;
  }
  return geschw;
}

float calcDistance()
{
  return Umdrehungen * Radumfang / 1000;
}

void showValues()
{
  String XML;
  XML = "<?xml version='1.0'?>";
  XML += "<data>";
  XML += "<uptime>";
  XML += millis2time();
  XML += "</uptime>";
  XML += "<speed>";
  XML += calcSpeed(); //millis();
  XML += " ";
  XML += Einheit; //übertragung der Einheit
  XML += "</speed>";
  XML += "<distance>";
  XML += calcDistance();
  XML += "</distance>";
  XML += "</data>";
  server.send(200, "text/xml", XML);
}

void setup()
{
  Serial.begin(115200);
  pinMode(Sensor, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);

  EEPROM.begin(512);
  loadSettings();

  Serial.println("size: " + String(ESP.getFlashChipSize()) + ", real size: " + String(ESP.getFlashChipRealSize()));
  if (!SPIFFS.begin())
    Serial.println("SPIFFS Mount failed");
  else
    Serial.println("SPIFFS Mount succesfull");

  startWifi();

  server.serveStatic("/", SPIFFS, "/index.html");
  server.serveStatic("/config.html", SPIFFS, "/config.html");
  server.serveStatic("/wlan.html", SPIFFS, "/wlan.html");
  server.on("/show_values.xml", showValues);
  server.on("/setting", handleSetting);
  server.on("/show_settings.xml", showSettings);

  server.begin();
  zeit1 = millis() - 30000; //Damit die Geschwindigkeit am Anfang 0 ist
  zeit2 = zeit1;
}

void loop()
{
  server.handleClient();
  if (digitalRead(Sensor))
  {
    if (h)
    {
      Serial.println("Flanke");
      h = 0;
      zeit2 = zeit1;
      zeit1 = millis();
      Umdrehungen++;
    }
  }
  else
    h = 1;
}
