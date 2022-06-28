/*
  MoistureDisplaySimple

  Reads an analog input on pin 0 (Moisture sensor), converts it to voltage, 
  and prints the result to the OLED Display.
  Attach the Moisture sensor output to pin A0, and power it with +5V and ground.
  
  OLED I2C display must use SH110x controller.
  
*/

#include <Wire.h>
//#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void printIntro() {
  display.clearDisplay();
  display.drawRect(0, 0, display.width() - 1, display.height() - 1, SH110X_WHITE);
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(5, 5);
  display.print("Moisture");
  display.display();
  delay(2000);
  display.setCursor(5, 25);
  display.print("Display");
  display.display();
  delay(2000);
  display.setCursor(5, 45);
  display.print("Simple");
  display.display();
}

void printScreen1(float voltage ) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  // text display 
  display.setCursor(5, 3);
  display.print("Moisture Display");
  display.setCursor(5, 20);
  display.print(voltage);
  display.display();
}

void setup() {
  // put your setup code here, to run once:

  delay(250); // wait for the OLED to power up
  display.begin(i2c_Address, true); // Address 0x3C default
  //display.setContrast (0); // dim display

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.

  // display.display(); shows initial Adafruit splash screen
  // delay(2000);

  // Clear the buffer.
  // display.clearDisplay();

  // Intro screen
  printIntro();  // clears the buffer at the begining
  delay(2000);
  
  // invert the display
  //display.invertDisplay(true);
  //delay(1000);
  //display.invertDisplay(false);
  //delay(1000);
  //display.clearDisplay();
}

void loop() {
  // put your main code here, to run repeatedly:

  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);
  
  printScreen1(voltage);
  delay(500);
}
