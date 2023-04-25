/*
 *  ESP32
 *  DEEP Sleep and wake up
 *  Based on work
 *  by Mischianti Renzo <https://www.mischianti.org>
 *
 *  https://www.mischianti.org/2021/03/15/esp32-practical-power-saving-deep-sleep-and-hibernation-3/
 * 
 * modified 2022-05-20 by Werner Schollenberger
 * 
 * 2022-09-27 WSB - Added monitoring of battery voltage via ADC2_04
 *                  and logging to Serial2
 */

// constants won't change. Used here to set a pin number:
//const int gpioPin =  4;      // GPIO4 - Touch_0 - ADC2_0
const int gpioPin =  13;       // WEMOS ESP Lite GPI13 - Touch_4 - ADC2_4
const byte adcBattPin = 33;    // ADC input WEMOS Lite ADC1-5 to measure battery voltage
                               //(Note that ADC2 cannot be used when WiFi is on)
const byte adcSensorPin = 36;  // ADC input WEMOS Lite ADC1-0 - Sensor reading 

// Duration before going into sleep
//const int wait_delay = 20;
const int wait_delay = 10;
// Sleep duration
//const int sleep_delay = 30;
const int sleep_delay = 10;


#include <WiFi.h>
//#include <BluetoothSerial.h>
#include "driver/adc.h"
//#include <esp_bt.h>
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
    default : Serial.printf("Wakeup was not caused by deep sleep: %d",wakeup_reason); Serial.println(); break;
  }
}

// Function for the linear adjustment of the ADC.
double analogReadAdjusted(byte pinNumber){

  // Specify the adjustment factors.
  const double f1 = 1.7111361460487501e+001;
  const double f2 = 4.2319467860421662e+000;
  const double f3 = -1.9077375643188468e-002;
  const double f4 = 5.4338055402459246e-005;
  const double f5 = -8.7712931081088873e-008;
  const double f6 = 8.7526709101221588e-011;
  const double f7 = -5.6536248553232152e-014;
  const double f8 = 2.4073049082147032e-017;
  const double f9 = -6.7106284580950781e-021;
  const double f10 = 1.1781963823253708e-024;
  const double f11 = -1.1818752813719799e-028;
  const double f12 = 5.1642864552256602e-033;

  // Specify the number of loops for one measurement.
  //const int loops = 40;
  const int loops = 4;

  // Specify the delay between the loops.
  //const int loopDelay = 200;
  const int loopDelay = 50;

  // Initialize the used variables.
  int counter = 1;
  int inputValue = 0;
  double totalInputValue = 0;
  double averageInputValue = 0;

  // Loop to get the average of different analog values.
  for (counter = 1; counter <= loops; counter++) {

    // Read the analog value.
    inputValue = analogRead(pinNumber);

    // Add the analog value to the total.
    totalInputValue += inputValue;

    // Wait some time after each loop.
    delay(loopDelay);
  }

  // Calculate the average input value.
  averageInputValue = totalInputValue / loops;

  // Calculate and return the adjusted input value.
  return f1 + f2 * pow(averageInputValue, 1) + f3 * pow(averageInputValue, 2) + f4 * pow(averageInputValue, 3) + f5 * pow(averageInputValue, 4) + f6 * pow(averageInputValue, 5) + f7 * pow(averageInputValue, 6) + f8 * pow(averageInputValue, 7) + f9 * pow(averageInputValue, 8) + f10 * pow(averageInputValue, 9) + f11 * pow(averageInputValue, 10) + f12 * pow(averageInputValue, 11);
}

