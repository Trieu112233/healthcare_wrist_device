#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

extern TaskHandle_t imuTaskHandle;
extern TaskHandle_t micTaskHandle;

#define IMU_TOTAL_SAMPLES 200
#define AUDIO_SAMPLE_RATE 16000
#define AUDIO_SAMPLES_PER_CYCLE 16000

extern float* imuBuffer;
extern int16_t* audioBuffer;

extern volatile int imu_head;
extern volatile bool audioDataReady;

extern portMUX_TYPE serialMutex;
extern portMUX_TYPE dataReadyMutex;

void allocateSensorBuffers();
void setupSensors();
void startSensorTasks();
void goToDeepSleep();

#endif // SENSORS_H
