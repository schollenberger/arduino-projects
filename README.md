# arduino-projects
  Simple Arduino projects

  Documentation of the Arduino the CC language:
    https://www.arduino.cc/reference/en/

  Link Arduino Libraries and their documentation:
    https://www.arduino.cc/reference/en/libraries/

  Use either the Ardiuino IDE (Windows 10 app) or the CLI `arduino-cli` to run
  the .ino files in this repository.

  The project directories have been written for an Arduino Nano device.
  There are subdirectories for code written for Arduino compatible devices
  like the ESP32 and the ESP-8266 controllers.
  Most of the code is portable but there are slight differences.
  Code written for a specific device may or may not run on another one.
  Therefore you may find similar projects in these directories doing the
  same think on different devices.

## Links to Arduino Adafruit libraries

  Wire:
      https://www.arduino.cc/reference/en/language/functions/communication/wire/

   OLED:
      http://adafruit.github.io/Adafruit-GFX-Library/html/index.html

   INA219 board
      https://github.com/adafruit/Adafruit_INA219
      http://adafruit.github.io/Adafruit_INA219/html/class_adafruit___i_n_a219.html


## Initial Setup Arduino Nano

  On delivery the Arduino Nano provided by AZDelivery runs the program
  "ASCII Table ~ Character Map" which you find in the Arduino IDE examples
  (File -> Examples -> 04 Communication -> ASCII Table).
  This program prints the ASCII table on the serial USB port on 9600 Baud.

  In order to upload a program successfully, on the Arduino IDE, you have to
  select for the processor: "ATmega 328P (Old Bootloader)". Board set to
  "Arduino Nano".
  Try to download the "Blink" example, The red LED will light up 2 times during
  the download and then the red LED will bink in 1 sec on/off intervals.

## LEDs on the Arduino Nano

  The Arduino Nano has 4 LEDs.
     x red   - GPIO (LED_BUILTIN)
     x green - Power
     x red   - serial TX
     # green - serial Rx

  ```
  Arduino Nano board
   hole -+  15  14  13  12  11  10   9   8   7   6   5   4   3   2   1   +- hole
      +--|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|--+
      |  o   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   o  |
      |     7V GND RST  5V  A7  A6  A5  A4  A3  A2  A1  A0 AREF 3.3V D13    |
      |                            SCL SDA                                  |
      |                 O LED green  GPIO                        +-------+  |
      |                 O LED red    Pwr      X Reset            |  USB  |  |
      |                 O LED red    Tx       X                  |       |  |
      |                 O LED green  Rx                          +-------+  |
      |     Rx  Tx                                                          |
      |     D1  D0 RST GND  D2  D3  D4  D5  D6  D7  D8  D9 D10 D11 D12      |
      |  o   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   o  |
      +--|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|--+
    hole-+  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30   +- hole

  ```

## I2C Connections

  For connecting I2C devices to the Arduino Nano use:

  Arduino      I2C
    A4 (8)  -  SDA
    A5 (9)  -  SCL


## Some coding memos for implicit features (device independent)

### Print to serial interface
  It is good practice to write debug information to the serial interface.
  You can use the IDE's serial monitor to see the content.

  Example in setup():
  ```
  Serial.begin(9600);
  Serial.println("Hello World.");
  ```
