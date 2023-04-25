/*
 *  ESP32
 *  Manage CPU Frequences
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
 
#include "Arduino.h"


void printBothSerialLines(char* txt) {
  Serial.print(txt);
  Serial2.print(txt);
}

void printlnBothSerialLines(char* txt) {
  Serial.println(txt);
  Serial2.println(txt);
  
}

void printCpuFrequency() 
{
  char buf[10];
  printBothSerialLines("CPU Freq: ");
  sprintf(buf, "%d", getCpuFrequencyMhz());  
  printlnBothSerialLines(buf);
}


void setup()
{
    Serial.begin(115200);
    Serial2.begin(115200);
    while (!Serial){    // mainly required for Arduino boards
        delay(500);
    }
    while (!Serial2){
        delay(500);
    }
    delay(500); // always give 500 ms to initialize for ESP boards
    
    int waitTime = 20;

    char buf[10];
    
    printBothSerialLines("Initial CPU Freq: ");
    sprintf(buf, "%d", getCpuFrequencyMhz());
    printlnBothSerialLines(buf);
    delay(waitTime*1000);
    
    setCpuFrequencyMhz(160);
    printCpuFrequency();
    delay(waitTime*1000);
    
    setCpuFrequencyMhz(80);
    printCpuFrequency();
    delay(waitTime*1000);
    
    setCpuFrequencyMhz(40);
    printCpuFrequency();
    delay(waitTime*1000);
    
    setCpuFrequencyMhz(20);
    printCpuFrequency();
    delay(waitTime*1000);
    
    setCpuFrequencyMhz(10);
    printCpuFrequency();
    delay(waitTime*1000);
}
 
void loop()
{
     delay(1000); 
}
