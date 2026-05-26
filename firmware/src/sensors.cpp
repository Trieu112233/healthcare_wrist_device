#include "sensors.h"
#include "../include/config.h"
#include <Wire.h>
#include "SparkFunLSM6DS3.h"
#include "driver/i2s.h"
#include "esp_heap_caps.h"
#include "driver/rtc_io.h"
#include "display_app.h"

LSM6DS3 myIMU(I2C_MODE, 0x6B);

RTC_DATA_ATTR float gyroBiasX = 0;
RTC_DATA_ATTR float gyroBiasY = 0;
RTC_DATA_ATTR float gyroBiasZ = 0;
RTC_DATA_ATTR bool isCalibrated = false;

float* imuBuffer = NULL;
volatile int imu_head = 0;

#define I2S_PORT I2S_NUM_0
int16_t* audioBuffer = NULL;

portMUX_TYPE serialMutex = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE dataReadyMutex = portMUX_INITIALIZER_UNLOCKED;

TaskHandle_t imuTaskHandle = NULL;
TaskHandle_t micTaskHandle = NULL;

volatile bool audioDataReady = false;

static inline int16_t convertI2SSample(int32_t rawSample) {
    return (int16_t)(rawSample >> 14);
}

void setupI2S() {
    updateStatusUI("I2S: starting");

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = AUDIO_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
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

    esp_err_t installResult = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    esp_err_t pinResult = i2s_set_pin(I2S_PORT, &pin_config);

    if (installResult != ESP_OK || pinResult != ESP_OK) {
        portENTER_CRITICAL(&serialMutex);
        Serial.printf("[I2S] ERROR: install=%d pin=%d\n", installResult, pinResult);
        portEXIT_CRITICAL(&serialMutex);
        updateStatusUI("I2S: failed");
        while(1) delay(100);
    }

    updateStatusUI("I2S: ready");
}

void setupIMU() {
    updateStatusUI("IMU: starting");
    Wire.end();
    delay(10);
    Wire.begin(SDA_PIN, SCL_PIN);
    if (myIMU.begin() != 0) {
        portENTER_CRITICAL(&serialMutex);
        Serial.println("[IMU] ERROR: IMU not found!");
        portEXIT_CRITICAL(&serialMutex);
        updateStatusUI("IMU: failed");
        while(1) delay(100);
    }

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
        portENTER_CRITICAL(&serialMutex);
        Serial.println("[IMU] Wakeup by motion interrupt.");
        portEXIT_CRITICAL(&serialMutex);
    } else {
        portENTER_CRITICAL(&serialMutex);
        Serial.println("[IMU] Cold boot. Gyro calibration required.");
        portEXIT_CRITICAL(&serialMutex);
        isCalibrated = false; 
    }

    if (!isCalibrated) {
        portENTER_CRITICAL(&serialMutex);
        Serial.println("Calibrating Gyro in 3s...");
        portEXIT_CRITICAL(&serialMutex);
        updateStatusUI("Gyro: calibrating");
        
        delay(3000);
        
        int numSamples = 200;
        gyroBiasX = 0; gyroBiasY = 0; gyroBiasZ = 0;
        for (int i = 0; i < numSamples; i++) {
            gyroBiasX += myIMU.readFloatGyroX();
            gyroBiasY += myIMU.readFloatGyroY();
            gyroBiasZ += myIMU.readFloatGyroZ();
            delay(10); 
        }
        
        gyroBiasX /= numSamples;
        gyroBiasY /= numSamples;
        gyroBiasZ /= numSamples;
        
        isCalibrated = true;
        
        portENTER_CRITICAL(&serialMutex);
        Serial.print("Calib done! Gyro Bias: ");
        Serial.print(gyroBiasX); Serial.print(", ");
        Serial.print(gyroBiasY); Serial.print(", ");
        Serial.println(gyroBiasZ);
        portEXIT_CRITICAL(&serialMutex);
        updateStatusUI("Gyro: calibrated");
    } else {
        portENTER_CRITICAL(&serialMutex);
        Serial.printf("Using retained gyro bias: X=%.2f, Y=%.2f, Z=%.2f\n", gyroBiasX, gyroBiasY, gyroBiasZ);
        portEXIT_CRITICAL(&serialMutex);
        updateStatusUI("Gyro: retained");
    }
    
    delay(1000);
    updateStatusUI("IMU: ready");
}

