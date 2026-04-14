#ifndef NETWORK_APP_H
#define NETWORK_APP_H

#include <Arduino.h>

void setupNetwork();
void processMQTT();
void publishAlert(float fallConf, float screamConf);

#endif
