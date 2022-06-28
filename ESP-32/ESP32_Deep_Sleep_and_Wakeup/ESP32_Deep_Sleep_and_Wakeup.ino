/*
 *  ESP32
 *  DEEP Sleep and wake up
 *  Based on work
 *  by Mischianti Renzo <https://www.mischianti.org>
 *
 *  https://www.mischianti.org/2021/03/15/esp32-practical-power-saving-deep-sleep-and-hibernation-3/
 * 
 * modified 2022-05-20 by Werner Schollenberger
 */
 
#include <WiFi.h>
#include <BluetoothSerial.h>
#include "driver/adc.h"
#include <esp_bt.h>
#include <esp_wifi.h>
#include <esp_sleep.h>
 
#define STA_SSID "<YOUR-SSID>"
#define STA_PASS "<YOUR-PASSWD>"
#include <WSB_private.h> // set the WLAN credentials outside the source code

//BluetoothSerial SerialBT;
 
int variable = 0;

/*
 * Method to print the reason by which ESP32 has been awaken from sleep
 * From: Pranav Cherukupalli <cherukupallip@gmail.com>
 * https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/DeepSleep/TimerWakeUp/TimerWakeUp.ino#L22
 */
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}


void setup() {
    Serial.begin(115200);
 
    while(!Serial){delay(500);}

    print_wakeup_reason();
 
//    SerialBT.begin("ESP32test"); //Bluetooth device name
//    SerialBT.println("START BT");
 
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
 
    Serial.print("Initial Variable value = ");
    Serial.println(variable);
 
    variable += 10;
 
    Serial.print("Variable updated = ");
    Serial.println(variable);
    int wait_delay = 20;
    Serial.println("Waiting " + String(wait_delay) + " sec before truning into deep sleep...");
    delay(wait_delay * 1000);
    
    int sleep_delay = 30;
    uint64_t sleep_delay_us = sleep_delay * 1000ULL * 1000ULL;
    esp_sleep_enable_timer_wakeup(sleep_delay_us);
    Serial.println("DEEP SLEEP (HIBERNATE) ENABLED FOR " + String(sleep_delay) + " secs");

    // Pay attention to this command, with this wake up source
    // your device can't go to hibernate mode
    // but only deep sleep
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_33,1);
    
    delay(100);
    esp_deep_sleep_start();
 
    Serial.println();
    Serial.println("DEEP SLEEP WAKE UP");
 
    Serial.print("Variable = ");
    Serial.println(variable);
}
 
void loop() {
 
}
 
void disableWiFi(){
    adc_power_off();
    WiFi.disconnect(true);  // Disconnect from the network
    WiFi.mode(WIFI_OFF);    // Switch WiFi off
 
    Serial.println("");
    Serial.println("WiFi disconnected!");
}
void enableWiFi(){
    adc_power_on();
    WiFi.disconnect(false);  // Reconnect the network
    WiFi.mode(WIFI_STA);    // Switch WiFi on
 
    Serial2.println("START WIFI");
    WiFi.begin(STA_SSID, STA_PASS);
 
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial2.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
/* 
 void disableBluetooth(){
    btStop();
    esp_bt_controller_disable();
    delay(1000);
    Serial.println("BT STOP");
}
*/
