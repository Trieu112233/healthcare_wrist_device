#include "network_app.h"
#include "../include/config.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <Preferences.h>
#include "display_app.h"

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);
extern portMUX_TYPE serialMutex;

static unsigned long lastMqttReconnectAttempt = 0;

bool setupNetwork() {
    Preferences preferences;
    preferences.begin("wifi_creds", true);
    String ssid = preferences.getString("ssid", "");
    String pass = preferences.getString("pass", "");
    preferences.end();
    
    if (ssid == "") {
        portENTER_CRITICAL(&serialMutex);
        Serial.println("No WiFi credentials found. Entering BLE Provisioning mode.");
        portEXIT_CRITICAL(&serialMutex);
        updateStatusUI("WiFi: no creds");
        return false;
    }

    portENTER_CRITICAL(&serialMutex);
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
    portEXIT_CRITICAL(&serialMutex);
    updateStatusUI("WiFi: connecting");

    WiFi.mode(WIFI_STA);
    WiFi.setTxPower(WIFI_POWER_8_5dBm); 
    WiFi.begin(ssid.c_str(), pass.c_str());
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        portENTER_CRITICAL(&serialMutex);
         Serial.println("\nWiFi connection failed! Entering BLE Provisioning mode.");
         portEXIT_CRITICAL(&serialMutex);
         WiFi.disconnect(true);
         WiFi.mode(WIFI_OFF);
         updateStatusUI("WiFi: failed");
         delay(100);
         return false;
    }
    
    portENTER_CRITICAL(&serialMutex);
    Serial.println("\nWiFi connected!");
    portEXIT_CRITICAL(&serialMutex);
    updateStatusUI("WiFi: connected");

    const char* ntpServer = "pool.ntp.org";
    const long  gmtOffset_sec = 7 * 3600;
    const int   daylightOffset_sec = 0;

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    updateStatusUI("NTP: syncing");

    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 10000)) {
        Serial.println("Time synchronized successfully!");
        updateStatusUI("NTP: synced");
    } else {
        Serial.println("Failed to obtain time");
        updateStatusUI("NTP: failed");
    }

    espClient.setInsecure(); 
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    updateStatusUI("MQTT: configured");
    return true;
}

void reconnectMQTT() {
    portENTER_CRITICAL(&serialMutex);
    Serial.print("Attempting HiveMQ connection...");
    portEXIT_CRITICAL(&serialMutex);
    
    String clientId = "esp32s3_01_Client";
    clientId += String(random(0xffff), HEX);
    
    if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
        portENTER_CRITICAL(&serialMutex);
        Serial.println("connected to HiveMQ!");
        portEXIT_CRITICAL(&serialMutex);
    } else {
        portENTER_CRITICAL(&serialMutex);
        Serial.print("failed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" (Non-blocking retry)");
        portEXIT_CRITICAL(&serialMutex);
    }
}

void processMQTT() {
    if (WiFi.status() == WL_CONNECTED) {
        if (!mqttClient.connected()) {
            unsigned long now = millis();
            // Retry without blocking the UI and inference loop.
            if (now - lastMqttReconnectAttempt > 5000) {
                lastMqttReconnectAttempt = now;
                reconnectMQTT();
            }
        } else {
            mqttClient.loop();
        }
    }
}

void publishAlert(float fallConf, float screamConf) {
    if (mqttClient.connected()) {
        StaticJsonDocument<200> doc;
        
        doc["deviceId"] = "xiao_esp32s3_01";
        doc["fall_confidence"] = fallConf;
        doc["scream_confidence"] = screamConf;

        char jsonBuffer[256];
        serializeJson(doc, jsonBuffer);
        
        uint32_t p_start = millis();
        bool pubResult = mqttClient.publish(MQTT_TOPIC_PUBLISH, jsonBuffer);
        uint32_t p_end = millis();

        if(pubResult) {
            portENTER_CRITICAL(&serialMutex);
            Serial.printf("\n[MQTT] Pushed alert successfully. MQTT Publish Latency: %lums\n", (p_end - p_start));
            portEXIT_CRITICAL(&serialMutex);
        } else {
            portENTER_CRITICAL(&serialMutex);
            Serial.println("\n[MQTT] Push failed!");
            portEXIT_CRITICAL(&serialMutex);
        }
    }
}
