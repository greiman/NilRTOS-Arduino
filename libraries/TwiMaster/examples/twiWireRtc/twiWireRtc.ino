// Demo of Wire style API.
//#include <Wire.h>
#include <TwiMaster.h>

// Wire style address for DS1307
const int DS1307ADDR = (0XD0 >>1);
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  
  // Use standard 100 kHz with internal pull-ups enabled.
  Wire.begin();

//  Next line sets I2C speed to 400 kHz for TwiMaster.  
//  Wire.speed(I2C_400KHZ);

//  Next line disables internal pullups for TwiMaster.
//  Wire.pullups(I2C_NO_PULLUPS);
}
//------------------------------------------------------------------------------
void loop() {
  int rtn;
  uint8_t add = 0;   // DS1307 time/date register address.
  uint8_t buf[7];    // Buffer for DS1307 register values.

  // Write DS1307 time/date rgister address.
  Wire.beginTransmission(DS1307ADDR);
  Wire.write(add);
  
  // End transfer with repeated start.
  if (rtn = Wire.endTransmission(false)) {
    Serial.print("write error: ");
    Serial.println(rtn);
  }
  // Read DS1307 time/date registers.
  if (Wire.requestFrom(DS1307ADDR, 7) != 7) {
    Serial.println("read error");
  }
  Wire.readBytes((char*)buf, 7);
  
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

