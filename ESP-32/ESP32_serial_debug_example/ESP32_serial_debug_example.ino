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
    //Serial.begin(9600);
    Serial.begin(115200); // this is the default baud rate where the controller sends boot info
    while(!Serial){delay(500);}  // wait for Arduino boards to connect serial to USB 
    delay(500);   // give the serial interface a chance to come up properly after SW download
    
    Serial.println("** ESP32_serial_debug_example **");

    // Set debug output
    Serial.setDebugOutput(true);
    Serial.println("Debug Output set to Serial_0.");

// No Serial 1 interface available on the large ESP board
/*
    //Serial1.begin(9600);
    //Serial1.begin(115200);
    delay(500);
    if (Serial1){
      Serial.println("Serial_1 object exists.");
      if (Serial1.available()){Serial.println("Serial_1 data is available.");} else {Serial.println("Serial_1 no data available.");}
    }
    else {
      Serial.println("Serial_1 object doesn't exist.");
    }
*/
  
    //Serial2.begin(9600);
    Serial2.begin(115200);
    delay(500);
    if (Serial2){
      Serial.println("Serial_2 object exists.");
      if (Serial2.available()){Serial.println("Serial_2 data is available.");} else {Serial.println("Serial_2 no data to read.");}
    }
    else {
      Serial.println("Serial_2 object doesn't exist.");
    }
    
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
int debugOutput = 0;

void loop()
{
    i++;
    int inBytes = 0;
    String inStr = "";
    
    Serial.print("Hi, I'm Serial (0) on standard TX RX pin! --> ");
    Serial.println(i);
    inBytes = Serial.available();
    if (inBytes){
      Serial.println("Serial_0 data is available: " + String(inBytes) + " bytes.");
      inStr = Serial.readString();
      Serial.println("Input read: <" + inStr + ">.");
      if (debugOutput != 0) {
        Serial.setDebugOutput(true);
        debugOutput = 0;
        Serial2.println ("Routing DebugOutput to Serial_0");
        Serial.println("Routing DebugOutput to here");
      }      
    } else {Serial.println("Serial_0 no data to read.");}
    
    //Serial1.print("Hi, I'm Serial_1 on GPIO17! --> ");
    //Serial1.println(i);

    Serial2.print("Hi, I'm Serial_2 on GPIO17/16! --> ");
    inStr = Serial2.println(i);

    inBytes = Serial2.available();
    if (inBytes){
      Serial.println("Serial_2 data is available: " + String(inBytes) + " bytes.");
      Serial2.println("Serial_2 data is available: " + String(inBytes) + " bytes.");
      inStr = Serial2.readString();
      Serial.println("Input read: <" + inStr + ">.");
      Serial2.println("Input read: <" + inStr + ">.");

      if (debugOutput != 2) {
        Serial2.setDebugOutput(true);
        debugOutput = 2;
        Serial.println ("Routing DebugOutput to Serial2");
        Serial2.println("Routing DebugOutput to here");
      }
     }
     else {
      Serial.println("Serial_2 no data to read.");
     }

    delay(100);

    log_v("Verbose");
    log_d("Debug");
    log_i("Info");
    log_w("Warning");
    log_e("Error");

    delay(5000);
//    delay(1000);
}
