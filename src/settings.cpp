#include "settings.hpp"

void writeSettings()
{
  int eeAddress = 0;
  EEPROM.put(eeAddress, bool(1)); //Indikator für gesetzte Einstellungen
  eeAddress += sizeof(bool);
  EEPROM.put(eeAddress, mode);
  eeAddress += sizeof(byte);
  EEPROM.put(eeAddress, Raddurchmesser);
  eeAddress += sizeof(float);
  EEPROM.put(eeAddress, Massstab);
  eeAddress += sizeof(float);
  EEPROM.put(eeAddress, Faktor);
  eeAddress += sizeof(float);
  EEPROM.put(eeAddress, PulseproUmdrehung);
  eeAddress += sizeof(int);
  EEPROM.put(eeAddress, EinheitM);
  eeAddress += sizeof(int);

  int i;
  if (mode == 1)
  {
    for (i = 0; i < password.length(); i++)
    {
      EEPROM.write(eeAddress + i, password[i]);
    }
    eeAddress += i;
    EEPROM.write(eeAddress, '\0');
    eeAddress++;
  }
  for (i = 0; i < ssid.length(); i++)
  {
    EEPROM.write(eeAddress + i, ssid[i]);
  }
  eeAddress += i;
  EEPROM.write(eeAddress, '\0');
  eeAddress++;

  EEPROM.commit();
  Radumfang = 3.1415926535 * Raddurchmesser;
  Radumfang /= PulseproUmdrehung;
}

void setEinheit()
{
  switch (EinheitM)
  {
  case 1: //mph
    Einheit = "mph";
    break;
  case 2: //m/s
    Einheit = "m/s";
    break;
  default: //km/h
    Einheit = "km/h";
    EinheitM = 0;
    break;
  }
}

void genSSID()
{
  ssid = "Wagen-" + String(ESP.getChipId());
}

void loadSettings()
{
  genSSID();
  bool b = 0;
  int eeAddress = 0;
  EEPROM.get(eeAddress, b);
  eeAddress += sizeof(bool);
  EEPROM.get(eeAddress, mode);
  eeAddress += sizeof(byte);
  if (b)
  { //Nur einlesen, wenn Einstellungen vorgenommen wurden
    Serial.println("Einstellungen im EEPROM gefunden");
    EEPROM.get(eeAddress, Raddurchmesser);
    eeAddress += sizeof(float);
    EEPROM.get(eeAddress, Massstab);
    eeAddress += sizeof(float);
    EEPROM.get(eeAddress, Faktor);
    eeAddress += sizeof(float);
    EEPROM.get(eeAddress, PulseproUmdrehung);
    eeAddress += sizeof(int);
    EEPROM.get(eeAddress, EinheitM);
    eeAddress += sizeof(int);
  }
  else
  {
    eeAddress += sizeof(float) + sizeof(float) + sizeof(float) + sizeof(int) + sizeof(int);
  }
  if (mode != 2)
  {
    int i;
    char c;
    if (mode == 1)
    {
      password = "";
      i = 0;
      c = EEPROM.read(eeAddress);
      while (i < 40 && c != '\0')
      {
        password += c;
        i++;
        c = EEPROM.read(eeAddress + i);
      }
      eeAddress += i + 1;
    }
    ssid = "";
    i = 0;
    c = EEPROM.read(eeAddress);
    while (i < 40 && c != '\0')
    {
      ssid += c;
      i++;
      c = EEPROM.read(eeAddress + i);
    }
  }

  EEPROM.commit();
  setEinheit();
  Radumfang = 3.1415926535 * Raddurchmesser;
  Radumfang /= PulseproUmdrehung;
}

