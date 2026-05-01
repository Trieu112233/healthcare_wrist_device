#ifndef BLE_PROVISIONING_H
#define BLE_PROVISIONING_H

#include <Arduino.h>

extern bool provisioningMode;

void setupBLE();
void updateBLEStatus(String status);
void checkBLEProvisioning();

#endif

