/*
 * Example on how to use the Wiegand reader library with interruptions.
 */

#include <Wiegand.h>
#include <Arduino.h>
// These are the pins connected to the Wiegand D0 and D1 signals.
// Ensure your board supports external Interruptions on these pins
#define PIN_D0 5
#define PIN_D1 4
#define PIN_LED 2

uint8_t fstatus = 0;
uint8_t fdata = 0;
int dataR = 0;
// The object that handles the wiegand protocol
Wiegand wiegand;
// When any of the pins have changed, update the state of the wiegand library
void pinStateChanged()
{
    wiegand.setPin0State(digitalRead(PIN_D0));
    wiegand.setPin1State(digitalRead(PIN_D1));
}

// Notifies when a reader has been connected or disconnected.
// Instead of a message, the seconds parameter can be anything you want -- Whatever you specify on `wiegand.onStateChange()`
void stateChanged(bool plugged, const char *message)
{
    Serial.print(message);
    Serial.print(plugged ? "CONNECTED" : "DISCONNECTED");
    fstatus = 1;
    if (plugged)
    {
        dataR = 1;
    }
    else
        dataR = 0;
}
// Notifies when a card was read.
// Instead of a message, the seconds parameter can be anything you want -- Whatever you specify on `wiegand.onReceive()`
void receivedData(uint8_t *data, uint8_t bits, const char *message)
{
    Serial1.print(message);
    Serial1.print(bits);
    Serial1.print("bits / ");
    // Print value in HEX
    // uint8_t bytes = (bits+7)/8;
    digitalWrite(LED_BUILTIN, LOW);
    uint8_t bytes = (bits + 7) / 8;
    Serial1.println(bytes);
    dataR = 0;

    for (int i = 0; i < bytes; i++)
    {

        dataR = dataR | (data[i] << (bytes - 1 - i) * 8);
    }
    fdata = 1;
    Serial1.print(dataR);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
}

// Notifies when an invalid transmission is detected
void receivedDataError(Wiegand::DataError error, uint8_t *rawData, uint8_t rawBits, const char *message)
{
    Serial.print("Error");

    Serial.print(Wiegand::DataErrorStr(error));
    Serial.print(" - Raw data: ");
    Serial.print(rawBits);
    Serial.print("bits / ");
    // Print value in HEX
    uint8_t bytes = (rawBits + 7) / 8;
    for (int i = 0; i < bytes; i++)
    {
        Serial.print(rawData[i] >> 4, 16);
        Serial.print(rawData[i] & 0xF, 16);
    }
    Serial.println();
}

// Initialize Wiegand reader
void setup()
{
    Serial1.begin(9600);
    Serial.begin(9600);

    // Install listeners and initialize Wiegand reader
    wiegand.onReceive(receivedData, "");
    wiegand.onReceiveError(receivedDataError, "ERROR");
    wiegand.onStateChange(stateChanged, "State changed: ");
    wiegand.begin(Wiegand::LENGTH_ANY, true);

    // initialize pins as INPUT and attaches interruptions
    pinMode(PIN_D0, INPUT);
    pinMode(PIN_D1, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    attachInterrupt(digitalPinToInterrupt(PIN_D0), pinStateChanged, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_D1), pinStateChanged, CHANGE);

    // Sends the initial pin state to the Wiegand library
    pinStateChanged();
}

// Every few milliseconds, check for pending messages on the wiegand reader
// This executes with interruptions disabled, since the Wiegand library is not thread-safe
void loop()
{
    noInterrupts();
    wiegand.flush();
    interrupts();
    // Sleep a little -- this doesn't have to run very often.
    delay(100);
    if (fdata)
    {
        fdata = 0;

        digitalWrite(LED_BUILTIN, LOW);
        Serial1.println(dataR);
        delay(100);

        digitalWrite(LED_BUILTIN, HIGH);
    }
    if (fstatus)
    {
        fstatus = 0;

        if (dataR)
        {
            Serial1.println("Conectado");
        }
        else
            Serial1.println("Desconectado");
        delay(100);
    }
}