void handleSetting()
{
  String tmp = server.arg("ei");
  int teinheit = tmp.toInt();
  tmp = server.arg("ma");
  float mass = tmp.toFloat();
  tmp = server.arg("du");
  float dur = tmp.toFloat();
  tmp = server.arg("mu");
  float mult = tmp.toFloat();
  tmp = server.arg("pu");
  int pul = tmp.toInt();

  tmp = server.arg("factory-reset");
  if (tmp.length() > 0)
  {
    Serial.println("RESET auf Werkseinstellungen");
    EEPROM.put(0, bool(0)); //Indikator für gesetzte Einstellungen
    if (tmp == "2")
    {
      Serial.println("Reset Wifi");
      EEPROM.put(0 + sizeof(bool), byte(2));
    }
    EEPROM.commit();
    ESP.reset();
  }

  tmp = server.arg("reset");
  if (tmp.length() > 0)
  {
    Serial.print("RESET der Strecke");
    Serial.println(Umdrehungen);
    Umdrehungen = 0;
  }

  tmp = server.arg("wlan");
  if (tmp.length() > 0)
  {
    Serial.print("Neue Netzwerkeinstellungen");
    if (tmp.toInt() == 0)
    { //Hotspot
      mode = 0;
      if (server.arg("ssid") != "")
      {
        ssid = server.arg("ssid");
      }
    }
    else
    { //client
      mode = 1;
      if (server.arg("ssid") != "")
      {
        ssid = server.arg("ssid");
      }
      if (server.arg("pass") != "")
      {
        password = server.arg("pass");
      }
    }
    startWifi();
    writeSettings();
  }

  else
  {
    Serial.println("Neue Einstellung");
    if (mass > 0 && mass < 3000)
    {
      Massstab = mass;
      Serial.print("Massstab: ");
      Serial.println(Massstab);
    }
    if (teinheit >= 0 && teinheit <= 2)
    {
      EinheitM = teinheit;
      setEinheit();
      Serial.print("Einheit: ");
      Serial.println(Einheit);
    }
    if (dur > 1 && dur < 25)
    {
      Raddurchmesser = dur;
      Serial.print("Raddurchmesser: ");
      Serial.println(Raddurchmesser);
    }
    if (mult > 0.05 && mult < 11)
    {
      Faktor = mult;
      Serial.print("Faktor: ");
      Serial.println(Faktor);
    }
    if (pul > 0 && pul < 50)
    {
      PulseproUmdrehung = pul;
      Serial.print("PulseproUmdrehung: ");
      Serial.println(PulseproUmdrehung);
    }
    writeSettings();
  }

  File file = SPIFFS.open("/erfolg.html", "r"); //läd wieder die Startseite
  size_t sent = server.streamFile(file, "text/html");
  file.close();
}

void showSettings()
{
  String HTML;
  HTML = "<?xml version='1.0'?>";
  HTML += "<data>";
  HTML += "<einheit>";
  HTML += EinheitM;
  HTML += "</einheit>";
  HTML += "<durchmesser>";
  HTML += Raddurchmesser;
  HTML += "</durchmesser><massstab>";
  HTML += Massstab;
  HTML += "</massstab><faktor>";
  HTML += Faktor;
  HTML += "</faktor><pulseproumdrehung>";
  HTML += PulseproUmdrehung;
  HTML += "</pulseproumdrehung><wlan>";
  HTML += mode;
  HTML += "</wlan><ssid>";
  HTML += ssid;
  HTML += "</ssid></data>";
  server.send(200, "text/xml", HTML);
}

void startWifi()
{
  if (mode)
  {
    Serial.println();
    Serial.print("Connecting ");
    WiFi.begin(ssid.c_str(), password.c_str());
    unsigned long zeit = millis() + 30000;
    while (!WiFi.isConnected() && millis() < zeit)
    {
      delay(500);
      Serial.print(".");
      digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
    }
    Serial.println();
    WiFi.mode(WIFI_STA);
    Serial.println("\n\nBOOTING ESP8266 ...");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("Station IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(BUILTIN_LED, HIGH);
  }
  if (!WiFi.isConnected())
  {
    digitalWrite(BUILTIN_LED, LOW);
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    Serial.print("AP: ");
    if (mode)
    {
      Serial.println("Failsafe");
      genSSID();
    }
    Serial.println(ssid);
    WiFi.softAP(ssid, "");
    Serial.print("Server IP address: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("Server MAC address: ");
    Serial.println(WiFi.softAPmacAddress());
    //Serial.println(ESP.getChipId());
  }
}