#include <Arduino.h>

/*
 * FIRMWARE: MULTI-MODEL SEQUENTIAL INFERENCE
 * Fall Detection (IMU 4s Ring Buffer) + Scream Detection (Audio 1s)
 * Board: Seeed Studio XIAO ESP32-S3
 * 
 * Sensors:
 *   - IMU (LSM6DS3): I2C (D4/D5) 
 *   - Mic (INMP441): I2S (pins 7,8,9)
 */

#include <Wire.h>
#include "SparkFunLSM6DS3.h"
#include "driver/i2s.h"
#include "esp_heap_caps.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// ===== EDGE IMPULSE MODEL INCLUDES =====
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "model-parameters/model_variables.h"

// ===== GPIO CONFIGURATION =====
#define SDA_PIN D4
#define SCL_PIN D5
#define I2S_WS   8
#define I2S_SCK  9  
#define I2S_SD   7

// ===== IMU CONFIGURATION =====
#define IMU_TOTAL_SAMPLES 200  // 4 giây * 50Hz = 200 mẫu
LSM6DS3 myIMU(I2C_MODE, 0x6B);
float* imuBuffer = NULL;
float gyroBiasX = 0, gyroBiasY = 0, gyroBiasZ = 0;

// Các biến phục vụ Ring Buffer cho IMU
volatile int imu_head = 0;             // Con trỏ ghi mẫu mới nhất của IMU
int inference_imu_head = 0;            // Lưu vị trí head ngay tại lúc kích hoạt Inference

// ===== AUDIO CONFIGURATION =====
#define I2S_PORT I2S_NUM_0
#define AUDIO_SAMPLE_RATE 16000
#define AUDIO_SAMPLES_PER_CYCLE 16000  // 1 giây = 16000 mẫu
int16_t* audioBuffer = NULL;

// ===== SYNCHRONIZATION PRIMITIVES =====
portMUX_TYPE serialMutex = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE dataReadyMutex = portMUX_INITIALIZER_UNLOCKED;

// ===== DATA READY FLAGS =====
bool audioDataReady = false;

// ===== INFERENCE RESULTS =====
ei_impulse_result_t lastFallResult = {0};
ei_impulse_result_t lastScreamResult = {0};
portMUX_TYPE resultMutex = portMUX_INITIALIZER_UNLOCKED;

// ===== SIGNAL WRAPPERS FOR MODELS =====
struct IMUSignalContext {
    float* buffer;
    size_t num_samples;
};

struct AudioSignalContext {
    int16_t* buffer;
    size_t num_samples;
};

IMUSignalContext imu_ctx;
AudioSignalContext audio_ctx;

// Callback for IMU signal (Ring Buffer Mode - Trích xuất xoay vòng)
int extract_imu_signal(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        size_t logical_feature_idx = offset + i;
        size_t sample_idx = logical_feature_idx / 6;  // Mẫu thứ mấy (0 -> 199)
        size_t axis_idx = logical_feature_idx % 6;    // Trục nào (0=ax, 1=ay,... 5=gz)
        
        // Vị trí thực tế trong Ring Buffer: bắt đầu từ điểm cũ nhất (inference_imu_head)
        size_t physical_sample_idx = (inference_imu_head + sample_idx) % IMU_TOTAL_SAMPLES;
        
        out_ptr[i] = imu_ctx.buffer[physical_sample_idx * 6 + axis_idx];
    }
    return 0; 
}

// Callback for Audio signal
int extract_audio_signal(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        if (offset + i < audio_ctx.num_samples) {
            out_ptr[i] = (float)audio_ctx.buffer[offset + i] / 32768.0f;  // Convert int16 to [-1, 1] range
        } else {
            out_ptr[i] = 0.0f;
        }
    }
    return 0;
}

// ========== SETUP I2S FOR MICROPHONE ==========
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

// ========== SETUP IMU ==========
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

// ========== TASK: MICROPHONE DATA COLLECTION ==========
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

        // Chờ Main Loop hoàn thành Inference thì mới thu âm chu kỳ tiếp theo
        while (audioDataReady) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}

