#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define IMU_TOTAL_SAMPLES 200          // 4 giây * 50Hz = 200 mẫu
#define AUDIO_SAMPLE_RATE 16000
#define AUDIO_SAMPLES_PER_CYCLE 16000  // 1 giây = 16000 mẫu

// Buffers (Dùng chung cho Inference)
extern float* imuBuffer;
extern int16_t* audioBuffer;

// Biến trạng thái
extern volatile int imu_head;
extern bool audioDataReady;

// Mutex (Đồng bộ)
extern portMUX_TYPE serialMutex;
extern portMUX_TYPE dataReadyMutex;

// Khai báo hàm
void allocateSensorBuffers();
void setupSensors();
void startSensorTasks();

#endif // SENSORS_H