void micTask(void *pvParameters) {
    int32_t raw_buf[1024];
    size_t bytes_read;

    while (1) {
        int samples_captured = 0;
        int16_t min_sample = INT16_MAX;
        int16_t max_sample = INT16_MIN;
        int64_t sum_squares = 0;
        
        while (samples_captured < AUDIO_SAMPLES_PER_CYCLE) {
            esp_err_t result = i2s_read(I2S_PORT, &raw_buf, sizeof(raw_buf), &bytes_read, portMAX_DELAY);
            if (result == ESP_OK && bytes_read > 0) {
                int samples_read = bytes_read / sizeof(int32_t);
                for (int i = 0; i < samples_read && samples_captured < AUDIO_SAMPLES_PER_CYCLE; i++) {
                    int16_t sample = convertI2SSample(raw_buf[i]);
                    audioBuffer[samples_captured] = sample;
                    if (sample < min_sample) min_sample = sample;
                    if (sample > max_sample) max_sample = sample;
                    sum_squares += (int32_t)sample * (int32_t)sample;
                    samples_captured++;
                }
            }
            taskYIELD();
        }

        portENTER_CRITICAL(&dataReadyMutex);
        audioDataReady = true;
        portEXIT_CRITICAL(&dataReadyMutex);

        portENTER_CRITICAL(&serialMutex);
        int min_peak = abs((int)min_sample);
        int max_peak = abs((int)max_sample);
        int peak = (min_peak > max_peak) ? min_peak : max_peak;
        float rms = sqrtf((float)sum_squares / (float)AUDIO_SAMPLES_PER_CYCLE);
        Serial.printf("[MIC] Audio buffer ready min=%d max=%d peak=%d rms=%.1f shift=16\n",
                      min_sample, max_sample, peak, rms);
        portEXIT_CRITICAL(&serialMutex);

        // Single-buffer mode: keep the audio window stable while AI consumes it.
        while (audioDataReady) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}

void imuTask(void *pvParameters) {
    while (1) {
        // SparkFun LSM6DS3 returns acceleration in g and gyro in deg/s.
        // The fall model uses acceleration in m/s^2 and angular velocity in rad/s.
        float ax = myIMU.readFloatAccelX() * 9.81f;
        float ay = myIMU.readFloatAccelY() * 9.81f;
        float az = myIMU.readFloatAccelZ() * 9.81f;
        
        float gx = (myIMU.readFloatGyroX() - gyroBiasX) * (3.14159265f / 180.0f);
        float gy = (myIMU.readFloatGyroY() - gyroBiasY) * (3.14159265f / 180.0f);
        float gz = (myIMU.readFloatGyroZ() - gyroBiasZ) * (3.14159265f / 180.0f);
        
        portENTER_CRITICAL(&dataReadyMutex);
        imuBuffer[imu_head * 6 + 0] = ax;
        imuBuffer[imu_head * 6 + 1] = ay;
        imuBuffer[imu_head * 6 + 2] = az;
        imuBuffer[imu_head * 6 + 3] = gx;
        imuBuffer[imu_head * 6 + 4] = gy;
        imuBuffer[imu_head * 6 + 5] = gz;

        imu_head = (imu_head + 1) % IMU_TOTAL_SAMPLES;
        portEXIT_CRITICAL(&dataReadyMutex);

        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

void allocateSensorBuffers() {
    updateStatusUI("Memory: allocating");

    size_t audioBufSize = AUDIO_SAMPLES_PER_CYCLE * sizeof(int16_t);
    audioBuffer = (int16_t*)heap_caps_malloc(audioBufSize, MALLOC_CAP_SPIRAM);
    if (!audioBuffer) audioBuffer = (int16_t*)malloc(audioBufSize);

    size_t imuBufSize = IMU_TOTAL_SAMPLES * 6 * sizeof(float);
    imuBuffer = (float*)heap_caps_malloc(imuBufSize, MALLOC_CAP_SPIRAM);
    if (!imuBuffer) imuBuffer = (float*)malloc(imuBufSize);

    if (!audioBuffer || !imuBuffer) {
        portENTER_CRITICAL(&serialMutex);
        Serial.println("[MEMORY] CRITICAL: buffer allocation failed!");
        portEXIT_CRITICAL(&serialMutex);
        updateStatusUI("Memory: failed");
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
    updateStatusUI("Memory: ready");
}

void setupSensors() {
    setupI2S();
    setupIMU();
}

void startSensorTasks() {
    // Keep IMU sampling isolated from AI/UI load to prioritize fall detection.
    BaseType_t imuTaskResult = xTaskCreatePinnedToCore(imuTask, "IMU_Task", 4096, NULL, 4, &imuTaskHandle, 0);
    BaseType_t micTaskResult = xTaskCreatePinnedToCore(micTask, "Mic_Task", 8192, NULL, 3, &micTaskHandle, 1);

    if (imuTaskResult != pdPASS || micTaskResult != pdPASS) {
        portENTER_CRITICAL(&serialMutex);
        Serial.printf("[TASK] ERROR: IMU=%d MIC=%d\n", imuTaskResult, micTaskResult);
        portEXIT_CRITICAL(&serialMutex);
        updateStatusUI("Tasks: failed");
        while(1) delay(100);
    }

    updateStatusUI("Tasks: ready");
}

void goToDeepSleep() {
    portENTER_CRITICAL(&serialMutex);
    Serial.println("Entering deep sleep mode...");
    portEXIT_CRITICAL(&serialMutex);

    if (imuTaskHandle != NULL) {
        vTaskSuspend(imuTaskHandle);
    }
    if (micTaskHandle != NULL) {
        vTaskSuspend(micTaskHandle);
    }

    // Configure LSM6DS3 motion interrupt as the deep-sleep wake source.
    Wire.beginTransmission(0x6B); Wire.write(0x10); Wire.write(0x60); Wire.endTransmission();
    Wire.beginTransmission(0x6B); Wire.write(0x58); Wire.write(0x80); Wire.endTransmission();
    Wire.beginTransmission(0x6B); Wire.write(0x5C); Wire.write(0x00); Wire.endTransmission();
    Wire.beginTransmission(0x6B); Wire.write(0x5B); Wire.write(0x02); Wire.endTransmission();
    Wire.beginTransmission(0x6B); Wire.write(0x5E); Wire.write(0x20); Wire.endTransmission();

    delay(100); 

    Wire.beginTransmission(0x6B);
    Wire.write(0x1B);
    Wire.endTransmission(false);
    Wire.requestFrom(0x6B, 1);
    if (Wire.available()) {
        Wire.read(); 
    }
    rtc_gpio_pulldown_en((gpio_num_t)IMU_INT1_PIN);
    rtc_gpio_pullup_dis((gpio_num_t)IMU_INT1_PIN);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)IMU_INT1_PIN, 1);

    sleepDisplay();
    i2s_driver_uninstall(I2S_PORT);

    Serial.flush();
    esp_deep_sleep_start();
}
