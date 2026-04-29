#include "network_app.h"
#include "../include/config.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h> // <=== Bổ sung thư viện time.h ở đầu file mạng

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);
extern portMUX_TYPE serialMutex;

void setupNetwork() {
    portENTER_CRITICAL(&serialMutex);
    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_SSID);
    portEXIT_CRITICAL(&serialMutex);

    WiFi.mode(WIFI_STA);
    WiFi.setTxPower(WIFI_POWER_8_5dBm); 
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    portENTER_CRITICAL(&serialMutex);
    Serial.println("\nWiFi connected!");
    portEXIT_CRITICAL(&serialMutex);

    // ==========================================
    // KHỞI TẠO ĐỒNG BỘ THỜI GIAN QUA INTERNET (NTP)
    // ==========================================
    const char* ntpServer = "pool.ntp.org";
    const long  gmtOffset_sec = 7 * 3600; // Múi giờ Việt Nam (GMT+7)
    const int   daylightOffset_sec = 0;   // Không có giờ mùa hè

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 10000)) { // Chờ tối đa 10s để đồng bộ mạng
        Serial.println("Time synchronized successfully!");
    } else {
        Serial.println("Failed to obtain time");
    }
    // ==========================================

    // KẾT NỐI QUA BẢO MẬT TLS
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
