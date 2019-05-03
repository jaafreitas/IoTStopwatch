#include "debug.h"
#include "version.h"
#include "ntp.h"
#include "server.h"
#include "control.h"

void setup(void) {
  setupDebug();
  debugMsg("\n***** %s *****\n", title());

  // order matters...
  setupSPIFFS();
  setupWifi();
  setupOTA();
  setupNTPClient();
  setupMDNS();
  setupServer();
  setupControl();
}

void loop(void) {
  loopNTPClient();
  loopServer();
  loopControl();
}
