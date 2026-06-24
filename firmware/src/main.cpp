#include <Arduino.h>
#include <lvgl.h>

#include "../include/config.h"
#include "network_app.h"
#include "sensors.h"
#include "ai_inference.h"
#include "display_app.h"
#include "ble_provisioning.h"

static unsigned long lastMotionTime = 0;
static unsigned long lastTimeUpdate = 0;
static const unsigned long SLEEP_TIMEOUT = 20000;
static const float MOTION_THRESHOLD = 0.10f;

void setup() {
    Serial.begin(115200); 
    delay(100);

    setupDisplay();
    for (int i = 0; i < 10; i++) { 
        lv_timer_handler(); 
        delay(5); 
    }

    portENTER_CRITICAL(&serialMutex);
    Serial.println("\n=====================================");
    Serial.println("ESP32-S3 MULTI-MODEL SYSTEM");
    Serial.println("Fall + Scream Detection");
    Serial.println("=====================================");
    portEXIT_CRITICAL(&serialMutex);

    updateStatusUI("Network: starting");
    if (!setupNetwork()) {
        setupBLEProvisioning();
        while (1) {
            lv_timer_handler();
            delay(5);
        }
    } 
    
    updateTimeUI();
    for (int i = 0; i < 10; i++) { 
        lv_timer_handler(); 
        delay(5); 
    }

    updateStatusUI("Sensors: starting");
    allocateSensorBuffers();
    setupSensors();

    portENTER_CRITICAL(&serialMutex);
    Serial.println("\n[INIT] Bắt đầu khởi động các tác vụ FreeRTOS...");
    portEXIT_CRITICAL(&serialMutex);
    
    updateStatusUI("Tasks: starting");
    startSensorTasks();

    portENTER_CRITICAL(&serialMutex);
    Serial.println("[INIT] HỆ THỐNG SẴN SÀNG!");
    Serial.println("[INIT] Chờ 4s để khởi động buffer bộ đệm IMU...\n");
    portEXIT_CRITICAL(&serialMutex);

    updateStatusUI("IMU warm-up");
    // Warm up the IMU ring buffer before enabling inference and sleep checks.
    unsigned long start_warmup = millis();
    while (millis() - start_warmup < 4000) {
        lv_timer_handler(); 
        delay(10);
    }

    lastMotionTime = millis();
    updateStatusUI("SAFE");
}

void loop() {
    lv_timer_handler();
    processMQTT();

    if (millis() - lastTimeUpdate > 1000) {
        updateTimeUI();
        lastTimeUpdate = millis();
    }

    int currentHead;
    float ax = 0.0f;
    float ay = 0.0f;
    float az = 0.0f;

    portENTER_CRITICAL(&dataReadyMutex);
    currentHead = imu_head;
    if (currentHead >= 0) {
        int last_idx = (currentHead == 0) ? IMU_TOTAL_SAMPLES - 1 : currentHead - 1;
        ax = imuBuffer[last_idx * 6 + 0];
        ay = imuBuffer[last_idx * 6 + 1];
        az = imuBuffer[last_idx * 6 + 2];
    }
    portEXIT_CRITICAL(&dataReadyMutex);

    if (currentHead >= 0) {
        float totalAcc = sqrt(ax*ax + ay*ay + az*az);
        float totalG = totalAcc / 9.81f;
        float delta = abs(totalG - 1.0f);
        
        if (delta > MOTION_THRESHOLD) {
            lastMotionTime = millis();
        }
    }

    if (millis() - lastMotionTime > SLEEP_TIMEOUT) {
        goToDeepSleep();
    }

    bool hasAudioData = false;
    portENTER_CRITICAL(&dataReadyMutex);
    hasAudioData = audioDataReady;
    portEXIT_CRITICAL(&dataReadyMutex);

    if (hasAudioData) {
        run_ai_inference();

        portENTER_CRITICAL(&dataReadyMutex);
        audioDataReady = false;
        portEXIT_CRITICAL(&dataReadyMutex);
    }

    delay(10);
}
