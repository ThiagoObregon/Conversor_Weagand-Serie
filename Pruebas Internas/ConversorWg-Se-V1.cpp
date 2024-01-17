#include <Arduino.h>
#include <Wiegand.h>

#define PIN_D0 5
#define PIN_D1 4

#define Led1 10



bool blinkLedWgnd = false;
bool RecivedWgndCrrect = false;
bool RecivedWgndINCrrect = false;

bool WgndConnected = false;

int dataR;

Wiegand wiegand;

void pinStateChanged()
{
    wiegand.setPin0State(digitalRead(PIN_D0));
    wiegand.setPin1State(digitalRead(PIN_D1));
}

void stateChanged(bool plugged, const char *message)
{    
    if (plugged){
        WgndConnected = true;
    }
    else {
        WgndConnected = false;
    }
}

void receivedData(uint8_t *data, uint8_t bits, const char *message)
{
    dataR = 0;
    uint8_t bytes = (bits + 7) / 8;
    for (int i = 0; i < bytes; i++)
    {
        // Serial.print(data[i] >> 4, 16);
        // Serial.print(data[i] & 0xF, 16);
        dataR = dataR | (data[i] << (bytes - 1 - i) * 8);
    }
    RecivedWgndCrrect = true;
    Serial.print(dataR);
}

void receivedDataError(Wiegand::DataError error, uint8_t *rawData, uint8_t rawBits, const char *message)
{
    RecivedWgndINCrrect = true;
    Serial.println("ErrWgIn");

    // Uncomment this line if it is necessary to debug the ErrWgIn failure.
    Serial.print(message);
    Serial.print(Wiegand::DataErrorStr(error));
    Serial.print(" - Raw data: ");
    Serial.print(rawBits);
    Serial.print(" bits");
}

void setup()
{
    pinMode(Led1, OUTPUT);
    digitalWrite(Led1, LOW);

    Serial.begin(9600);
    // Serial.swap();

    wiegand.onReceive(receivedData, "Card readed: ");
    wiegand.onReceiveError(receivedDataError, "Card read error: ");
    wiegand.onStateChange(stateChanged, "State changed: ");
    wiegand.begin(Wiegand::LENGTH_ANY, true);

    pinMode(LED_BUILTIN, OUTPUT);

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
}

void loop()
{
    noInterrupts();
    wiegand.flush();
    interrupts();
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);

    // Si hay datos disponibles en el puerto serie
    if (Serial.available() > 0)
    {
        // Vaciar el búfer del puerto serie
        while (Serial.available() > 0)
        {
            Serial.read();
        }
    }

    if (WgndConnected){
        Serial.println("Conected");
    }

    if (RecivedWgndCrrect)
    {
        digitalWrite(Led1, HIGH);
        delay(100);
        digitalWrite(Led1, LOW);
        RecivedWgndCrrect = false;
    }

    if (RecivedWgndINCrrect)
    {
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
}
