#include <Arduino.h>

/*
 * FIRMWARE: MULTI-MODEL SEQUENTIAL INFERENCE
 * Fall Detection (IMU 4s Ring Buffer) + Scream Detection (Audio 1s)
 * Board: Seeed Studio XIAO ESP32-S3
 */

// ===== MODULE INCLUDES =====
#include "../include/config.h"
#include "network_app.h"
#include "sensors.h"
#include "ai_inference.h"
#include "display_app.h"

// ========== SETUP ==========
void setup() {
    Serial.begin(115200);
    delay(2000);

    portENTER_CRITICAL(&serialMutex);
    Serial.println("\n╔═════════════════════════════════════╗");
    Serial.println("║  ESP32-S3 MULTI-MODEL SYSTEM       ║");
    Serial.println("║  Fall + Scream Detection           ║");
    Serial.println("═════════════════════════════════════");
    portEXIT_CRITICAL(&serialMutex);

    // 1. Kết nối WiFi và MQTT
    setupNetwork();

    // 2. Cấp phát vùng nhớ, Khởi tạo MCU I2S, I2C/IMU
    allocateSensorBuffers();
    setupSensors();
    setupDisplay();

    portENTER_CRITICAL(&serialMutex);
    Serial.println("\n[INIT] Starting FreeRTOS tasks...");
    portEXIT_CRITICAL(&serialMutex);
    
    // 3. Kích hoạt FreeRTOS Tasks
    startSensorTasks();

    portENTER_CRITICAL(&serialMutex);
    Serial.println("\nSYSTEM READY");
    Serial.println("  Waiting 4s for IMU buffer warm-up...\n");
    portEXIT_CRITICAL(&serialMutex);

    vTaskDelay(4000 / portTICK_PERIOD_MS); 
}

// ========== MAIN LOOP ==========
void loop() {
    bool hasAudioData = false;

    // Xem liệu Mic đã thu đủ 1s audio chưa
    portENTER_CRITICAL(&dataReadyMutex);
    hasAudioData = audioDataReady;
    portEXIT_CRITICAL(&dataReadyMutex);

    // Duy trì Client MQTT
    processMQTT();

    // Đồng bộ kích hoạt suy luận theo 1s
    if (hasAudioData) {
        
        run_ai_inference();

        delay(200); 

        // reset flag
        portENTER_CRITICAL(&dataReadyMutex);
        audioDataReady = false;
        portEXIT_CRITICAL(&dataReadyMutex);
    }
    
    delay(10); 
}
