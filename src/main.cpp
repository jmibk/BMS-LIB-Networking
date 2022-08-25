#include <Arduino.h>
#include "networking.h"

Networking network = Networking();

void setup() {
  Serial.begin(115200);
  
  network.init();

  Serial.println("init completed!");
  }

void loop() {
 
  }