#include "network_app.h"
#include "../include/config.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);
extern portMUX_TYPE serialMutex;

void setupNetwork() {
    portENTER_CRITICAL(&serialMutex);
    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_SSID);
    portEXIT_CRITICAL(&serialMutex);

    WiFi.mode(WIFI_STA);
    WiFi.setTxPower(WIFI_POWER_8_5dBm); // Giảm công suất WiFi để chống nhiễu Mic INMP441
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    portENTER_CRITICAL(&serialMutex);
    Serial.println("\nWiFi connected!");
    portEXIT_CRITICAL(&serialMutex);

    // KẾT NỐI QUA BẢO MẬT TLS (Bỏ qua cert đối với Demo HiveMQ)
    espClient.setInsecure(); 
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
}

void reconnectMQTT() {
    while (!mqttClient.connected()) {
        portENTER_CRITICAL(&serialMutex);
        Serial.print("Attempting HiveMQ connection...");
        portEXIT_CRITICAL(&serialMutex);
        
        String clientId = "ESP32WatchBo-";
        clientId += String(random(0xffff), HEX);
        
        if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
            portENTER_CRITICAL(&serialMutex);
            Serial.println("connected to HiveMQ!");
            portEXIT_CRITICAL(&serialMutex);
        } else {
            portENTER_CRITICAL(&serialMutex);
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            portEXIT_CRITICAL(&serialMutex);
            delay(5000);
        }
    }
}

void processMQTT() {
    if (WiFi.status() == WL_CONNECTED) {
        if (!mqttClient.connected()) {
            reconnectMQTT();
        }
        mqttClient.loop();
    }
}

void publishAlert(float fallConf, float screamConf) {
    if (mqttClient.connected()) {
        StaticJsonDocument<200> doc;
        
        // Cái chuỗi JSON để gửi Firebase theo đúng luồng
        doc["deviceId"] = "xiao_esp32s3_01"; // Trùng document ID ở NodeRED
        doc["fall_confidence"] = fallConf;
        doc["scream_confidence"] = screamConf;

        char jsonBuffer[256];
        serializeJson(doc, jsonBuffer);
        
        if(mqttClient.publish(MQTT_TOPIC_PUBLISH, jsonBuffer)) {
            portENTER_CRITICAL(&serialMutex);
            Serial.println("\n[MQTT] Pushed alert successfully: " + String(jsonBuffer));
            portEXIT_CRITICAL(&serialMutex);
        } else {
            portENTER_CRITICAL(&serialMutex);
            Serial.println("\n[MQTT] Push failed!");
            portEXIT_CRITICAL(&serialMutex);
        }
    }
}
