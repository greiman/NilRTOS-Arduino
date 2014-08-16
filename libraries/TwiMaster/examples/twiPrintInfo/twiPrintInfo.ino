// Demo of TwiMaster printInfo functions.
// Requires a DS1307.
#include <TwiMaster.h>

// Two Wire Interface instance.
TwiMaster twi;

// DS1307 I2C address in high bits, R/W in low bit.
const uint8_t DS1307ADDR = 0XD0;
//------------------------------------------------------------------------------
void setup() {

  uint8_t add = 0;   // DS1307 time/date register address.
  uint8_t buf[7];    // Buffer for DS1307 register values.

  Serial.begin(9600);

  // Use standard 100 kHz speed and no internal pullups.
  twi.begin(I2C_100KHZ);

  // Write DS1307 time/date rgister address.
  twi.transfer(DS1307ADDR | I2C_WRITE, &add, 1, I2C_REP_START);

  Serial.println(F("Example of printInfo() after an I2C write:"));
  twi.printInfo(&Serial);

  // Invalid tranferContinue call.
  twi.transferContinue(buf, 7);

  Serial.println();
  Serial.println(F("Example of printInfo() after an invalid transferContinue():"));
  twi.printInfo(&Serial);

  twi.transfer((DS1307ADDR + 2) | I2C_READ, &add, 1);
  Serial.println();
  Serial.println(F("Example of printInfo() after a read with a bad I2C address:"));
  twi.printInfo(&Serial);
}
//------------------------------------------------------------------------------
void loop() {
}
