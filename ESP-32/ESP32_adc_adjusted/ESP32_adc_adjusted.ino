/**  
 *   from: https://bitbucket.org/Blackneron/esp32_adc/src/master/ESP32_ADC/ESP32_ADC.ino
 *   
 *   Read adjusted analog value - compensates the ADC's non-linearity   
 */


void setup() {
  Serial.begin(115200);
  while(!Serial){delay(500);}  // wait for Arduino boards to connect serial to USB 
  delay(500);  // give the serial interface on a ESP module a chance to come up properly after SW download

  Serial.println("** ESP32_adc_adjusted **");
    

  // If your ESP32 runs on battery and you have to use Serial2 to log output
  // Connect an Serial2USB converter and uncomment the following section.
  
  Serial.println("- Log output see on Serial port 2");
  Serial = Serial2;
  Serial.begin(115200);
  while(!Serial){delay(500);}
  delay(500); 
  Serial.println("** ESP32_adc_adjusted **");
  
}

void loop() {

  // Specify constant for analog input
//  const byte pinNumber = 36;  // ADC input
//  const byte pinNumber = 36;  // ADC input WEMOS Lite VP (ADC1-0)
//  const byte pinNumber = 39;  // ADC input WEMOS Lite ADC1-3
  const byte pinNumber = 4;  // ADC input WEMOS Lite ADC2-0
  
  // Get the 'normal' value from the analog input. 
  int inputValue = analogRead(pinNumber);

  // Calculate the voltage from the 'normal' analog input value.
  float inputVoltage = 3.3 / 4096 * inputValue;

  // Get the adjusted input value with the help of the 'analogReadAdjusted' function.
  float adjustedInputValue = analogReadAdjusted(pinNumber);

  // Calculate the adjusted voltage from the adjusted analog input value.
  float adjustedInputVoltage = 3.3 / 4096 * adjustedInputValue;

  // Display some information via serial monitor.
  Serial.print("Input: ");
  Serial.print(inputValue);
  Serial.print(" | I new: ");
  Serial.print(adjustedInputValue, 3);
  Serial.print(" | Voltage: ");
  Serial.print(inputVoltage, 3);
  Serial.print(" | V new: ");
  Serial.print(adjustedInputVoltage, 3);
  Serial.print(" | Delta %: ");
  Serial.println((adjustedInputValue - inputValue) / inputValue * 100, 2);

  // Wait some time between the measurements.
  delay(500);
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
