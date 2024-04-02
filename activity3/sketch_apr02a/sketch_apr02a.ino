#include <EEPROM.h>

const int pinTest = 11;

void setup() {
  pinMode(pinTest, OUTPUT);

  // Cleaning EEPROM
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  digitalWrite(pinTest, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

}
