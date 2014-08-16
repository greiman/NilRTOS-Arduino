// Demo of native TwiMaster API.
#include <TwiMaster.h>

// Two Wire Interface instance.
TwiMaster twi;

// DS1307 I2C address in high bits, R/W in low bit.
const uint8_t DS1307ADDR = 0XD0;
// DS1307 address and read bit.
const uint8_t RTC_READ = DS1307ADDR | I2C_READ;
// DS1307 address and write bit
const uint8_t RTC_WRITE = DS1307ADDR | I2C_WRITE;
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);

  // Use standard 100 kHz speed and no internal pullups.
  twi.begin(I2C_100KHZ);
}
//------------------------------------------------------------------------------
void loop() {
  uint8_t add = 0;   // DS1307 time/date register address.
  uint8_t buf[7];    // Buffer for DS1307 register values.

  // Write DS1307 time/date rgister address.
  // Demo option argument, repeat start is not required here.
  if (!twi.transfer(RTC_WRITE, &add, 1, I2C_REP_START)) {
    Serial.print(F("Write failed: 0X"));
    Serial.println(twi.failureState(), HEX);
  }

  // Read DS1307 time/date registers.
  if (!twi.transfer(RTC_READ, buf, 7)) {
    Serial.print(F("Read failed: 0X"));
    Serial.println(twi.failureState(), HEX);
  }

  // Print YYYY-MM-DD hh:mm:ss
  Serial.print("20");
  for (int i = 6; i >= 0; i--){
    // Skip day of week.
    if (i == 3) continue;

    // Always print field with two digits.
    if (buf[i] < 10) Serial.print('0');

    // Ds1307 is BCD.
    Serial.print(buf[i], HEX);

    // Print field separator.
    if (i == 6 || i == 5) Serial.print('-');
    if (i == 4) Serial.print(' ');
    if (i == 2 || i == 1) Serial.print(':');
  }
  Serial.println();
  delay(1000);
}
