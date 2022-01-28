#include <Arduino.h>

const int fanPin = 9;
boolean on = false;

void setup()
{
  pinMode(fanPin, OUTPUT);
}

void loop()
{
  tone(fanPin, 1000000000);
  delay(1000);
  noTone(fanPin);
  delay(1000);
}