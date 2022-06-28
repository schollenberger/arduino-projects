/*
  InaTest

  Displays Bus Voltage and Current of the INA219 sensor board together with the Bus Voltage
  measured by the Analog Pin 0 and prints the result to the OLED Display.

  Requires an INA219 breakout board.
  OLED I2C display must use SH110x controller.

*/


#include <Wire.h>
//#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_INA219.h>

#define oled_i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
#define ina_i2c_Address  0x40 //initialize with the I"C addr 0x40 typically INA219 board (Current sensor)

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_INA219 ina219;

float VCC = 4.67;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("InaTest");

  delay(250); // wait for the OLED to power up
  display.begin(oled_i2c_Address, true); // Address 0x3C default
  // Wire.setClock(400000); // increase I2C clock speed to 400 kHz
  
  //display.setContrast (0); // dim display
 
  // display.display(); shows initial Adafruit splash screen
  // delay(2000);

  // Clear the buffer.
  // display.clearDisplay();
  
  // Intro screen
  printIntro();
  delay(2000);
  
  // invert the display
  //display.invertDisplay(true);
  //delay(1000);
  //display.invertDisplay(false);
  //delay(1000);
  
  // Configure the Analog reference
  //analogReference(EXTERNAL);  // use AREF for reference voltage (must be at least 1.1V)
  //analogReference(DEFAULT);   // reset to internal 5V reference

  // Configure the INA219 sensor
  if (! ina219.begin()) {
     printError("Failed to find", "INA219 chip", "");
     while (1) { delay(10); }
  }
  // To use the highest 32V, 1A range (current unit: 0,1mA, power unit 2mW, overflow at 3.2A):
  ina219.setCalibration_32V_2A();
  char buf1[10];
  dtostrf(VCC, 4, 2, buf1);
  char buf[20];
  strcpy(buf, "VREF = ");
  strcat(buf, buf1);
  strcat(buf, " V.");
  //snprintf(buf, sizeof(buf), "VREF = %f", VCC);  // Analog input reference

  printError("INA219 Calibration:", "32V / 2 A", buf);

  // To use a slightly lower 32V, 1A range (higher precision on amps 0.04mA, 0.8mW, overflow at 1.3A):
  //ina219.setCalibration_32V_1A();
  //printError("INA219 Calibration:", "32V / 1 A");

  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();
  //printError("INA219 Calibration:", "16V / 400mA");
  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (VCC / 1023.0);
  
  printScreen(shuntvoltage, busvoltage, current_mA, power_mW, loadvoltage, voltage);
  delay(500);
}

// local subroutines

void printIntro() {
  display.clearDisplay();
  display.drawRect(0, 0, display.width() - 1, display.height() - 1, SH110X_WHITE);
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  //display.setCursor(5, 5);
  //display.print("");
  //display.display();
  //delay(2000);
  display.setCursor(5, 25);
  display.print("Ina Test");
  display.display();
  //delay(2000);
  //display.setCursor(5, 45);
  //display.print("Simple");
  //display.display();
}

void printScreen(float shuntvoltage, float busvoltage, float current_mA, float power_mW, float loadvoltage, float voltage ) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  // text display 
  display.setCursor(5,5);
  display.print("Ubus:    "); display.print(busvoltage); display.print(" V");
  display.setCursor(5,15);
  display.print("Ushunt:  "); display.print(shuntvoltage); display.print(" mV");
  display.setCursor(5,25);
  display.print("Uload:   "); display.print(loadvoltage); display.print(" V");
  display.setCursor(5,35);
  display.print("Current: "); display.print(current_mA); display.print(" mA");
  display.setCursor(5,45);
  display.print("Power:   "); display.print(power_mW); display.print(" mW");
  display.setCursor(5,55);
  display.print("Uanalog: "); display.print(voltage); display.print(" V");
  
  display.display();
}

void printError(const char *line1, const char *line2, const char *line3) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  // text display 
  display.setCursor(5, 3);
  display.print("Ina Test");
  display.setCursor(5, 20);
  display.print(line1);
  display.setCursor(5, 30);
  display.print(line2);
  display.setCursor(5, 40);
  display.print(line3);
  display.display();
}
