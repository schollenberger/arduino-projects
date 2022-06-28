/*
  BatteryTest

  This program tests the capacity status of a battery,
  rechargable or not by measuring the voltage with and without
  some load.
  To be used with an INA219 Ampere/Voltage meter and a Relay
  to connect the discharge resistor and an OLED display (128x64 pixel)
  to show actual voltage, current and discharge values.
 
  Starting and stopping the discharge test is done via an
  electrical relay connected to D2 of the Aduino Nano.
 
  It uses the Adafruit Circuit LCD libraries to control the
  ina219 device, relay, and the OLED/LCD display.

  It uses the Analog pin A0 to measure battery voltage as the values 
  form the INA219 board are not accurate.

  It uses the serial port (set to 9600 baud) to log messages.

  Requires an INA219 breakout board, Relay and an OLED I2C display.
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

int RELAY = 2;
float VCC = 4.6;

struct INA_RESULT {
   float shuntvoltage;
   float busvoltage;
   float current_mA;
   float loadvoltage;
   float power_mW;

};

void setup() {
  // put your setup code here, to run once:

  // initialize pin LED_BUILTIN and Relay as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY, OUTPUT);
  // initialize serial line
  Serial.begin(9600);
  Serial.println("BatteryTest Debug Log");
  
  // initialize OLED display
  delay(250); // wait for the OLED to power up
  display.begin(oled_i2c_Address, true); // Address 0x3C default
  // Wire.setClock(400000); // increase I2C clock speed to 400 kHz
  Serial.println("OLED display initialized.");
  //display.setContrast (0); // dim display
 
  display.display(); //shows initial Adafruit splash screen
  delay(2000);

  // Clear the buffer.
  // display.clearDisplay();
  
  // Intro screen
  printIntro();
  delay(2000);
  Serial.println("Intro on OLED printed.");
  
  // invert the display
  //display.invertDisplay(true);
  //delay(1000);
  //display.invertDisplay(false);
  //delay(1000);
  
  // Configure the Analog reference
  //analogReference(EXTERNAL);  // use AREF for reference voltage (must be at least 1.1V)
  //analogReference(DEFAULT);   // reset to internal 5V reference
 
   const char* msg[] = {"Test error message", "Error line 2", 0};
   printError(msg);
   delay(2000);
 
  // Configure the INA219 sensor
  if (! ina219.begin()) {
     const char* msg[] = {"Failed to find", "INA219 chip", 0 };
     printError(msg);
     while (1) { delay(10); }
  }
  
  const char *cal_msg; // text to store ina calibration
  
  // To use the highest 32V, 1A range (current unit: 0,1mA, power unit 2mW, overflow at 3.2A):
  //ina219.setCalibration_32V_2A();
  cal_msg = "32V / 2 A";
  // To use a slightly lower 32V, 1A range (higher precision on amps 0.04mA, 0.8mW, overflow at 1.3A):
  //ina219.setCalibration_32V_1A();
  //cal_msg = "32V / 1 A";
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();
  //cal_msg = "16V / 400 mA";

/*
  // show config on display
  //char buf[10];
  //snprintf(buf, sizeof(buf), "VREF = %d", VCC);  // Analog input reference
  const char* imsg[4];
  imsg[0] = "INA219 Calibration:",
//  imsg[1] = cal_msg;
  imsg[1] = 0;
//  imsg[2] = buf;
  imsg[2] = "VREF =  5.0V";
  imsg[3] = 0;
  printError(imsg);
  delay(5000);
*/

  // !!! const char* msg2[] = {"2nd test message", "2nd line", 0};
  printError(msg2);
   delay(2000);


  Serial.println("End of setup()");
}

