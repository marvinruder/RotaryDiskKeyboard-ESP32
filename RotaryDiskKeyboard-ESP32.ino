#include "USB.h"
#include "USBHIDKeyboard.h"

// NSI switch contact, generating the impulses
static constexpr uint8_t NSI_PIN = D12;
// NSA switch contact, indicating dial displacement
static constexpr uint8_t NSA_PIN = D13;

USBHIDKeyboard keyboard;

// Timer for sampling the NSI state
hw_timer_t* nsiSamplingTimer = NULL;

// Sample Rate, in ms
static const byte sampleRate = 5;
// Minimum duration of a state change to be counted, in ms
static const byte minImpulseDuration = 10 / sampleRate;

// Debounce counter for rising edge of NSI
volatile unsigned int nsiHighCount = 0;
// Debounce counter for falling edge of NSI
volatile unsigned int nsiLowCount = 0;

// Debounce counter for rising edge of NSA
unsigned short nsaHighCount = 0;
// Debounce counter for falling edge of NSA
unsigned short nsaLowCount = 0;

// Counter for the number entered
volatile byte number = 0;
byte previousNumber = 0;

// Whether the NSI sampling timer is active
boolean timerActive = false;

void IRAM_ATTR nsiSamplingISR() {
    // Ensure NSA is closed
    if (!digitalRead(NSA_PIN)) {
        if (!digitalRead(NSI_PIN)) {
            if (nsiLowCount++ > minImpulseDuration) {
                nsiHighCount = 0;
                digitalWrite(LED_RED, HIGH);
            }
        } else {
            if (nsiHighCount++ > minImpulseDuration) {
                if (nsiLowCount > minImpulseDuration) {
                    number++;
                    nsiLowCount = 0;
                    digitalWrite(LED_RED, LOW);
                }
            }
        }
    }
}

void setup() {
    pinMode(NSA_PIN, INPUT_PULLUP);
    pinMode(NSI_PIN, INPUT_PULLUP);
    pinMode(LED_RED, OUTPUT);

    keyboard.begin();

    nsiSamplingTimer = timerBegin(0, 80, true);
    timerAttachInterrupt(nsiSamplingTimer, &nsiSamplingISR, true);
    timerAlarmWrite(nsiSamplingTimer, 1000 * sampleRate, true);
}

void loop() {
    if (!(digitalRead(NSA_PIN))) {
        nsaHighCount = 0;

        if (number > previousNumber) {
            previousNumber = number;
            uint8_t numberChar = '0' + (previousNumber % 10);
            if (previousNumber > 1) {
                uint8_t* charBuffer = new uint8_t[2];
                charBuffer[0] = KEY_BACKSPACE;
                charBuffer[1] = numberChar;
                keyboard.write(charBuffer, 2);
            } else {
                keyboard.write(numberChar);
            }
        }

        if (nsaLowCount > 50) {
            if (!timerActive) {
                timerAlarmEnable(nsiSamplingTimer);
                timerActive = true;
            }
        } else nsaLowCount++;
        
    } else {
        nsaLowCount = 0;

        if (nsaHighCount > 50) {
            if (timerActive) {
                timerAlarmDisable(nsiSamplingTimer);
                timerActive = false;
                number = 0;
                previousNumber = 0;
            }
        } else nsaHighCount++;
    }
}
