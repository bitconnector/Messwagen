#include "xml.hpp"

volatile unsigned long zeit1, zeit2;
bool h = 1;
long Umdrehungen;

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

void detectPulse()
{
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

void setupPulse()
{
    pinMode(Sensor, INPUT);
    zeit1 = millis() - 30000; //Damit die Geschwindigkeit am Anfang 0 ist
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
