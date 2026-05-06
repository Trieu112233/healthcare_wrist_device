#include "ai_inference.h"
#include "sensors.h"
#include "../include/config.h"
#include "network_app.h"
#include "display_app.h"

#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "model-parameters/model_variables.h"

// Biến lưu vị trí vòng đệm IMU khi vào suy luận
static int inference_imu_head = 0;

// Context truy xuất dữ liệu
struct IMUSignalContext {
    float* buffer;
    size_t num_samples;
};

struct AudioSignalContext {
    int16_t* buffer;
    size_t num_samples;
};

static IMUSignalContext imu_ctx;
static AudioSignalContext audio_ctx;

static float inference_imu_snapshot[IMU_TOTAL_SAMPLES * 6]; // Dùng để chốt snapshot của IMU khi vào suy luận

// Callback cho mô hình Fall Detection (IMU)
static int extract_imu_signal(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        size_t logical_feature_idx = offset + i;
        size_t sample_idx = logical_feature_idx / 6;  
        size_t axis_idx = logical_feature_idx % 6;    
        
        // Đọc vòng từ buffer
        size_t physical_sample_idx = (inference_imu_head + sample_idx) % IMU_TOTAL_SAMPLES;
        out_ptr[i] = inference_imu_snapshot[physical_sample_idx * 6 + axis_idx];
    }
    return 0; 
}

// Callback cho mô hình Scream Detection (Audio)
static int extract_audio_signal(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        if (offset + i < audio_ctx.num_samples) {
            out_ptr[i] = (float)audio_ctx.buffer[offset + i] / 32768.0f;  // Normalize [-1, 1]
        } else {
            out_ptr[i] = 0.0f;
        }
    }
    return 0;
}

void run_ai_inference() {
    // Chốt snapshot buffer của IMU
    portENTER_CRITICAL(&dataReadyMutex);
    inference_imu_head = imu_head;
    memcpy(inference_imu_snapshot, imuBuffer, IMU_TOTAL_SAMPLES * 6 * sizeof(float));
    portEXIT_CRITICAL(&dataReadyMutex);

    portENTER_CRITICAL(&serialMutex);
    Serial.println("\nSEQUENTIAL INFERENCE RUNNING   ║");
    Serial.println("\n[1/2] Running Fall Detection...");
    portEXIT_CRITICAL(&serialMutex);

    uint32_t t1 = millis();
    
    // ===== [1/2] FALL DETECTION =====
    imu_ctx.buffer = inference_imu_snapshot;
    imu_ctx.num_samples = IMU_TOTAL_SAMPLES * 6;
    
    signal_t imu_signal;
    imu_signal.total_length = IMU_TOTAL_SAMPLES * 6;
    imu_signal.get_data = extract_imu_signal;
    
    ei_impulse_result_t fall_result;
    EI_IMPULSE_ERROR fall_err = run_classifier(&impulse_handle_868007_1, &imu_signal, &fall_result, false);
    
    uint32_t t2 = millis();

    // ===== [2/2] SCREAM DETECTION =====
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

    // ===== PUBLISH DATA TO CLOUD =====
    float actFall = 0.0f;
    float actScream = 0.0f;
    
    if (fall_err == EI_IMPULSE_OK) {
        for (size_t i = 0; i < impulse_handle_868007_1.impulse->label_count; i++) {
            if (String(fall_result.classification[i].label) == "fall") { 
                actFall = fall_result.classification[i].value;
            }
        }
    }

    if (scream_err == EI_IMPULSE_OK) {
        for (size_t i = 0; i < impulse_handle_916888_1.impulse->label_count; i++) {
            if (String(scream_result.classification[i].label) == "scream") { 
                actScream = scream_result.classification[i].value;
            }
        }
    }

    updateAlertUI(actFall, actScream);

    if (actFall >= ALERT_THRESHOLD || actScream >= ALERT_THRESHOLD) {
        publishAlert(actFall, actScream);
    }
}