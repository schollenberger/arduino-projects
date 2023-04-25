/*
 *  ESP8266
 *  DEEP Sleep and wake up 
 *  
 *  In the setup part WIFI gets enabled for realistic current measurements.
 *  During wake-up time, the controller blinks with its builtin LED.
 *  Then the controller goes int deep sleep for some time.
 *  
 *  This is as well an example for active waiting in the loop() function.
 *  
 *  Note that with the ESP8266, the RTC only sets the D0 output to low when 
 *  programmed a wakeup time in the ESP.deepSleep() command. 
 *  You need to wire it to the RESET pin. However this connection prevents the 
 *  controller from being programmed as the D0 output us used there as well.
 *  
 *  Created: 2022-07-07 by Werner Schollenberger
 */

//#include  <ESP8266WiFi.h>
#include <esp_sleep.h>
 
#define STA_SSID "<YOUR-SSID>"
#define STA_PASS "<YOUR-PASSWD>"
#include <WSB_private.h> // set the WLAN credentials outside the source code

//BluetoothSerial SerialBT;

// constants won't change.
// Used here to set a pin number:
const int ledPin =  LED_BUILTIN;// the number of the LED pin

// some timing constants upfront

const long ledOnInterval  = 2;           // interval at which the LED is on (seconds)
const long ledOffInterval = 1;           // interval at which the LED is off (seconds)

const long sleepDelay = 20;               // in seconds
const long sleepTime  = 30;               // in seconds


// Variables will change:
int ledState = HIGH;             // ledState used to set the LED, 
                                 // note that the LED is active low

// static variables save data across loop iterations

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousLedMillis = 0;        // will store last time LED was updated

unsigned long previousSleepMillis = 0;        // will store last time LED was updated


void setup() {
    //Serial.begin(115200);
    Serial.begin(74880);    // the baud rate in which the board boots
    while(!Serial){delay(500);}  // wait for Arduino boards to connect serial to USB 
    delay(500);  // give the serial interface on a ESP module a chance to come up properly after SW download
    
    Serial.println("** ESP8266_Deep_Sleep_and_Wakeup **");

    // Initialize the LED
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, ledState);
    Serial.println("LED enabled (still off).");
    
    Serial.print("START WIFI");
    WiFi.begin(STA_SSID, STA_PASS);
 
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
 
    delay(100);

    //int wait_delay = 5;
    //Serial.println("Waiting " + String(wait_delay) + " sec before truning into deep sleep...");
    //delay(wait_delay * 1000);
    
    ledState = LOW;
    digitalWrite(ledPin, ledState);
    Serial.println("Switched LED on.");
    delay(100);
   
    // initialize time stamps before entering the loop() function
    previousLedMillis = previousSleepMillis = millis();
    
    Serial.println("Exiting setup() - will enter deep sleep state in "+ String(sleepDelay) +" seconds...");

}

int wifiStatus = 1;

void loop() {
  // put code here  that runs on every loop
  //Serial.print(".");
  //delay(100);
  // put code here that runs at certain intervals
  unsigned long currentMillis = millis();

  if (currentMillis - previousSleepMillis >= sleepDelay * 1000) {
    previousSleepMillis = currentMillis;

/*
    uint64_t sleepTime_us = sleepTime * 1000ULL * 1000ULL;
    Serial.println("DEEP SLEEP (HIBERNATE) ENABLED FOR " + String(sleepTime) + " secs");
    ESP.deepSleep(sleepTime_us);
*/
    if (wifiStatus == 1) {
      disableWiFi();
      wifiStatus = 0;
    }
    else {
      enableWiFi();
      wifiStatus = 1;
    }
  }
  // LED timings depend on state
  if (ledState == HIGH) {  // LED off
    if (currentMillis - previousLedMillis >= ledOffInterval * 1000) {
      //Serial.println("DEBUG: ledState = " + String(ledState) + " - currentMillis = " + String(currentMillis) + " - previousLedMillis = " + String(previousLedMillis)+".");
      previousLedMillis = currentMillis;
      ledState = LOW;
      digitalWrite(ledPin, ledState);
      Serial.println("Switched LED on.");
      //Serial.println("DEBUG2: ledState = " + String(ledState) + " - currentMillis = " + String(currentMillis) + " - previousLedMillis = " + String(previousLedMillis)+".");
    }  
  }
  else {   // LED on
    if (currentMillis - previousLedMillis >= ledOnInterval * 1000) {
      previousLedMillis = currentMillis;
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
      Serial.println("Switched LED off.");
    }  
  } 
}  // loop()


 
void disableWiFi(){
    WiFi.disconnect();      // Disconnect from the network
    WiFi.forceSleepBegin();
    delay(1);
    Serial.println("");
    Serial.println("WiFi disconnected!");
}

void enableWiFi(){
    WiFi.forceSleepWake();
    Serial.print("START WIFI after enable");
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
