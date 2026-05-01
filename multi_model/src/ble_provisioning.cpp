#include "ble_provisioning.h"
#include <ArduinoBLE.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Preferences.h>

Preferences preferences;

// Services and Characteristics
BLEService scanService("12345678-1234-5678-1234-567812345678");
BLEStringCharacteristic wifiListChar("12345678-1234-5678-1234-567812345678", BLERead, 512);

BLEService provService("87654321-4321-8765-4321-876543218765");
BLEStringCharacteristic ssidChar("87654321-4321-8765-4321-876543218761", BLEWrite, 64);
BLEStringCharacteristic passChar("87654321-4321-8765-4321-876543218762", BLEWrite, 64);
BLEStringCharacteristic statusChar("87654321-4321-8765-4321-876543218763", BLENotify, 32);

String receivedSSID = "";
String receivedPass = "";
unsigned long bleStartTime = 0;
bool provisioningMode = false;

extern portMUX_TYPE serialMutex;

void handleBLEWrite(BLEDevice central, BLECharacteristic characteristic) {
    if (characteristic.uuid() == ssidChar.uuid()) {
        receivedSSID = ssidChar.value();
        Serial.print("Received SSID: ");
        Serial.println(receivedSSID);
    } else if (characteristic.uuid() == passChar.uuid()) {
        receivedPass = passChar.value();
        Serial.print("Received Password!");
    }
    
    if (receivedSSID.length() > 0 && receivedPass.length() > 0) {
        updateBLEStatus("connecting");
        
        Serial.println("Attempting WiFi connect...");
        WiFi.begin(receivedSSID.c_str(), receivedPass.c_str());
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 10) {
            delay(500);
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            updateBLEStatus("connected");
            
            // Save credentials
            preferences.begin("wifi_creds", false);
            preferences.putString("ssid", receivedSSID);
            preferences.putString("pass", receivedPass);
            preferences.end();
            
            Serial.println("WiFi configured via BLE and saved!");
            delay(2000); // Allow time for client to get status
            ESP.restart(); // Restart to normal mode
        } else {
            updateBLEStatus("failed");
            receivedSSID = "";
            receivedPass = "";
        }
    }
}

void updateBLEStatus(String status) {
    statusChar.writeValue(status);
}

void setupBLE() {
    if (!BLE.begin()) {
        Serial.println("starting Bluetooth® Low Energy module failed!");
        return;
    }

    BLE.setLocalName("ESP32-S3-HealthWatch");
    BLE.setAdvertisedService(provService);

    // Add characteristics to services
    scanService.addCharacteristic(wifiListChar);
    
    provService.addCharacteristic(ssidChar);
    provService.addCharacteristic(passChar);
    provService.addCharacteristic(statusChar);

    // Add services
    BLE.addService(scanService);
    BLE.addService(provService);

    // Callbacks
    ssidChar.setEventHandler(BLEWritten, handleBLEWrite);
    passChar.setEventHandler(BLEWritten, handleBLEWrite);

    // Scan for WiFi networks and format as JSON
    Serial.println("Scanning WiFi networks...");
    int n = WiFi.scanNetworks();
    StaticJsonDocument<1024> doc;
    JsonArray array = doc.to<JsonArray>();
    
    for (int i = 0; i < n; ++i) {
        array.add(WiFi.SSID(i));
    }
    String jsonStr;
    serializeJson(doc, jsonStr);
    wifiListChar.writeValue(jsonStr);

    updateBLEStatus("provisioning");
    
    BLE.advertise();
    Serial.println("BLE advertising started. Waiting for connection...");
    
    bleStartTime = millis();
    provisioningMode = true;
}

void checkBLEProvisioning() {
    if (!provisioningMode) return;
    
    BLE.poll();
    
    // Timeout logic - 5 minutes (300000 ms)
    if (millis() - bleStartTime > 300000) {
        Serial.println("BLE Provisioning timeout (5 mins). Restarting...");
        ESP.restart();
    }
}
