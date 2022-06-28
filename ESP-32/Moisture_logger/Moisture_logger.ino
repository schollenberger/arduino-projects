/*********
 * Log moisture levels periodically fall into deep sleep in the
 * meantime.
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
const int analogIn = 36;  // ADC1-0

// ADC parameters
const float adc_ref = 3.33;
const int   adc_range = 4095;

// Digital Output to drive Sensor
// const int digitalOutPin =  17;  // G17

// Sensor Server
const char* serverName = "http://192.168.178.100/sensor_values.php";  // pizero.wlan.local
String token = "35c17e3e-88fe-4978-8930-080264f8c615";
String sensorName = "Moisture-ESP";
String sensorLocation  = "Work Room";


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
    Serial.println("Moisture String: " + moistureString);
    return moistureString;
//  }
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
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  while(!Serial){delay(500);}
  Serial.println("** Moisture_logger **");

  print_wakeup_reason();

  // set the digital pin as output:
  //Serial.println("Power Up Moisture Sensor.");
  //pinMode(digitalOutPin, OUTPUT);
  //digitalWrite(digitalOutPin, LOW);
  //delay(20000);
  
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
    Serial.print("HTTP Response code: ");
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
  
  //Serial.println("Power Down Moisture Sensor.");
  //digitalWrite(digitalOutPin, HIGH);
  //delay(5000);
  
  //Send an HTTP POST request periodically
  if (httpResponseCode == 200) {
    int sleep_delay = 60;
    uint64_t sleep_delay_us = sleep_delay * 60ULL * 1000ULL * 1000ULL;
    Serial.println("HTTP Request successful - entering deep sleep mode for " + String(sleep_delay) + " min.");
//    uint64_t sleep_delay_us = sleep_delay * 3600ULL * 1000ULL * 1000ULL;
//    Serial.println("HTTP Request successful - entering deep sleep mode for " + String(sleep_delay) + " hours.");
    esp_sleep_enable_timer_wakeup(sleep_delay_us); // in microseconds 
  }
  else {
    int sleep_delay = 40;
    uint64_t sleep_delay_us = sleep_delay * 60ULL * 1000ULL * 1000ULL;
    Serial.println("HTTP error received - entering deep sleep mode  for " + String(sleep_delay) + " min.");
    esp_sleep_enable_timer_wakeup(sleep_delay_us); // in microseconds 
  }
  esp_deep_sleep_start();
}
 
void loop(){
}
