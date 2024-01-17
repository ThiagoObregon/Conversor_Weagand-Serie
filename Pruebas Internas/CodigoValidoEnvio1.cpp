#include <Arduino.h>
#include <Wiegand.h>

#define PIN_D0 5
#define PIN_D1 4

void receivedData(uint8_t* data, uint8_t bits, const char* message);

Wiegand wiegand;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  wiegand.onReceive(receivedData, "Wiegand data: ");
  wiegand.begin(Wiegand::LENGTH_ANY, true);

  pinMode(PIN_D0, INPUT);
  pinMode(PIN_D1, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_D0), []{wiegand.setPin0State(digitalRead(PIN_D0));}, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_D1), []{wiegand.setPin1State(digitalRead(PIN_D1));}, CHANGE);
}

void loop() {
  noInterrupts();
  wiegand.flush();
  interrupts();
  delay(100);
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

    // Enviar los datos por el puerto serie adicional
    Serial1.write(data, bytes);
}


