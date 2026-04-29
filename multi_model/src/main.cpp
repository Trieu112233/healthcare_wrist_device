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
#include <lvgl.h> 

unsigned long lastMotionTime = 0;
unsigned long lastTimeUpdate = 0;
const unsigned long SLEEP_TIMEOUT = 10000;

// ========== SETUP ==========
void setup() {
    Serial.begin(115200);
    delay(100);

    setupDisplay();
    for(int i=0; i<10; i++) { lv_timer_handler(); delay(5); }

    portENTER_CRITICAL(&serialMutex);
    Serial.println("\n╔═════════════════════════════════════╗");
    Serial.println("║  ESP32-S3 MULTI-MODEL SYSTEM       ║");
    Serial.println("║  Fall + Scream Detection           ║");
    Serial.println("═════════════════════════════════════");
    portEXIT_CRITICAL(&serialMutex);

    // 2. KẾT NỐI MẠNG & THỜI GIAN (Blocking)
    setupNetwork(); 

    // 3. CẬP NHẬT GIỜ LÊN MÀN HÌNH (Lúc này có mạng -> Hiện giờ, đổi chữ thành "SAFE")
    updateTimeUI();
    for(int i=0; i<10; i++) { lv_timer_handler(); delay(5); } // Kịp render giờ và đổi chữ


    // 2. Cấp phát vùng nhớ, Khởi tạo MCU I2S, I2C/IMU
    allocateSensorBuffers();
    setupSensors();

    portENTER_CRITICAL(&serialMutex);
    Serial.println("\n[INIT] Starting FreeRTOS tasks...");
    portEXIT_CRITICAL(&serialMutex);
    
    // 3. Kích hoạt FreeRTOS Tasks
    startSensorTasks();

    portENTER_CRITICAL(&serialMutex);
    Serial.println("\nSYSTEM READY");
    Serial.println("  Waiting 4s for IMU buffer warm-up...\n");
    portEXIT_CRITICAL(&serialMutex);

    // 5. WARM-UP 4 GIÂY (Cho phép màn hình vẫn chạy animation)
    unsigned long start_warmup = millis();
    while (millis() - start_warmup < 4000) {
        lv_timer_handler(); // Giữ cho màn hình không bị đơ
        delay(10);
    }

    lastMotionTime = millis(); // Reset lại đồng hồ đếm thời gian sau khi IMU đã ổn định
}


// ========== MAIN LOOP ==========
void loop() {
    bool hasAudioData = false;

    // --- BỔ SUNG CHO MÀN HÌNH LVGL ---
    lv_timer_handler(); // Hàm xử lý công việc hiển thị (vẽ, update text, animation...)
    // ---------------------------------

    // Xem liệu Mic đã thu đủ 1s audio chưa
    portENTER_CRITICAL(&dataReadyMutex);
    hasAudioData = audioDataReady;
    portEXIT_CRITICAL(&dataReadyMutex);

    // Duy trì Client MQTT
    processMQTT();

    if (millis() - lastTimeUpdate > 1000) {
        updateTimeUI();
        lastTimeUpdate = millis();
    }

    int currentHead = imu_head;
    if (currentHead >= 0) {
        int last_idx = (currentHead == 0) ? IMU_TOTAL_SAMPLES - 1 : currentHead - 1;
        float ax = imuBuffer[last_idx * 6 + 0];
        float ay = imuBuffer[last_idx * 6 + 1];
        float az = imuBuffer[last_idx * 6 + 2];
        
        // Tính tổng gia tốc Acc = sqrt(ax^2 + ay^2 + az^2)
        float totalAcc = sqrt(ax*ax + ay*ay + az*az);
        float totalG = totalAcc / 9.81f;
        float delta = abs(totalG - 1.0f); 
        
        // Kiem tra nguong
        if (delta > 0.15f) {
            lastMotionTime = millis(); // Có rung lắc -> Cập nhật lại
        }
    }

    // ĐÃ QUÁ timeout MÀ KHÔNG AI CỬ ĐỘNG -> ĐI NGỦ
    if (millis() - lastMotionTime > SLEEP_TIMEOUT) {
        goToDeepSleep();
    }

    // Đồng bộ kích hoạt suy luận theo 1s
    if (hasAudioData) {
        run_ai_inference();

        // reset flag
        portENTER_CRITICAL(&dataReadyMutex);
        audioDataReady = false;
        portEXIT_CRITICAL(&dataReadyMutex);
    }

    delay(10); 
}
