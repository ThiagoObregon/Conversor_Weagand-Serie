#include <Arduino.h>
#include <Wiegand.h>

#define PIN_D0 5
#define PIN_D1 4

void receivedData(uint8_t* data, uint8_t bits, const char* message);
void receivedDataError(Wiegand::DataError error, uint8_t* rawData, uint8_t rawBits, const char* message);
void stateChanged(bool plugged, const char* message);
void pinStateChanged();

Wiegand wiegand;

void setup() {
  Serial.begin(9600);

  wiegand.onReceive(receivedData, "Card read: ");
  wiegand.onReceiveError(receivedDataError, "Card read error: ");
  wiegand.onStateChange(stateChanged, "State changed: ");
  wiegand.begin(Wiegand::LENGTH_ANY, true);

  pinMode(PIN_D0, INPUT);
  pinMode(PIN_D1, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_D0), pinStateChanged, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_D1), pinStateChanged, CHANGE);

  pinStateChanged();
}

void loop() {
  noInterrupts();
  wiegand.flush();
  interrupts();
  delay(100);
}

void pinStateChanged() {
  wiegand.setPin0State(digitalRead(PIN_D0));
  wiegand.setPin1State(digitalRead(PIN_D1));
}

void stateChanged(bool plugged, const char* message) {
    Serial.print(message);
    Serial.println(plugged ? "CONNECTED" : "DISCONNECTED");
}

void receivedData(uint8_t* data, uint8_t bits, const char* message) {
    Serial.print(message);
    Serial.print(bits);
    Serial.print(" bits / ");
    
    // Print value in HEX
    uint8_t bytes = (bits + 7) / 8;
    for (int i = 0; i < bytes; i++) {
        Serial.print(data[i] >> 4, 16);
        Serial.print(data[i] & 0xF, 16);
    }
    Serial.println();
}

void receivedDataError(Wiegand::DataError error, uint8_t* rawData, uint8_t rawBits, const char* message) {
    Serial.print(message);
    Serial.print(Wiegand::DataErrorStr(error));
    Serial.print(" - Raw data: ");
    Serial.print(rawBits);
    Serial.print(" bits / ");

    // Print value in HEX
    uint8_t bytes = (rawBits + 7) / 8;
    for (int i = 0; i < bytes; i++) {
        Serial.print(rawData[i] >> 4, 16);
        Serial.print(rawData[i] & 0xF, 16);
    }
    Serial.println();
}