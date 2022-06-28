/*
 *  ESP32 DOIT DEV KIT v1 - ESP32 WeMos LOLIN32
 *  Debug on standard Serial
 *  Based on work
 *  by Mischianti Renzo <https://www.mischianti.org>
 *
 *  https://www.mischianti.org/
 * 
 * modified 2022-05-20 by Werner Schollenberger
 */
 
#include "Arduino.h"
#include <WiFi.h>
 
//#define STA_SSID "<YOUR-SSID>"
//#define STA_PASS "<YOUR-PASSWD>"
#include <WSB_private.h> // set the WLAN credentials outside the source code
 
#include "esp32-hal-log.h"
 
void setup()
{
    // put your setup code here, to run once:
//    Serial.begin(9600);
    Serial.begin(115200);
    Serial.setDebugOutput(true);
//    Serial2.begin(9600);
    Serial.println("START WIFI");
    WiFi.begin(STA_SSID, STA_PASS);
 
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    delay(1000);
}
 
int i = 0;
void loop()
{
    // put your main code here, to run repeatedly:
    i++;
    Serial.print("Hi, I'm Serial on standard TX RX pin! --> ");
    Serial.println(i);
//    Serial1.print("Hi, I'm Serial2 on GPIO17! --> ");
//    Serial1.println(i);
    delay(100);
    log_v("Verbose");
    log_d("Debug");
    log_i("Info");
    log_w("Warning");
    log_e("Error");
    delay(5000);
//    delay(1000);
}
