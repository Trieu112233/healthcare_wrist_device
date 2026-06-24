#include "ai_inference.h"
#include "sensors.h"
#include "../include/config.h"
#include "network_app.h"
#include "display_app.h"

#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "model-parameters/model_variables.h"

static int inference_imu_head = 0;

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

static float inference_imu_snapshot[IMU_TOTAL_SAMPLES * 6];

struct FallMotionStats {
    float maxAccelDeltaG;
};

static FallMotionStats calculate_fall_motion_stats() {
    FallMotionStats stats = {0.0f};

    for (size_t i = 0; i < IMU_TOTAL_SAMPLES; i++) {
        float ax = inference_imu_snapshot[i * 6 + 0];
        float ay = inference_imu_snapshot[i * 6 + 1];
        float az = inference_imu_snapshot[i * 6 + 2];

        float accelG = sqrtf(ax * ax + ay * ay + az * az) / 9.81f;
        float accelDeltaG = fabsf(accelG - 1.0f);

        if (accelDeltaG > stats.maxAccelDeltaG) stats.maxAccelDeltaG = accelDeltaG;
    }

    return stats;
}

static int extract_imu_signal(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        size_t logical_feature_idx = offset + i;
        size_t sample_idx = logical_feature_idx / 6;  
        size_t axis_idx = logical_feature_idx % 6;    
        
        size_t physical_sample_idx = (inference_imu_head + sample_idx) % IMU_TOTAL_SAMPLES;
        out_ptr[i] = inference_imu_snapshot[physical_sample_idx * 6 + axis_idx];
    }
    return 0; 
}

static int extract_audio_signal(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        if (offset + i < audio_ctx.num_samples) {
            out_ptr[i] = (float)audio_ctx.buffer[offset + i];
        } else {
            out_ptr[i] = 0.0f;
        }
    }
    return 0;
}

void run_ai_inference() {
    // Snapshot the IMU ring buffer so the fall model sees one coherent 4s window.
    portENTER_CRITICAL(&dataReadyMutex);
    inference_imu_head = imu_head;
    memcpy(inference_imu_snapshot, imuBuffer, IMU_TOTAL_SAMPLES * 6 * sizeof(float));
    portEXIT_CRITICAL(&dataReadyMutex);

    portENTER_CRITICAL(&serialMutex);
    Serial.println("\nSEQUENTIAL INFERENCE RUNNING");
    Serial.println("\n[1/2] Running Fall Detection...");
    portEXIT_CRITICAL(&serialMutex);

    uint32_t t1 = millis();
    
    imu_ctx.buffer = inference_imu_snapshot;
    imu_ctx.num_samples = IMU_TOTAL_SAMPLES * 6;
    
    signal_t imu_signal;
    imu_signal.total_length = IMU_TOTAL_SAMPLES * 6;
    imu_signal.get_data = extract_imu_signal;
    
    ei_impulse_result_t fall_result;
    EI_IMPULSE_ERROR fall_err = run_classifier(&impulse_handle_868007_1, &imu_signal, &fall_result, false);
    
    uint32_t t2 = millis();

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
    EI_IMPULSE_ERROR scream_err = run_classifier(&impulse_handle_1011331_1, &audio_signal, &scream_result, false);
    
    uint32_t t4 = millis();

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
        for (size_t i = 0; i < impulse_handle_1011331_1.impulse->label_count; i++) {
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
        for (size_t i = 0; i < impulse_handle_1011331_1.impulse->label_count; i++) {
            if (String(scream_result.classification[i].label) == "scream") { 
                actScream = scream_result.classification[i].value;
            }
        }
    }

    FallMotionStats fallMotionStats = calculate_fall_motion_stats();
    bool hasFallMotion = fallMotionStats.maxAccelDeltaG >= FALL_MIN_ACCEL_DELTA_G;

    if (actFall >= FALL_ALERT_THRESHOLD && !hasFallMotion) {
        portENTER_CRITICAL(&serialMutex);
        Serial.printf("[FALL] Suppressed static pose: fall=%.2f maxAccelDeltaG=%.2f\n",
                      actFall, fallMotionStats.maxAccelDeltaG);
        portEXIT_CRITICAL(&serialMutex);
        actFall = 0.0f;
    }

    updateAlertUI(actFall, actScream);

    if (actFall >= FALL_ALERT_THRESHOLD || actScream >= SCREAM_ALERT_THRESHOLD) {
        publishAlert(actFall, actScream);
    }
    
}
