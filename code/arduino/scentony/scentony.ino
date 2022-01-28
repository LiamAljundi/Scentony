#include <ArduinoJson.h>

int count = 5;
int baudRate = 9600;
int scentDelay = 150;
int fanTimer = 0;
int fanPin = 8;
int fanDelay = 10;
int btns[] = {2, 3, 4, 5, 6};
int knobs[] = {A0, A1, A2, A3, A4};
int piezos[] = {9, 10, 11, 12, 13};

int concentrations[5][4] = {
    {0, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 1},
    {1, 1, 0, 1},
    {1, 1, 1, 1}};

int concentrationValues[5] = {0, 128, 256, 768, 1023};

int scentArray[4] = {1, 3, 4, 3};
int programs[5][4] = {
    {3, 1, 0, 0},
    {2, 1, 1, 0},
    {1, 1, 1, 0},
    {1, 1, 1, 1},
    {0, 1, 1, 2}};
int smell = 0;
int scentIndex = 0;
bool dispensing = false;

const int docSize = JSON_ARRAY_SIZE(4) + JSON_OBJECT_SIZE(2) + 20;
DynamicJsonDocument doc(docSize);

const char *json = "{\"timestamp\":123456789012345900000,\"scent\":[10,10,10,10]}";

void setup()
{
  Serial.begin(baudRate);
  for (int i = 0; i < count; i++)
  {
    pinMode(btns[i], INPUT_PULLUP);
    pinMode(piezos[i], OUTPUT);
    pinMode(knobs[i], INPUT);
    digitalWrite(piezos[i], LOW);
  }
  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin, LOW);
}

void activatePiezo(int piezo)
{
  digitalWrite(piezos[piezo], HIGH);
  fanTimer = fanDelay;
}

void turnOffPiezos()
{
  for (int i = 0; i < count; i++)
  {
    digitalWrite(piezos[i], LOW);
  }
}

void endScent()
{
  dispensing = false;
  turnOffPiezos();
}

bool readInput()
{
  if (!Serial.available())
    return false;

  DeserializationError error = deserializeJson(doc, Serial);

  // Test if parsing succeeds.
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return false;
  }

  for (int i = 0; i < count; i++)
  {
    scentArray[i] = doc["scent"][i];
  }

  return true;
}
int getKnobConcentration(int value)
{
}

void readKnobs()
{
  for (int i = 0; i < 4; i++)
  {
    int knobVal = analogRead(knobs[i]);
    Serial.print(knobVal);
  }
  Serial.println(" - done");
  delay(1000);
}

void loop()
{
  // readKnobs();
  if (!dispensing) // Not dispensing
  {
    for (int i = 0; i < count; i++)
    {
      if (digitalRead(btns[i]) == LOW)
      {
        Serial.println(i);
        smell = i;
        dispensing = true;
        scentIndex = 0;
      };
    }
  }

  if (dispensing) // Dispensing
  {
    for (int i = 0; i < 4; i++)
    {
      int conc = programs[smell][i];
      if (concentrations[conc][scentIndex] > 0)
      {
        activatePiezo(i);
        // dispense for 250ms and then turn all off
      }
      delay(scentDelay);
      turnOffPiezos();
    }

    scentIndex++;
    if (scentIndex >= 4)
      endScent();
  }

  if (fanTimer > 0)
  {
    digitalWrite(fanPin, HIGH);
    fanTimer -= 1;
  }
  if (fanTimer == 0 && digitalRead(fanPin) == HIGH)
  {
    digitalWrite(fanPin, LOW);
  }

  // delay(10);
}