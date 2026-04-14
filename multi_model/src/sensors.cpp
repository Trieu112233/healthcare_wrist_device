#include "sensors.h"
#include "../include/config.h"
#include <Wire.h>
#include "SparkFunLSM6DS3.h"
#include "driver/i2s.h"
#include "esp_heap_caps.h"

// ===== IMU CONFIGURATION =====
LSM6DS3 myIMU(I2C_MODE, 0x6B);
float gyroBiasX = 0, gyroBiasY = 0, gyroBiasZ = 0;

// Các biến phục vụ Ring Buffer
float* imuBuffer = NULL;
volatile int imu_head = 0;

// ===== AUDIO CONFIGURATION =====
#define I2S_PORT I2S_NUM_0
int16_t* audioBuffer = NULL;

// ===== SYNCHRONIZATION =====
portMUX_TYPE serialMutex = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE dataReadyMutex = portMUX_INITIALIZER_UNLOCKED;

bool audioDataReady = false;

// ========== THUẬT TOÁN I2S ==========
void setupI2S() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = AUDIO_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 128,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_SD
    };

    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_PORT, &pin_config);
}

// ========== THUẬT TOÁN IMU ==========
void setupIMU() {
    Wire.begin(SDA_PIN, SCL_PIN);
    if (myIMU.begin() != 0) {
        portENTER_CRITICAL(&serialMutex);
        Serial.println("❌ ERROR: IMU not found!");
        portEXIT_CRITICAL(&serialMutex);
        while(1) delay(100);
    }
    
    portENTER_CRITICAL(&serialMutex);
    Serial.println("Calibrating Gyro in 3s...");
    portEXIT_CRITICAL(&serialMutex);
    
    delay(1000);
    
    int numSamples = 200;
    for (int i = 0; i < numSamples; i++) {
        gyroBiasX += myIMU.readFloatGyroX();
        gyroBiasY += myIMU.readFloatGyroY();
        gyroBiasZ += myIMU.readFloatGyroZ();
        delay(10); 
    }
    
    gyroBiasX /= numSamples;
    gyroBiasY /= numSamples;
    gyroBiasZ /= numSamples;
    
    portENTER_CRITICAL(&serialMutex);
    Serial.print("Calib done! Gyro Bias: ");
    Serial.print(gyroBiasX); Serial.print(", ");
    Serial.print(gyroBiasY); Serial.print(", ");
    Serial.println(gyroBiasZ);
    portEXIT_CRITICAL(&serialMutex);
    delay(1000);
}

// ========== TASK MIC ==========
void micTask(void *pvParameters) {
    int32_t raw_buf[256];
    size_t bytes_read;

    while (1) {
        int samples_captured = 0;
        
        while (samples_captured < AUDIO_SAMPLES_PER_CYCLE) {
            esp_err_t result = i2s_read(I2S_PORT, &raw_buf, sizeof(raw_buf), &bytes_read, portMAX_DELAY);
            if (result == ESP_OK && bytes_read > 0) {
                int samples_read = bytes_read / sizeof(int32_t);
                for (int i = 0; i < samples_read && samples_captured < AUDIO_SAMPLES_PER_CYCLE; i++) {
                    audioBuffer[samples_captured] = (int16_t)(raw_buf[i] >> 14);
                    samples_captured++;
                }
            }
            taskYIELD();
        }

        portENTER_CRITICAL(&dataReadyMutex);
        audioDataReady = true;
        portEXIT_CRITICAL(&dataReadyMutex);

        portENTER_CRITICAL(&serialMutex);
        Serial.println("[MIC] ✓ Audio buffer ready");
        portEXIT_CRITICAL(&serialMutex);

        // Chờ Main Loop xử lý xong rồi thu chu kỳ mới
        while (audioDataReady) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}

// ========== TASK IMU ==========
void imuTask(void *pvParameters) {
    while (1) {
        float ax = myIMU.readFloatAccelX() * 9.81f;
        float ay = myIMU.readFloatAccelY() * 9.81f;
        float az = myIMU.readFloatAccelZ() * 9.81f;
        
        float gx = (myIMU.readFloatGyroX() - gyroBiasX) * (3.14159265f / 180.0f);
        float gy = (myIMU.readFloatGyroY() - gyroBiasY) * (3.14159265f / 180.0f);
        float gz = (myIMU.readFloatGyroZ() - gyroBiasZ) * (3.14159265f / 180.0f);
        
        imuBuffer[imu_head * 6 + 0] = ax;
        imuBuffer[imu_head * 6 + 1] = ay;
        imuBuffer[imu_head * 6 + 2] = az;
        imuBuffer[imu_head * 6 + 3] = gx;
        imuBuffer[imu_head * 6 + 4] = gy;
        imuBuffer[imu_head * 6 + 5] = gz;

        // Trượt
        imu_head = (imu_head + 1) % IMU_TOTAL_SAMPLES;

        vTaskDelay(20 / portTICK_PERIOD_MS);  // 50 Hz (20ms)
    }
}

// ========== HÀM BỌC NGOÀI ==========
void allocateSensorBuffers() {
    size_t audioBufSize = AUDIO_SAMPLES_PER_CYCLE * sizeof(int16_t);
    audioBuffer = (int16_t*)heap_caps_malloc(audioBufSize, MALLOC_CAP_SPIRAM);
    if (!audioBuffer) audioBuffer = (int16_t*)malloc(audioBufSize);

    size_t imuBufSize = IMU_TOTAL_SAMPLES * 6 * sizeof(float);
    imuBuffer = (float*)heap_caps_malloc(imuBufSize, MALLOC_CAP_SPIRAM);
    if (!imuBuffer) imuBuffer = (float*)malloc(imuBufSize);

    if (!audioBuffer || !imuBuffer) {
        portENTER_CRITICAL(&serialMutex);
        Serial.println("❌ CRITICAL: Memory allocation failed!");
        portEXIT_CRITICAL(&serialMutex);
        while(1);
    }
    
    portENTER_CRITICAL(&serialMutex);
    Serial.printf("\n[MEMORY] Malloc Audio Buffer: %zu bytes\n", audioBufSize);
    Serial.printf("[MEMORY] Malloc IMU Buffer: %zu bytes\n", imuBufSize);
    if (psramFound()) {
        uint32_t psramUsed = ESP.getPsramSize() - ESP.getFreePsram();
        Serial.printf("[MEMORY] Total PSRAM currently used: %u bytes\n", psramUsed);
    }
    portEXIT_CRITICAL(&serialMutex);
}

void setupSensors() {
    setupI2S();
    setupIMU();
}

void startSensorTasks() {
    xTaskCreatePinnedToCore(micTask, "Mic_Task", 8192, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(imuTask, "IMU_Task", 4096, NULL, 2, NULL, 0);
}
