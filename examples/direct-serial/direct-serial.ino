#include <senseBoxIO.h>

/**
 * @brief Script to test direct serial communication between the rain gauge
 * sensor RG15 and the senseBox.
 * @author Björn Luig (@BjoernLuig)
 * @note See official documentation of the RG15:
 * https://rainsensors.com/wp-content/uploads/sites/3/2020/07/rg-15_instructions_sw_1.000.pdf#page=2
 */

String data;  // raw data
long sendTime, recieveTime;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial1.begin(9600);
  while (!Serial1);
}

void loop() {
  while (Serial.available()) {
    sendTime = millis();
    data = Serial.readString();
    Serial1.println(data);
    Serial.print(sendTime);
    Serial.print(" send: ");
    Serial.print(data);
  }
  while (Serial1.available()) {
    recieveTime = millis();
    data = Serial1.readString();
    Serial.print(recieveTime);
    Serial.print(" recieved: ");
    Serial.print(data);
  }
  delay(10);
}
