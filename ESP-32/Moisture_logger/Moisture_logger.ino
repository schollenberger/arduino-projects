/*********
 * Log moisture levels periodically fall into deep sleep in the meantime.
 * Version for WEMOS LoLin Lite board which runs efficiently on battery.
 *
 * Now the program controls the sensor power via GPIO output and
 * transmits battery voltage as well.
 * On-board LED gets turned on when board is active.
 *********/

// Import required libraries
#include "WiFi.h"
#include <HTTPClient.h>
#include <esp_sleep.h>

// Replace with your network credentials
//const char* ssid = "REPLACE_WITH_YOUR_SSID";
//const char* password = "REPLACE_WITH_YOUR_PASSWORD";
#include <WSB_private.h> // set the WLAN credentials outside the source code
const char* ssid = STA_SSID;
const char* password = STA_PASS;

// Assign input variables to GPIO pins
const int analogIn = 36;  // ADC1-0  for Moisture sensor
const int analogBat = 33; // Battery ADC input WEMOS Lite ADC1-5 (Note that ADC2 cannot be used when WiFi is on)

// ADC parameters
const float adc_ref = 3.30;   // for WEMOS LiLo Lite, was 3.33 for Node CPU board
const int   adc_range = 4095;

// Battery voltage divider correction
const float scaleBatteryVoltage = 543 / 323;  // Assuming voltage divider of: 220kOhm + 330kOhm || 15 MOhm (ADC)
                                              // Total resistance = 543 kOhm, resistor at ADC = 323 kOhm

// Digital Output to drive Sensor
// const int digitalOutPin =  17;  // G17
const int digitalOutPin =  13;  // Wemos LoLin Lite

// Sensor Server

// Server running on PiZero
const char* serverName = "http://192.168.178.100/sensor_values.php";  // pizero.wlan.local
String token = "35c17e3e-88fe-4978-8930-080264f8c615";
String sensorName = "Moisture-ESP";
String sensorLocation  = "Work Room";

/*
// Server running within Docker
const char* serverName = "http://192.168.178.33/sensor_values.php";  // wsdesktop7
String token = "09ba4f19-7629-4cc2-90cd-a0874268b860";
String sensorName = "Moisture-ESP Wemos Lite";
String sensorLocation  = "Test";
*/

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


String readMoistureLevel() {
  int inputRawValue = analogRead(analogIn);
  float inputValue = inputRawValue * (adc_ref / adc_range);
  String moistureString = String(inputValue, 2);
    
  // Check if any reads failed and exit early (to try again).
//  if (isnan(moistureString)) {    
//    Serial.println("Failed to read analog value");
//    return "--";
//  }
//  else {
    Serial.println(" -- Moisture String: " + moistureString);
    return moistureString;
//  }
}


String readBatteryVoltage() {
   float adjustedInputValue = analogReadAdjusted(analogBat);
    // Calculate the adjusted voltage from the adjusted analog input value.
    float adjustedInputVoltage = adjustedInputValue * (adc_ref / adc_range);
    // Calculate battery voltage assuming voltage divider of: 220kOhm + 330kOhm || 15 MOhm (ADC)
    // Total resistance = 543 kOhm, resistor at ADC = 323 kOhm
    float vBat = adjustedInputVoltage * 543 / 323;
    String vBatString = String(vBat, 2);
    Serial.println(" -- Battery Voltage: " + vBatString);
    return vBatString;
}

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
    default : Serial.printf("Wakeup was not caused by deep sleep: %d",wakeup_reason); Serial.println();  break;
  }
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  while(!Serial){delay(500);}
  delay(500);
  Serial.println("** Moisture_logger **");

  // If your ESP32 runs on battery and you have to use Serial2 to log output
  // Connect an Serial2USB converter and uncomment the following section.
  
  Serial.println("- Log output see on Serial port 2");
  Serial = Serial2;
  Serial.begin(115200);
  while(!Serial){delay(500);}
  delay(500);
  Serial.println("** Moisture_logger on Serial2 **");

  print_wakeup_reason();

  // light up internal LED
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Turn on the internal LED.");
  digitalWrite(LED_BUILTIN, LOW);  // Note: the LED on a WEMOS Lite board is active-low

  // Set the digital pin as output:
  Serial.println("Enable Moisture Sensor.");
  pinMode(digitalOutPin, OUTPUT);
  digitalWrite(digitalOutPin, LOW);
  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi..");
  WiFi.begin(ssid, password);
  int count = 40;
  
  while ((WiFi.status() != WL_CONNECTED) && (count > 0)) {
    delay(500);
    count--;
    Serial.print(".");
  }
  if (count > 0) {
    Serial.println(" connected.");
  }
  else {
    Serial.println();
    int sleep_delay = 30;
    uint64_t sleep_delay_us = sleep_delay * 60ULL * 1000ULL * 1000ULL;
    Serial.println("Failed to connect to WIFI - entering deep sleep mode for " + String(sleep_delay) + " min.");
    esp_sleep_enable_timer_wakeup(sleep_delay_us); // in microseconds 
    esp_deep_sleep_start();
  }
   
  // Print ESP32 Local IP Address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  Serial.println("Pushing actual sensor value to Server...");
  WiFiClient client;
  HTTPClient http;
  http.begin(client, serverName);
    
  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization","Bearer " + token);
  
  // Prepare your HTTP POST request data
  String httpRequestData = "sname=" + sensorName
                        + "&slocation=" + sensorLocation + "&svalue=" + readMoistureLevel()
                        + "&sbattery=" + readBatteryVoltage()
                        + "";
  Serial.print("httpRequestData: ");
  Serial.println(httpRequestData);
  
  // Send HTTP POST request
  int httpResponseCode = http.POST(httpRequestData);
  //int httpResponseCode = 200;
  // Read response
  String httpResponsePayload = http.getString();
    
  delay(10); // to calm down the HTTP client and allow serial prints
    
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code Moisture level read: ");
    Serial.println(httpResponseCode);
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  Serial.print("POST request returned:");
  Serial.println(httpResponsePayload);

  // Free resources
  http.end();
  delay(10);
  
  Serial.println("Power Down Moisture Sensor.");
  digitalWrite(digitalOutPin, HIGH);
  delay(500);
  
  //Send an HTTP POST request periodically
  if (httpResponseCode == 200) {
    int sleep_delay = 3; // hours  - was 60 min;
//    uint64_t sleep_delay_us = sleep_delay * 60ULL * 1000ULL * 1000ULL;
//    Serial.println("HTTP Request successful - entering deep sleep mode for " + String(sleep_delay) + " min.");
    uint64_t sleep_delay_us = sleep_delay * 3600ULL * 1000ULL * 1000ULL;
    Serial.println("HTTP Request successful - entering deep sleep mode for " + String(sleep_delay) + " hours.");
    esp_sleep_enable_timer_wakeup(sleep_delay_us); // in microseconds 
  }
  else {
    int sleep_delay = 40;
    uint64_t sleep_delay_us = sleep_delay * 60ULL * 1000ULL * 1000ULL;
    Serial.println("HTTP error received - entering deep sleep mode  for " + String(sleep_delay) + " min.");
    esp_sleep_enable_timer_wakeup(sleep_delay_us); // in microseconds 
  }
  // disable builtin LED
  Serial.println("Turn off the internal LED.");
  digitalWrite(LED_BUILTIN, HIGH);

  esp_deep_sleep_start();
}
 
void loop(){
}
