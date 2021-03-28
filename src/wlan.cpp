#include "wlan.hpp"

String ssid = "";
String password = "";
bool client = 0;

void handleWlanSetting()
{
    String tmp;
    tmp = server.arg("wlan");
    if (tmp.length() > 0)
    {
        if (server.arg("ssid") != "")
            ssid = server.arg("ssid");
        else
            genSSID();

        if (tmp.toInt() == 1) //client
        {
            client = 1;
            password = server.arg("pass");
        }
        else
        {
            client = 0;
            password = "";
        }
        writeSettings();
        startWifi();
    }

    File file = LittleFS.open("/erfolg.html", "r"); //läd wieder die Startseite
    server.streamFile(file, "text/html");
    file.close();
}

void genSSID()
{
    ssid = "Wagen-" + String(ESP.getChipId());
    password = "";
    client = 0;
}

void startWifi()
{
    printWlan();
    if (client)
    {
#ifdef DEBUG_PRINT
        Serial.println();
        Serial.print("Connecting ");
#endif
        WiFi.begin(ssid.c_str(), password.c_str());
        unsigned long zeit = millis() + 30000;
        while (!WiFi.isConnected() && millis() < zeit)
        {
            delay(500);
#ifdef DEBUG_PRINT
            Serial.print(".");
#endif
#ifdef DEBUG_LED
            digitalWrite(DEBUG_LED, !digitalRead(DEBUG_LED));
#endif
        }
        WiFi.mode(WIFI_STA);
#ifdef DEBUG_LED
        digitalWrite(DEBUG_LED, HIGH);
#endif
    }
    if (!WiFi.isConnected())
    {
#ifdef DEBUG_LED
        digitalWrite(DEBUG_LED, LOW);
#endif
        WiFi.disconnect();
        WiFi.mode(WIFI_AP);
        if (client)
        {
#ifdef DEBUG_PRINT
            Serial.println("Failsafe");
#endif
            genSSID();
            printWlan();
        }
        WiFi.softAP(ssid, "");
    }
}

void printWlan()
{
#ifdef DEBUG_PRINT
    Serial.println("WLAN:");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Mode (");
    Serial.print(client);
    Serial.print("): ");
    if (client)
    {
        Serial.println("Client");
        Serial.print("Password: ");
        Serial.print(password);
    }
    else
    {
        Serial.println("Hotspot");
    }
#endif
}