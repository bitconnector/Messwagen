#include "xml.hpp"

volatile unsigned long zeit1, zeit2;
bool h = 1;
long Umdrehungen;

void showValues()
{
    String XML;
    XML = F("<?xml version='1.0'?><data><uptime>");
    XML += millis2time();
    XML += F("</uptime><speed>");
    XML += addScale(calcSpeed());
    XML += " " + Einheit;
    XML += F("</speed><distance>");
    XML += calcDistance();
    XML += F("</distance></data>");
    server.send(200, "text/xml", XML);
}

void showSettings()
{
    String HTML;
    HTML = F("<?xml version='1.0'?><data><name>");
    HTML += Name;
    HTML += F("</name><einheit>");
    HTML += settings.einheit;
    HTML += F("</einheit><durchmesser>");
    HTML += settings.durchmesser;
    HTML += F("</durchmesser><massstab>");
    HTML += settings.massstab;
    HTML += F("</massstab><faktor>");
    HTML += settings.faktor;
    HTML += F("</faktor><pulseproumdrehung>");
    HTML += settings.pulse;
    HTML += F("</pulseproumdrehung><wlan>");
    HTML += client;
    HTML += F("</wlan><ssid>");
    HTML += ssid;
    HTML += F("</ssid></data>");
    server.send(200, "text/xml", HTML);
}

void detectPulse()
{
    if (digitalRead(Sensor))
    {
        if (h)
        {
#ifdef DEBUG_PRINT
            Serial.println("Flanke");
#endif
            h = 0;
            zeit2 = zeit1;
            zeit1 = millis();
            Umdrehungen++;
        }
    }
    else
        h = 1;
}

void setupPulse()
{
    pinMode(Sensor, INPUT);
    zeit1 = millis() - PULSE_TIMEOUT; //Damit die Geschwindigkeit am Anfang 0 ist
    zeit2 = zeit1;
}

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
    if (Zeit > PULSE_TIMEOUT)
        return 0;                    //Timeout für pulse länger als PULSE_TIMEOUT
    float geschw = Radumfang / Zeit; //in mm/ms
    switch (settings.einheit)
    {
    case 1: //mph
        geschw *= 2.236936;
        break;
    case 2:    //m/s
        break; //passt alles da die einheit mm/ms
    default:   //km/h
        geschw *= 3.6;
        break;
    }
    return geschw;
}

float calcDistance()
{
    return Umdrehungen * Radumfang / 1000;
}

float addScale(float value)
{
    value *= settings.massstab; //Multiplizieren mit dem Massstab
    value *= settings.faktor;   //Multiplizieren mit dem Faktor
    return value;
}