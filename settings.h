#pragma once

#include <Arduino.h>

#define DEBUG

#define HOSTNAME "IoTStopwatch"

#define WIFI_SSID "********"
#define WIFI_PASSWORD "********"

#define OTA_UPDATEPATH "/firmware"

#define NTP_SERVER "a.st1.ntp.br"
#define NTP_PORT 123
#define NTP_LOCALPORT 2390
#define NTP_INTERVAL 1 * 60 * 60 * 1000 // Every 1 hour (in milliseconds)
#define TZ_OFFSET (-3 * 60 * 60) // in seconds.

#define CSV_HEADER "ID;Time(ms)\n"

#define TOTAL_STOPWATCHES 4

#define BUTTON_DEBOUNCE_DELAY 50

#define BUTTON_1_ID "1"
#define BUTTON_2_ID "2"
#define BUTTON_3_ID "3"
#define BUTTON_4_ID "4"

// digitalRead did NOT work with GPIOs 1, 3, 6, 7, 8, 11, and the ADC (A0)
#define BUTTON_1_PIN D1 // GPIO05
#define BUTTON_2_PIN D2 // GPIO04
#define BUTTON_3_PIN D5 // GPIO14
#define BUTTON_4_PIN D6 // GPIO12

// digitalWrite did NOT work with GPIOs 6, 7, 8, 11, and ADC (A0)
#define BUTTON_1_LED_PIN D3 // GPIO00
#define BUTTON_2_LED_PIN D4 // GPIO02
#define BUTTON_3_LED_PIN D7 // GPIO13
#define BUTTON_4_LED_PIN D8 // GPIO15
