#pragma once

#include <Arduino.h>

void setupSPIFFS();
void setupWifi();
void setupMDNS();
void setupOTA();
void setupServer();

void loopServer();
