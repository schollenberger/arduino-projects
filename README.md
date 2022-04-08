# arduino-projects
  Simple Arduino projects

  Documentation of the Arduino the CC language:
    https://www.arduino.cc/reference/en/

  Link Arduino Libraries and their documentation:
    https://www.arduino.cc/reference/en/libraries/

  Use either the Ardiuino IDE (Windows 10 app) or the CLI `arduino-cli` to run
  the .ino files in this repository.


## Links to Arduino Adafruit libraries

  Wire:
      https://www.arduino.cc/reference/en/language/functions/communication/wire/

   OLED:
      http://adafruit.github.io/Adafruit-GFX-Library/html/index.html

   INA219 board
      https://github.com/adafruit/Adafruit_INA219
      http://adafruit.github.io/Adafruit_INA219/html/class_adafruit___i_n_a219.html

## Some coding memos for implicit features

### Print to serial interface
  It is good practice to write debug information to the serial interface.
  You can use the IDE's serial monitor to see the content.

  Example in setup():
  ```
  Serial.begin(9600);
  Serial.println("Hello World.");
  ```