void loop() {
  // put your main code here, to run repeatedly:
/*
  INA_RESULT res = { 
     0, // shuntvoltage
     0, // busvoltage
     0, // current_mA
     0, // loadvoltage
     0  // power_mW
  };

  res = read_ina();
   
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - VCC):
  float voltage = sensorValue * (VCC / 1023.0);
  
  printScreen(res.shuntvoltage, res.busvoltage, res.current_mA, res.power_mW, res.loadvoltage, voltage);
  delay(500);
*/
/*
    try:
        Uactual,Iactual,Pactual = ina219_read(ina)
        Uinit = Uactual
        print_start(lcd, Uinit)
        sleep(wait_init)
        log_message("Start discharging at {:0.3f} V...".format(Uinit))
        turn_on(relay_io)
        time_discharge = 0

        while time_discharge <= DISCHARGE_INTERVAL:
            sleep(1)
            Uactual,Iactual,Pactual = ina219_read(ina)
            print_status(lcd, time_discharge, Uinit, Uactual, Iactual)
            time_discharge  += 1

        log_message('Stopping discharge...')
        turn_off(relay_io)
        Uload = Uactual
        if not args.quicktest:
            print_display(lcd, "Stopped at {0:0.3} V\n\nLet battery recover...".format(Uload))
            sleep(1)
            time_discharge  += 1
            Uactual,dummy,dummy = ina219_read(ina)
            log_message('Voltage after discharge end: {0:0.3f} V'.format(Uactual))

            log_message('Waiting another few seconds to let the battery relax')
            sleep(1)
            time_discharge  += 1
            Uactual,dummy,dummy = ina219_read(ina)
            log_message('Voltage discharged battery:  {0:0.3f} V'.format(Uactual))

        print_final(lcd, time_discharge, Uinit, Uload, Iactual, Uactual)
#        logger.info("Waiting 5 sec so before closing, so you can read the display")
#        sleep(5)

 */
}

// local subroutines

void logMessage(const char* txt) {
  // This routine logs messages to the serial port
 //Serial.println(txt);
}

const char* duration_to_hhmmss(int duration){
/*  

    durtxt = 'undefined'
    if duration < 60:
         durtxt = '  {0:2d} sec'.format(duration)
    else:
         mm,ss = divmod(duration,60)
         if duration < 3600:
              durtxt = '   {0:02d}:{1:02d}'.format(mm,ss)
         else:
              hh,mm = divmod(mm,60)
              durtxt ='{0:02d}:{1:02d}:{2:02d}'.format(hh,mm,ss)

    return durtxt   
*/
   return "undefined";
}

void printIntro() {
  //Serial.println("printIntro() - start");
  display.clearDisplay();
  display.drawRect(0, 0, display.width() - 1, display.height() - 1, SH110X_WHITE);
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(5, 5);
  display.print("Battery");
  display.display();
  //Serial.println("printIntro() - 1st line");
  delay(2000);
  display.setCursor(5, 25);
  display.print("Test");
  display.display();
  //delay(2000);
  //display.setCursor(5, 45);
  //display.print("Simple");
  //display.display();
  //Serial.println("printIntro() - end");
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

void printError(const char *lines[]) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  // text display 
  display.setCursor(5, 3);
  display.print("Battery Test");
  int ypos = 20;
  int i = 0;
 // display.setCursor(5, ypos + i*10);
  while (lines[i] != 0) {
     display.setCursor(5, ypos + i*10);
     display.print(lines[i]);    
     Serial.print("printError(): "); Serial.println(lines[i]);
     i++;
  }
  display.display();
}

void turn_on() {
  logMessage("Turn relay on");
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(RELAY, HIGH);
  delay(500);
}

void turn_off() {
  logMessage("Turn relay off");
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(RELAY, LOW);
  delay(500);
}

INA_RESULT read_ina() {
  /*
  INA_RESULT res = { 
     0, // shuntvoltage
     0, // busvoltage
     0, // current_mA
     0, // loadvoltage
     0  // power_mW
  }
  */
  
  INA_RESULT res;
  /*
  res.shuntvoltage = ina219.getShuntVoltage_mV();
  res.busvoltage = ina219.getBusVoltage_V();
  res.current_mA = ina219.getCurrent_mA();
  res.power_mW = ina219.getPower_mW();
  res.loadvoltage = res.busvoltage + (res.shuntvoltage / 1000);
*/
  return res;
}
