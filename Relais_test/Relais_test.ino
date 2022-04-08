/*
  Relais Test
  
  derived from Blink example

  Turns an LED and Relais (D02) on for 5 second, then off for 5 second, repeatedly.
  It prints out something to the serial port just for demo purposes.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  creaded 2022-04-03
  by Werner Schollenberger
  
  The base of this example code is in the public domain.
  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/

int RELAIS = 2;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAIS, OUTPUT);
  Serial.begin(9600);
  Serial.println("Hallo Du da");
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(RELAIS, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(5000);                    // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(RELAIS, LOW);    // turn the LED off by making the voltage LOW
  delay(5000);                    // wait for a second
}
