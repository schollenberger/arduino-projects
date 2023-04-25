# ESP32 Memos

  This document captures stuff around the ESP-32 Dev. Boards. These include:
    - ESP-32 Dev Kit C V2 / ESP-32 NodeMCU board
	- ESP-32 Lolin32
	

  It summarizes Hardware aspects as how to power the Lolin board or how to
  log over the serial line if you are running on battery power and do not
  have an USB port. 
  It summarizes as well software aspects as how to properly use the AD 
  converters, DA converter, deep-sleep mode etc.
  
  Some of the chapters are applicable to ESP-8266 boards. There is a separate
  memo file that covers 8266 board specifics.
  
### Logging when running on battery

    When runnin on battery you cannot use the USB port for logging over the
	serial line. Instead you have to connect a TTL-USB serial adapter to the
	internal serial port (GND, TX). As you are only capturing log data with
	your PC, you don't have to connect the serial RX line.
	
	For the Moisture Logger project with the ESP-32 Lolin32 board, 
	I've been using the FT232-AZ adapter that a Mini USB port. This adapter
    has the same serial chip on it as the ESP board. Therefore you don't 
    have to install an additional COM driver on your PC.	
	When using this port you have to select the logic pin levels via a jumper.
	Set it to 3.3 V. Note, that the adapter receives its power from the USB
	port and therefore doesn't need any power from the ESP board. Connect
	GND and TX (GPIO 17) only.
	
	With this HW setup you have to log to the Serial2 interface. In order
	to switch easily between the two serial lines, you might find the 
	following code snipplet at the beginning of the setup() function useful:
	
	```
	void setup(){
      // Serial port for debugging purposes
      Serial.begin(115200);
      while(!Serial){delay(500);}
     delay(500);
     Serial.println("** Moisture_logger **");

    // If your ESP32 runs on battery and you have to use Serial2 to log output
    // Connect an Serial2USB converter and uncomment the following section.
  
    Serial.println("- Log output see on Serial port 2");
    Serial = Serial2;
    Serial.begin(115200);
    while(!Serial){delay(500);}
    delay(500); 
    Serial.println("** Moisture_logger on Serial2 **");

	...
	```
	It overloads the Serial object with the Serial2 object. Thus you don't have
	to change anything in the remaining code.
	
### ADC read

  ESP32 Analog pins have 12-bit resolution. This means that when you read an
  analog input, its range may vary from 0 to 4095.

  Maximum Voltage that can be read is:  3,24 V
  

  Note that on an ESP32 you cannot use ADC2 in conjuction with WiFi!!
  If you plan to communicate via Wifi stick to the ADC1 channels.

  Note that on a ESP32 the ADC conversion curve is not linear.

  From: https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/

  From https://github.com/espressif/arduino-esp32/issues/92

  Inconsistent values when using "analogRead()"
    Hi,

    I checked yesterday the new implementation of analogRead and values are pretty incosistent. Not sure if it is because of the implementation of the method or just because the ADC on ESP32 is not very accurate.

    I created a simple several stage voltage divider to read on. Source was the 3.3V output pin of the ESP WROOM 32. Then I applied 3 different steps with 4.7K 1% tolerance resistors. Values measured with the multimeter:

    P1: 3.286V --> Expected = 4095 / ADC output = 4095
    P2: 2.462V --> Expected = 3071 / ADC output = 2811
    P3: 1.641V --> Expected = 2047 / ADC output = 1814
    P4: 0.819V --> Expected = 1021 / ADC output = 823

    Any explanation to this? I though the levels would go from 0 being 0V and 4095 being top of the range 3.3V

    I need to rely on an accurante ADC for my project so any help is welcomed. Cheers!



### Analog write

  The ESP32 chip has 2 DAC channels, DAC1 and DAC2 an a number of GPIO ports
  that support PWM (Pulse Width Modulation).
  From https://github.com/espressif/arduino-esp32/issues/92

      I'm waiting for the last PWM driver to be documented and prepped in IDF. There are currently 3 options that you can use for PWM already available.

          LEDC (16 channels) < its basically PWM
          SigmaDelta (8 channels) < it is SigmaDelta which is great for LEDs and others
          DAC (2 channels) < outputs actual analog signal

      //SigmaDelta Setup(pin 16, channel 0)
      sdSetup(0, 10000);//channel, frequency
      sdAttachPin(16, 0);//pin, channel
      //SigmaDelta Write
      sdWrite(0, 128);//channel, 8bit value

      //LEDC Setup(pin 17, channel 5)
      ledcSetup(5, 10000, 16);//channel, frequency, bits
      ledcAttachPin(17, 5);//pin, channel
      //LEDC Write
      ledcWrite(5, 1024);//channel, bits value

      //DAC (pins 25 or 26)
      sdWrite(25, 128);//pin, 8bit value
      sdWrite(26, 65);//pin, 8bit value
