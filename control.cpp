#include "control.h"
#include "debug.h"
#include "settings.h"

#include <FS.h>
#include "src/Time/TimeLib.h"

const uint8_t pinButton[TOTAL_STOPWATCHES] = {BUTTON_1_PIN, BUTTON_2_PIN, BUTTON_3_PIN, BUTTON_4_PIN};
const uint8_t pinLEDButton[TOTAL_STOPWATCHES] = {BUTTON_1_LED_PIN, BUTTON_2_LED_PIN, BUTTON_3_LED_PIN, BUTTON_4_LED_PIN};
const String buttonID[TOTAL_STOPWATCHES] = {String(BUTTON_1_ID), String(BUTTON_2_ID), String(BUTTON_3_ID), String(BUTTON_4_ID)};

bool stopwatchTriggered[TOTAL_STOPWATCHES] = {false, false, false, false};
unsigned long timeStopwatchStarted[TOTAL_STOPWATCHES] = {0, 0, 0, 0}; // in miliseconds.

unsigned long lastDebounceTime[TOTAL_STOPWATCHES] = {0, 0, 0, 0}; // in miliseconds.
bool previousButtonState[TOTAL_STOPWATCHES] = {LOW, LOW, LOW, LOW}; // the previous reading from the input pin
bool buttonState[TOTAL_STOPWATCHES];

String currentCSVFile() {
  static char path[14];
  time_t t = now();
  sprintf(path, "/%04d%02d%02d.csv", year(t), month(t), day(t));
  return String(path);
}

void setupControl() {
  for (int idx = 0; idx < TOTAL_STOPWATCHES; idx++) {
    pinMode(pinButton[idx], INPUT);
    pinMode(pinLEDButton[idx], OUTPUT);
    // Turn off all LEDs.
    digitalWrite(pinLEDButton[idx], LOW);
  }  
}

void checkStopwatch(uint8_t idx) {
  int reading = digitalRead(pinButton[idx]);
  
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != previousButtonState[idx]) {
    // reset the debouncing timer
    lastDebounceTime[idx] = millis();
  }
  
  if ((millis() - lastDebounceTime[idx]) > BUTTON_DEBOUNCE_DELAY) {    
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState[idx]) {
      buttonState[idx] = reading;

      // only toggle if the new button state is HIGH
      if (buttonState[idx] == HIGH) {
        stopwatchTriggered[idx] = !stopwatchTriggered[idx];
        digitalWrite(pinLEDButton[idx], stopwatchTriggered[idx]);

        // Stopwatch started.
        if (stopwatchTriggered[idx]) {
          debugMsg("stopwatch ID %s triggered\n", buttonID[idx].c_str());
          timeStopwatchStarted[idx] = millis();
        }
        
        // Stopwatch finished.
        if (!stopwatchTriggered[idx]) {
          unsigned long stopwatch = millis() - timeStopwatchStarted[idx];
          debugMsg("stopwatch ID %s / time %lu (ms)\n", buttonID[idx].c_str(), stopwatch);
    
          String path = currentCSVFile();
          File file;
          if (!SPIFFS.exists(path)) {
            file = SPIFFS.open(path, "w");
            file.print(CSV_HEADER);
          } else {      
            file = SPIFFS.open(path, "a");
          }
          file.printf("%s;%lu\n", buttonID[idx].c_str(), stopwatch);
          file.close();
        }        
      }
    }
  }
  // save the reading. Next time through the loop, it'll be the previousButtonState:
  previousButtonState[idx] = reading;
}

void loopControl() {
  for (int idx = 0; idx < TOTAL_STOPWATCHES; idx++) {
    checkStopwatch(idx);
  }
}
