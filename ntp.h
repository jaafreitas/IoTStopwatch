#pragma once

#include <Arduino.h>

void setupNTPClient();

void loopNTPClient();

String getISOTime();
