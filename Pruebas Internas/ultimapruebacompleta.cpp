#include <Arduino.h>
#include <Wiegand.h>

#define PIN_D0 5
#define PIN_D1 4

void receivedData(uint8_t *data, uint8_t bits, const char *message);

Wiegand wiegand;
bool blinkLed = false;

void setup()
{
    Serial.begin(9600);
    Serial.swap();
    Serial1.begin(9600);
    wiegand.onReceive(receivedData, "Wiegand data: ");
    wiegand.begin(Wiegand::LENGTH_ANY, true);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

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
    digitalWrite(LED_BUILTIN, LOW);

    // Si hay datos disponibles en el puerto serie
    if (Serial.available() > 0)
    {
        blinkLed = true;
        // Vaciar el búfer del puerto serie
        while (Serial.available() > 0)
        {
            Serial.read();
        }
    }

    // Control del parpadeo del LED incorporado
    if (blinkLed)
    {
        // Parpadear el LED incorporado
        for (int i = 0; i < 4; i++)
        {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(100);
            digitalWrite(LED_BUILTIN, LOW);
            delay(100);
        }

        // Desactivar el parpadeo después de parpadear
        blinkLed = false;
    }
}

void receivedData(uint8_t *data, uint8_t bits, const char *message)
{
    // Enviar los datos por el puerto serie adicional
    uint8_t bytes = (bits + 7) / 8;
    Serial.write(data, bytes);
}
