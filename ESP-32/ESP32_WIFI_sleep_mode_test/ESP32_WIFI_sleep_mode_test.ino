/*
 *  ESP32
 *  Manage WiFi sleep mode
 *  Based on work 
 *  by Mischianti Renzo <https://www.mischianti.org>
 *
 *  https://www.mischianti.org
 *
 * You may copy, alter and reuse this code in any way you like, but please leave
 * reference to www.mischianti.org in your comments if you redistribute this code.
 * 
 * modified 2022-05-20 by Werner Schollenberger
 */
 
#include <WiFi.h>
#include <esp_wifi.h>
#include <BluetoothSerial.h>
#include "driver/adc.h"
 
//#define STA_SSID "<YOUR-SSID>"
//#define STA_PASS "<YOUR-PASSWD>"
#include <WSB_private.h> // set the WLAN credentials outside the source code

//BluetoothSerial SerialBT;

void setWiFiPowerSavingMode();
void disableWiFi();
 
void setup() {
    Serial.begin(115200);
 
    while(!Serial){delay(500);}
 
//    SerialBT.begin("ESP32test"); //Bluetooth device name
//    SerialBT.println("START BT");
 
    Serial.println("START WIFI");
    WiFi.begin(STA_SSID, STA_PASS);
 
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
 
    setWiFiPowerSavingMode();
 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("");
    Serial.println("Waiting before cycling WIFI to see power consumption in Power Saving Mode...");
    delay(30000);
    
}
 
void loop() {
    Serial.println("Disabling WIFI after 20 sec...");
    delay(20000);
    disableWiFi();  // aprox. 50 mA
    Serial.println("WIFI disabled, enabling again after 20 sec...");
    delay(20000);
    enableWiFi();  // aprox. 140 mA with no WiFi power saving, goes down w/ PSM (Power Saving Mode)
   
}
 
void setWiFiPowerSavingMode(){
// Current measured when BT is disabled

//    WiFi.setSleep(false);  // no sleep 164 mA - low NW latency
//    WiFi.setSleep(true);  // default (WIFI_PS_MIN_MODEM) - 80 - 110 mA - high NW latency

//     esp_wifi_set_ps(WIFI_PS_MAX_MODEM);  // remains longer at 80 mA
//    esp_wifi_set_ps(WIFI_PS_NONE); // same as "false"
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
}

void disableWiFi(){
    adc_power_off();
    WiFi.disconnect(true);  // Disconnect from the network
    WiFi.mode(WIFI_OFF);    // Switch WiFi off
}

void enableWiFi(){
    adc_power_on();
    WiFi.disconnect(false);  // Reconnect the network
    WiFi.mode(WIFI_STA);    // Switch WiFi on
 
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
}
