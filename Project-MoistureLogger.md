# Project: Moisture_logger
  This document contains information for the Project `Moisture_logger` that
  runs on an ESP32 platform, preferably an ESP32-Lolin32 board powered by a LiPo
  battery and a moisture sensor connected to the ESP board.
  The ESP-32 board periodically sends out moisture levels to a Web Service via
  WLAN. Between the measurements the processor enters deep-sleep mode.
  One design goal was that the logger could run on battery for weeks without
  recharging. Therefore, between measurements the ESP-32 board disconnects
  the moisture sensor its power source.

## Overall context
  This project is part of a system that captures moisture levels of, e.g. plants
  in a database which can be read out via a Web interface.
  The system consists of:
    - Moisture Logger - described here.
    - Web Server (Lamp instance): Raspberry Pi or a docker container running
      on a PC or in the cloud. It hosts the Web Service endpoint and captures
      the moisture levels (and battery level) from the moisture logger in a
      database. To read out the moisture history, the server offers a Web UI
      (https://<hostname>/index.php).
      See Git project:  `schollenberger/lamp-sensor-readings`

## Interface between logger and Web Server

  The Web Service endpoint URL is: https://<hostname>/sensor_values.php
  This service requires an authentication token in the HTTP request header
  `Authorization: Bearer` parameter that identifies a valid user.
  It requriRes the following parameters in a POST request:
    - `sname` - sensor name (string)
    - `slocation` - sensor location (string)
    - `svalue` - moisture level (floating number)
    - `sbattery` - battery level of the sensor (floating number)

  Todo:
  In order to secure the communication between the moisture logger and the Web
  server, use of the HTTPS protocol should be made mandatory. Currently the
  system uses the HTTP protocol as it is running in the local network.


## Hardware setup
  We made several test with different ESP-32/ESP8266 development boards where we
  measured standby current in deep-sleep mode. It turned out that the more
  cheaper modules drew too much power for running on battery for weeks.
  Only the ESP-32 LoLin32 board turned out to be efficient enough.

  We use a 1100mAh Lithium Rechargeable Battery from MakerFocus (3.7V) at which
  we changed the connector to directly fit into the LoLin32 board.

  For the moisture sensor,  we use the `Capacitive Soil Moisture Sensor v 1.2`
  from AZ-Delivery. Supplemental to the AZ-Delivery eBook, you find a good
  technical of this sensor description under:
  https://how2electronics.com/interface-capacitive-soil-moisture-sensor-arduino/

  The datasheet can be found under:
  https://www.sigmaelectronica.net/wp-content/uploads/2018/04/sen0193-humedad-de-suelos.pdf

  According to the spec. this sensor requires a supply voltage of at least 3.3V.
  In order to disconnect the sensor from its power source between measurements,
  the sensor gets connected to the 3.3V output of the processor board via a
  transistor. It is critical that the voltage drop at the transistor is small
  enough so that enough voltage remains for the sensor. The sensor's operation
  current is specified to be 5mA.

  To power the moisture sensor we use the following circuit:
  ```
                                            +--------> VCC Sensor
      From CPU Board                        |C
                                      +-----+-+     
                        +------+      |    /  | BC 640
            GPIO13 >----+ 170R +------+---|   |
                        +------+      |    \^ |
                                      +-----+-+
                                            |E
              3.3V >------------------------+   

               GND >----------------------------------> GND Sensor              

  ```
  The transistor blocks when GPIO13 is high or tri-state.
  When GPIO13 goes low, the transistor opens and enters saturation, thus
  powering the sensor module.
  The resistor has been dimensioned to limit the Basis-Emitter Current to -15mA.

  We measured the transistor saturation voltage to be less than 10mV.
  Normal voltage at Base is about 2.83V and drops down to 2.63V when the
  transistor opens. So really there is about 15mA that the GPIO port has to
  sink. !! (The ESP-32 seems to be sepcified to sink -28mA max.) However, we
  measured the port low voltage to be 12mV so it doesn't look like the port
  is overloaded.

  ## Todo ##
  Think about increasing the resistor 2k to reduce IBE to about 1mA.

  In order to receive log messages when running on battery, you have to connect
  a TTL-to-USB converter to retrieve the log messages. We use the FT232-AZ USB
  Mini serial converter (AZ-Delivery) for this. Note that this device receives
  its power from the USB port if it is connected. No need to connect it to
  the CPU power. Only GND and TX (GPIO17) must be connected.

  Remaining connection diagram:
  ```

                3.3V           TX
                 |       (GPIO17)      GPIO13
                 |           |           |
            +----+-----------+-----------+--+
            |    o o o o o o o o o o o o o  |
            |   13 @--+      7           1  |
            | W       +--BuiltIn LED        |
            | L  ESP-32                 X --+=== USB    
            | A  LoLin32                O   + <- Reset Switch
            | N   Board            ++===x --+=== Battery
            |                +--R2-+|       |
            |   14           |      +-R1-+  |
            |    o o o o o o o o o o o o o  |
            +----+-----------+-----------+--+
                 |       (GPIO33)        |26
               (VP)      (ADC1-5)        |
              ADC1-0                    GND



             Mini USB Port
            +-------------+
            |     XXXX    |
            |             |
            |   TTL-USB   |
            |  Converter  |
            |             |
            |   o  o--o   |  <- 3.3V
            |             |
            | o o o o o o |
            +---+-------+-+
                |       |
               TX      GND
  ```

## Software

  From a software perspective, most of the time the ESP-32 board is running in
  deep sleep mode. When the processor wakes up, it lights up the internal LED
  and powers up the sensor. It connects to the WLAN using hard coded credentials.
  Then it reads the moisture level and the battery level via two AD converter
  ports (ADC-1 port 0 and 5). Then it prepares the HTTP POST request and sends
  it to the Web Service endpoint (hard coded).
  Finally it sets the sleep timer and enters deep-sleep mode.
  Log messages are sent on Serial2 for debugging at 115200 baud.

  The Code has been written within the Arduino IDE for ESP32 and can be found
  in Git under:
    `schollenberger/arduino-projects/ESP-32/Moisture_logger`

  The code doesn't contain any secrets or credentials. These are included with
  the file `WSB_private.h`. A template for this file and a readme on how to make
  this file available in the Arduino IDE can be found in Git under:
    `schollenberger/arduino-projects/ESP-32/WSB_private`

  Web Service endpoint and bearer token that identifies the user name under
  which data is logged are hard coded.
  We use the bearer token of the user: `Demo User`.
  Fixed Web Service parameters are:
    sname = `Moisture-ESP`
    slocation = `Work Room`


## Operation

Sensor readings:

   Air:    1.85 - 1.94 (V)
   Water:  

   Water the plant at a sensor reading above 1.2 V.
   Freshly watered the reading normally goes down to 0.6 - 0.8 V.

Battery
   4.25 to 4.15 V - freshly charged
   3.92 V         - still good.

   3.67 to 3.70 V - reccommended charge voltage
    _