// ========== TASK: IMU DATA COLLECTION (CONTINUOUS RING BUFFER) ==========
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

        // Trượt con trỏ Ring Buffer, đầy thì tự động ghi đè về 0
        imu_head = (imu_head + 1) % IMU_TOTAL_SAMPLES;

        vTaskDelay(20 / portTICK_PERIOD_MS);  // 50 Hz sampling (20ms/mẫu)
    }
}

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

    // Initialize Sensors
    setupI2S();
    setupIMU();

    // Allocate Buffers
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

    // === IN DUNG LƯỢNG PSRAM ===
    portENTER_CRITICAL(&serialMutex);
    Serial.printf("\n[MEMORY] Malloc Audio Buffer: %zu bytes\n", audioBufSize);
    Serial.printf("[MEMORY] Malloc IMU Buffer: %zu bytes\n", imuBufSize);
    Serial.printf("[MEMORY] Total requested for Buffers: %zu bytes\n", audioBufSize + imuBufSize);
    
    if (psramFound()) {
        uint32_t psramUsed = ESP.getPsramSize() - ESP.getFreePsram();
        Serial.printf("[MEMORY] Total PSRAM currently used: %u bytes\n", psramUsed);
    }
    portEXIT_CRITICAL(&serialMutex);

    portENTER_CRITICAL(&serialMutex);
    Serial.println("\n[INIT] Starting FreeRTOS tasks...");
    portEXIT_CRITICAL(&serialMutex);
    
    xTaskCreatePinnedToCore(
        micTask, "Mic_Task", 8192, NULL, 2, NULL, 1
    );
    
    xTaskCreatePinnedToCore(
        imuTask, "IMU_Task", 4096, NULL, 2, NULL, 0
    );

    portENTER_CRITICAL(&serialMutex);
    Serial.println("\nSYSTEM READY");
    Serial.println("  Waiting 4s for IMU buffer warm-up...\n");
    portEXIT_CRITICAL(&serialMutex);

    vTaskDelay(4000 / portTICK_PERIOD_MS); 
}

// ========== MAIN LOOP ==========
void loop() {
    bool hasAudioData = false;

    portENTER_CRITICAL(&dataReadyMutex);
    hasAudioData = audioDataReady;
    portEXIT_CRITICAL(&dataReadyMutex);

    // Đồng bộ kích hoạt suy luận dựa trên nhịp 1s một lần của Audio Task
    if (hasAudioData) {
        // Cố định một mốc (Snapshot) lấy 200 mẫu cũ nhất của vòng IMU tại thời điểm này
        inference_imu_head = imu_head; 

        portENTER_CRITICAL(&serialMutex);
        Serial.println("\nSEQUENTIAL INFERENCE RUNNING   ║");
        Serial.println("\n[1/2] Running Fall Detection...");
        portEXIT_CRITICAL(&serialMutex);

        uint32_t t1 = millis();
        
        // ===== [1/2] FALL DETECTION (IMU) =====
        imu_ctx.buffer = imuBuffer;
        imu_ctx.num_samples = IMU_TOTAL_SAMPLES * 6;
        
        signal_t imu_signal;
        imu_signal.total_length = IMU_TOTAL_SAMPLES * 6;
        imu_signal.get_data = extract_imu_signal;
        
        ei_impulse_result_t fall_result;
        // Chú ý: Handle mới của bạn cung cấp là impulse_handle_868007_1
        EI_IMPULSE_ERROR fall_err = run_classifier(&impulse_handle_868007_1, &imu_signal, &fall_result, false);
        
        uint32_t t2 = millis();

        // ===== [2/2] SCREAM DETECTION (AUDIO) =====
        portENTER_CRITICAL(&serialMutex);
        Serial.println("\n[2/2] Running Scream Detection...");
        portEXIT_CRITICAL(&serialMutex);
        
        uint32_t t3 = millis();
        
        audio_ctx.buffer = audioBuffer;
        audio_ctx.num_samples = AUDIO_SAMPLES_PER_CYCLE;
        
        signal_t audio_signal;
        audio_signal.total_length = AUDIO_SAMPLES_PER_CYCLE;
        audio_signal.get_data = extract_audio_signal;
        
        ei_impulse_result_t scream_result;
        // Screm detection giữ Handle cũ: impulse_handle_916888_1
        EI_IMPULSE_ERROR scream_err = run_classifier(&impulse_handle_916888_1, &audio_signal, &scream_result, false);
        
        uint32_t t4 = millis();

        // ===== PRINT RESULTS =====
        portENTER_CRITICAL(&serialMutex);
        
        Serial.print("\nFall: ");
        if (fall_err == EI_IMPULSE_OK) {
            for (size_t i = 0; i < impulse_handle_868007_1.impulse->label_count; i++) {
                Serial.print(fall_result.classification[i].label);
                Serial.print("=");
                Serial.print(fall_result.classification[i].value * 100.0f, 1);
                Serial.print("% ");
            }
        } else {
            Serial.print("ERR");
        }

        Serial.print("\nScream: ");
        if (scream_err == EI_IMPULSE_OK) {
            for (size_t i = 0; i < impulse_handle_916888_1.impulse->label_count; i++) {
                Serial.print(scream_result.classification[i].label);
                Serial.print("=");
                Serial.print(scream_result.classification[i].value * 100.0f, 1);
                Serial.print("% ");
            }
        } else {
            Serial.print("ERR");
        }
        
        Serial.printf("\nTime: %lums\n", (t2 - t1) + (t4 - t3));

        uint32_t sramUsed = ESP.getHeapSize() - ESP.getFreeHeap();
        Serial.printf("SRAM Used: %u bytes\n", sramUsed);
        
        portEXIT_CRITICAL(&serialMutex);

        delay(200); 

        // Khôi phục cờ để bật lại MicTask thu âm cho chu kỳ kết tiếp
        portENTER_CRITICAL(&dataReadyMutex);
        audioDataReady = false;
        portEXIT_CRITICAL(&dataReadyMutex);
    }
    
    // Nhường CPU cho watchdogs
    delay(10); 
}