void setup() {
    Serial.begin(115200);
    while(!Serial){delay(500);}  // wait for Arduino boards to connect serial to USB 
    delay(500);  // give the serial interface on a ESP module a chance to come up properly after SW download
    
    Serial.println("** ESP32_Deep_Sleep_and_Wakeup ** - Log output see on Serial port 2");

    Serial = Serial2;
    Serial.begin(115200);
    while(!Serial){delay(500);}  // wait for Arduino boards to connect serial to USB 
    delay(500);  // give the serial interface on a ESP module a chance to come up properly after SW download
    Serial.println("** ESP32_Deep_Sleep_and_Wakeup on Serial2 **");
    print_wakeup_reason();
 
//    SerialBT.begin("ESP32test"); //Bluetooth device name
//    SerialBT.println("START BT");

    // set the digital pin as output:
    pinMode(gpioPin, OUTPUT);
    digitalWrite(gpioPin, HIGH);

/* 
    Serial.print("START WIFI .");
    WiFi.begin(STA_SSID, STA_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.print("Initial Variable value = ");
    Serial.println(variable);
    variable += 10;
    Serial.print("Variable updated = ");
    Serial.println(variable);
*/
    delay(500);
 
    // Get the adjusted input value with the help of the 'analogReadAdjusted' function.
    float adjustedInputValue = analogReadAdjusted(adcBattPin);
    // Calculate the adjusted voltage from the adjusted analog input value.
    float adjustedInputVoltage = 3.3 / 4096 * adjustedInputValue;
    // Calculate battery voltage assuming voltage divider of: 220kOhm + 330kOhm || 15 MOhm (ADC)
    // Total resistance = 543 kOhm, resistor at ADC = 323 kOhm
    float vBatt = adjustedInputVoltage * 543 / 323;

    //Serial.print("Battery State: raw ADC value: ");
    //Serial.print(inputValue);
    //Serial.print(" | raw corrected: ");
    //Serial.print(adjustedInputValue, 1);
    //Serial.print(" | Voltage: ");
    //Serial.print(inputVoltage, 3);
    //Serial.print(" | corrected voltage: ");
    //Serial.print(adjustedInputVoltage, 3);
    //Serial.print(" | Delta %: ");
    //Serial.println((adjustedInputValue - inputValue) / inputValue * 100, 2);
    //Serial.println();

    Serial.println();
    Serial.print("Battery Voltage: ");
    Serial.println(vBatt, 2);
    Serial.println();
    delay(500);
        
    Serial.println("Check that Sensor is deactivated.. - you have 10 sec.");
    delay(10*1000);
    Serial.println("Activating Sensor.");
    digitalWrite(gpioPin, LOW);
    delay(500);
    // Get the 'normal' value from the sensor analog input. 
    int inputValue = analogRead(adcSensorPin);
    // Calculate the voltage from the 'normal' analog input value.
    float inputVoltage = 3.3 / 4096 * inputValue;
    Serial.println();
    Serial.print("Sensor Voltage:  ");
    Serial.println(inputVoltage, 2);
    Serial.println();
    delay(500);
    
    Serial.println("Waiting " + String(wait_delay) + " sec before truning into deep sleep...");
    delay(wait_delay * 1000);

        // Get the 'normal' value from the sensor analog input. 
    inputValue = analogRead(adcSensorPin);
    // Calculate the voltage from the 'normal' analog input value.
    inputVoltage = 3.3 / 4096 * inputValue;
    Serial.println();
    Serial.print("Sensor Voltage after some wait time:  ");
    Serial.println(inputVoltage, 2);
    Serial.println();
    delay(500);
    
    uint64_t sleep_delay_us = sleep_delay * 1000ULL * 1000ULL;
    esp_sleep_enable_timer_wakeup(sleep_delay_us);
    Serial.println("DEEP SLEEP (HIBERNATE) ENABLED FOR " + String(sleep_delay) + " secs");

    // Pay attention to this command, with this wake up source
    // your device can't go to hibernate mode
    // but only deep sleep
    //esp_sleep_enable_ext0_wakeup(GPIO_NUM_33,1);  // GPIO_NUM_33 for ESP32 Node CPU board / GPIO_NUM_12 for LoLin board
    
    delay(100);
    //disableWiFi();
    esp_deep_sleep_start();
    //esp_light_sleep_start();
    
    Serial.println();
    Serial.println("DEEP SLEEP WAKE UP");
 
    Serial.print("Variable = ");
    Serial.println(variable);
    //enableWiFi();

    Serial.println("Here we are back again - exiting setup().");
 
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
 
    Serial.print("START WIFI after enable .");
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
/* 
 void disableBluetooth(){
    btStop();
    esp_bt_controller_disable();
    delay(1000);
    Serial.println("BT STOP");
}
*/
