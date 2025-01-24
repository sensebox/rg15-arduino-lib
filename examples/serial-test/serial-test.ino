#include <RG15.h>

#include "senseBoxIO.h"

RG15 rg15(Serial1);

void setup() {
  // USB seriel
  Serial.begin(9600);
  while (!Serial);

  // start sensor
  Serial.println("Starting RG15");
  if (rg15.begin(9600, 500, 2)) {
    Serial.println("RG15 sensor started");
  } else {
    Serial.print("error: ");
    Serial.println(rg15.getErrorCode());
    Serial.println(rg15.getResponseBuffer());
  }
}

void loop() {
  if(!rg15.poll()) {
    Serial.print("error: ");
    Serial.println(rg15.getErrorCode());
  }
  char* text = rg15.getResponseBuffer();
  size_t len = 100;
  Serial.println(rg15.getResponseBuffer());
  Serial.print("accumulation: ");
  Serial.println(rg15.getAccumulation());
  Serial.print("event accumulation: ");
  Serial.println(rg15.getEventAccumulation());
  Serial.print("total accumulation: ");
  Serial.println(rg15.getTotalAccumulation());
  Serial.print("rainfall intensity: ");
  Serial.println(rg15.getRainfallIntensity());

  delay(1000);
}