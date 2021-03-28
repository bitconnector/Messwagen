#include "settings.hpp"

struct Settings settings;
String Einheit = "km/h";
String Name = "";

void writeSettings()
{
  uint16 eeAddress = 0;
  EEPROM.put(eeAddress, settings);
  eeAddress += sizeof(settings);
  EEPROM.put(eeAddress, client);
  eeAddress += sizeof(client);

  writeString(eeAddress, ssid);
  eeAddress += sizeof(ssid) + 1;
  writeString(eeAddress, Name);
  eeAddress += sizeof(Name) + 1;
  if (client == 1)
  {
    writeString(eeAddress, password);
    eeAddress += sizeof(password) + 1;
  }
  EEPROM.commit();
}

void setEinheit()
{
  switch (settings.einheit)
  {
  case 1: //mph
    Einheit = "mph";
    break;
  case 2: //m/s
    Einheit = "m/s";
    break;
  default: //km/h
    Einheit = "km/h";
    settings.einheit = 0;
    break;
  }
  Radumfang = 3.1415926535 * settings.durchmesser;
  Radumfang /= settings.pulse;
}

void loadSettings()
{
  uint16 eeAddress = 0;
  EEPROM.get(eeAddress, settings);
  eeAddress += sizeof(settings);
  EEPROM.get(eeAddress, client);
  eeAddress += sizeof(client);

  ssid = readString(eeAddress);
  eeAddress += sizeof(ssid) + 1;
  Name = readString(eeAddress);
  eeAddress += sizeof(Name) + 1;
  if (Name == "")
  {
    Name = String(ESP.getChipId());
  }
  if (client == 1)
  {
    password = readString(eeAddress);
    eeAddress += sizeof(password) + 1;
  }

  setEinheit();
}

void handleSetting()
{
  String tmp = server.arg("ma");
  float mass = tmp.toFloat();
  if (mass > 0 && mass < 3000)
  {
    settings.massstab = mass;
  }
  tmp = server.arg("ei");
  int teinheit = tmp.toInt();
  if (teinheit >= 0 && teinheit <= 2)
  {
    settings.einheit = teinheit;
    setEinheit();
  }
  tmp = server.arg("du");
  float dur = tmp.toFloat();
  if (dur > 1 && dur < 150)
  {
    settings.durchmesser = dur;
  }
  tmp = server.arg("mu");
  float mult = tmp.toFloat();
  if (mult > 0.05 && mult < 11)
  {
    settings.faktor = mult;
  }
  tmp = server.arg("pu");
  int pul = tmp.toInt();
  if (pul > 0 && pul < 50)
  {
    settings.pulse = pul;
  }
  tmp = server.arg("name");
  if (tmp != "")
  {
    Name = tmp;
  }

  writeSettings();
  printSettings();

  File file = LittleFS.open("/erfolg.html", "r"); //läd wieder die Startseite
  server.streamFile(file, "text/html");
  file.close();
}

void handleReset()
{
  String tmp = server.arg("factory-reset");
  if (tmp.length() > 0)
  {
#ifdef DEBUG_PRINT
    Serial.println("RESET auf Werkseinstellungen");
#endif
    setDefaultSettings(tmp.toInt());
  }

  tmp = server.arg("reset");
  if (tmp.length() > 0)
  {
#ifdef DEBUG_PRINT
    Serial.print("RESET der Strecke: ");
    Serial.println(Umdrehungen);
#endif
    Umdrehungen = 0;
  }
  File file = LittleFS.open("/erfolg.html", "r"); //läd wieder die Startseite
  server.streamFile(file, "text/html");
  file.close();
}

void writeString(int start, String data)
{
  int _size = data.length();
  for (int i = 0; i < _size; i++)
  {
    EEPROM.write(start + i, data[i]);
  }
  EEPROM.write(start + _size, '\0'); //Add termination null character for String Data
  EEPROM.commit();
}

String readString(int start)
{
  String data = "";
  char c;
  for (byte i = 0; i < 40; i++)
  {
    c = EEPROM.read(start + i);
    if (c == '\0')
      break;
    data += c;
  }
  return data;
}

void resetString(int index, int size)
{
  size += index;
  while (index < size)
  {
    index++;
    EEPROM.write(index, '\0');
  }
  EEPROM.commit();
}

void setDefaultSettings(byte type)
{
  if (type == 2)
  {
    resetString(sizeof(settings), sizeof(Name) + sizeof(ssid) + sizeof(password) + 3);
    genSSID();
    printWlan();
  }
  settings.durchmesser = 11.5;
  settings.massstab = 87;
  settings.faktor = 1;
  settings.pulse = 2;
  settings.einheit = 0;
  Name = "Wagen-" + String(ESP.getChipId());

  writeSettings();
  printSettings();
}

void printSettings()
{
#ifdef DEBUG_PRINT
  Serial.println("Settings:");
  Serial.print("Name: ");
  Serial.println(Name);
  Serial.print("Durchmesser: ");
  Serial.println(settings.durchmesser);
  Serial.print("Massstab: ");
  Serial.println(settings.faktor);
  Serial.print("Faktor: ");
  Serial.println(settings.faktor);
  Serial.print("Pulse pro Umdrehung: ");
  Serial.println(settings.pulse);
  Serial.print("Einheit (");
  Serial.print(settings.einheit);
  Serial.print("): ");
  Serial.println(Einheit);
#endif
}
