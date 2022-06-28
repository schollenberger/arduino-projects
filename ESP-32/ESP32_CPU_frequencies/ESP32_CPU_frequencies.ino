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
 
void setup()
{
    Serial.begin(115200);
 
    while (!Serial){
        delay(500);
    }

    int waitTime = 20;
    Serial.print("CPU Freq: ");
    Serial.println(getCpuFrequencyMhz());
    delay(waitTime*1000);
    
    setCpuFrequencyMhz(160);
    Serial.print("CPU Freq: ");
    Serial.println(getCpuFrequencyMhz());
    delay(waitTime*1000);
    
    setCpuFrequencyMhz(80);
    Serial.print("CPU Freq: ");
    Serial.println(getCpuFrequencyMhz());
    delay(waitTime*1000);
    
    setCpuFrequencyMhz(40);
    Serial.print("CPU Freq: ");
    Serial.println(getCpuFrequencyMhz());
    delay(waitTime*1000);
    
    setCpuFrequencyMhz(20);
    Serial.print("CPU Freq: ");
    Serial.println(getCpuFrequencyMhz());
    delay(waitTime*1000);
    
    setCpuFrequencyMhz(10);
    Serial.print("CPU Freq: ");
    Serial.println(getCpuFrequencyMhz());
    delay(waitTime*1000);
}
 
void loop()
{
 
}
