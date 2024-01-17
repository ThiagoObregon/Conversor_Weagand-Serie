#include <Arduino.h>
#include <Wiegand.h>

#define PIN_D0 5
#define PIN_D1 4

#define Led1 10

bool RecivedWgndCrrect = false;
bool RecivedWgndINCrrect = false;
bool SendSCrrect = false;
bool ConectWgnd = false;

char Mensaje;
const char *ErrorWgnd;
uint8_t RawBits;

int dataR;
uint8_t bytes;
uint8_t *Data;

Wiegand wiegand;

void pinStateChanged()
{
  wiegand.setPin0State(digitalRead(PIN_D0));
  wiegand.setPin1State(digitalRead(PIN_D1));
}

void stateChanged(bool plugged, const char *message)
{
  if (plugged)
  {
    Serial.print("Conected");
  }
  else
  {
    Serial.print("Disconected");
  }
}

void receivedData(uint8_t *data, uint8_t bits, const char *message)
{
  dataR = 0;
  bytes = (bits + 7) / 8;
  Data = data;
  RecivedWgndCrrect = true;
}

void receivedDataError(Wiegand::DataError error, uint8_t *rawData, uint8_t rawBits, const char *message)
{
  RecivedWgndINCrrect = true;
  Mensaje = *message;
  ErrorWgnd = Wiegand::DataErrorStr(error);
}

void setup()
{
  pinMode(Led1, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(Led1, LOW);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(9600);
  Serial.swap();

  wiegand.onReceive(receivedData, "Card readed: ");
  wiegand.onReceiveError(receivedDataError, "Card read error: ");
  wiegand.onStateChange(stateChanged, "State changed: ");
  wiegand.begin(Wiegand::LENGTH_ANY, true);

  pinMode(PIN_D0, INPUT);
  pinMode(PIN_D1, INPUT);

  attachInterrupt(
      digitalPinToInterrupt(PIN_D0), []
      { wiegand.setPin0State(digitalRead(PIN_D0)); },
      CHANGE);
  attachInterrupt(
      digitalPinToInterrupt(PIN_D1), []
      { wiegand.setPin1State(digitalRead(PIN_D1)); },
      CHANGE);
  pinStateChanged();
}

void loop()
{
  noInterrupts();
  wiegand.flush();
  interrupts();
  delay(100);

  if (RecivedWgndCrrect)
  {

    for (int i = 0; i < bytes; i++)
    {
      dataR = (dataR << 8) | Data[i];
    }
    digitalWrite(Led1, HIGH);
    delay(100);
    digitalWrite(Led1, LOW);
    RecivedWgndCrrect = false;
    Serial.print(dataR);
  }

  if (RecivedWgndINCrrect)
  {
    // Uncomment this lines if it is necessary to debug the ErrWgIn failure.
    // Serial.write((const uint8_t *)"ErrWgIn", 7);
    // Serial.print(Mensaje);
    // Serial.print(ErrorWgnd);
    // Serial.print(" - Raw data: ");
    // Serial.print(RawBits);
    // Serial.print(" bits");
    digitalWrite(Led1, HIGH);
    delay(100);
    digitalWrite(Led1, LOW);
    delay(100);
    digitalWrite(Led1, HIGH);
    delay(100);
    digitalWrite(Led1, LOW);
    delay(100);
    digitalWrite(Led1, HIGH);
    delay(100);
    digitalWrite(Led1, LOW);
    RecivedWgndINCrrect = false;
  }

  if (Serial.available() > 0)
  {
    while (Serial.available() > 0)
    {
      Serial.read();
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
    }
  }
}
