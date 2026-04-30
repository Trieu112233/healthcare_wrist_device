# 🔧 Firmware Multi-Model - Thiết bị đeo tay chăm sóc sức khỏe

## I. Tổng Quan

**Firmware Multi-Model** là hệ thống nhúng trên **Seeed Studio XIAO ESP32-S3** thực hiện xử lý dữ liệu cảm biến và suy luận máy học trực tiếp trên thiết bị (Edge Computing).

| Thông số | Giá trị |
|---------|--------|
| **Bộ vi xử lý** | ESP32-S3 (Dual-core Xtensa) |
| **RAM** | 8MB PSRAM + 512KB SRAM |
| **Chức năng chính** | Fall Detection + Scream Detection |
| **Framework** | Arduino + PlatformIO |
| **Kết nối** | WiFi 802.11b/g/n + MQTT |

---

## II. Kiến Trúc Hệ Thống

### A. Sơ đồ Khối

```
┌──────────────────────────────────────────┐
│        Seeed XIAO ESP32-S3               │
├──────────────────────────────────────────┤
│                                          │
│  ┌─────────┐      ┌──────────┐          │
│  │   IMU   │      │ Microphone│         │
│  │LSM6DS3  │      │ INMP441   │         │
│  │(I2C)    │      │ (I2S)     │         │
│  └────┬────┘      └─────┬────┘          │
│       │                 │               │
│       ├─────┬───────────┤               │
│       │     │           │               │
│  ┌────▼─────▼───────────▼───┐          │
│  │   Ring Buffers (PSRAM)    │          │
│  │ - IMU: 4s (200 samples)   │          │
│  │ - Audio: 1s (16000 samples)│         │
│  └────┬──────────────────────┘          │
│       │                                 │
│  ┌────▼───────────────────┐             │
│  │  Edge Impulse Models   │             │
│  │ - Fall Detection (IMU) │             │
│  │ - Scream Detection (Audio)           │
│  └────┬──────────────────┘              │
│       │                                 │
│  ┌────▼──────────┐     ┌────────────┐  │
│  │  MQTT Publish │     │  Display   │  │
│  │  (HiveMQ)     │     │ LVGL + GC9A01│ │
│  └───────────────┘     └────────────┘  │
│                                         │
└─────────────────────────────────────────┘
```

### B. Các Module Chính

| Module | Chức năng | Ngôn ngữ |
|--------|----------|----------|
| **sensors.cpp** | Đọc IMU & Microphone | C++ |
| **ai_inference.cpp** | Chạy mô hình ML | C++ |
| **network_app.cpp** | WiFi + MQTT | C++ |
| **display_app.cpp** | Hiển thị LVGL | C++ |
| **main.cpp** | FreeRTOS scheduler | C++ |

---

## III. Chi Tiết Các Thành Phần

### 1. Cảm Biến (Sensors)

#### IMU - LSM6DS3
- **Tần số lấy mẫu:** 50 Hz (20ms/mẫu)
- **Dữ liệu:** 6 trục (Acc X/Y/Z + Gyro X/Y/Z)
- **Ring Buffer:** 200 mẫu = 4 giây liên tục
- **Hiệu chỉnh:** Gyro bias calibration tự động
- **Giao tiếp:** I2C (SDA: D4, SCL: D5)

#### Microphone - INMP441
- **Tần số lấy mẫu:** 16 kHz
- **Độ phân giải:** 16-bit
- **Buffer:** 16000 mẫu = 1 giây
- **Giao tiếp:** I2S (SCK: D1, WS: D9, SD: D6)

#### FreeRTOS Tasks
```c
// Task IMU chạy trên Core 0
xTaskCreatePinnedToCore(imuTask, "IMU_Task", 4096, NULL, 2, NULL, 0);

// Task Mic chạy trên Core 1
xTaskCreatePinnedToCore(micTask, "Mic_Task", 8192, NULL, 2, NULL, 1);
```

### 2. Suy Luận Máy Học (AI Inference)

#### Mô hình 1: Fall Detection (IMU)
- **Input:** 4 giây dữ liệu IMU (200 mẫu × 6 trục)
- **Model ID:** 868007_1
- **Output:** Fall confidence (0-100%)
- **Xử lý:** ~80ms

#### Mô hình 2: Scream Detection (Audio)
- **Input:** 1 giây audio (16000 mẫu @ 16kHz)
- **Model ID:** 916888_1
- **Output:** Scream confidence (0-100%)
- **Xử lý:** ~70ms
- **Tiền xử lý:** MFCC feature extraction

#### Quy trình Inference
```cpp
// Sequential inference mỗi 1 giây
if (hasAudioData) {
    // 1. Chốt snapshot IMU
    memcpy(inference_imu_snapshot, imuBuffer, ...);
    
    // 2. Fall detection
    run_classifier(&impulse_handle_868007_1, &imu_signal, &fall_result, false);
    
    // 3. Scream detection  
    run_classifier(&impulse_handle_916888_1, &audio_signal, &scream_result, false);
    
    // 4. Cập nhật UI & Publish
    if (confidence >= ALERT_THRESHOLD)
        publishAlert(fall_conf, scream_conf);
}
```

### 3. Kết Nối Mạng (Network)

#### WiFi Setup
- **Mode:** Station (STA)
- **TX Power:** -17 dBm (tiết kiệm năng lượng)
- **NTP Time Sync:** GMT+7 (Múi giờ Việt Nam)

#### MQTT Configuration
- **Server:** HiveMQ Cloud (Bảo mật TLS/SSL)
- **Port:** 8883
- **Topic:** Tùy định nghĩa (config.h)
- **Payload:** JSON format
```json
{
  "deviceId": "xiao_esp32s3_01",
  "fall_confidence": 0.95,
  "scream_confidence": 0.0
}
```

### 4. Hiển Thị (Display)

#### Hardware
- **Display:** GC9A01 (240×240 pixel, circular)
- **Giao tiếp:** Hardware SPI
- **Độ sáng:** Khả năng điều chỉnh (0-255)

#### UI Framework
- **LVGL 8.3.11** - Lightweight GUI
- **LovyanGFX** - Graphics driver
- **SquareLine Studio** - Design tool

#### States
- **SAFE:** Vòng xoay xanh (bình thường)
- **FALL!!!:** Vòng báo động đỏ
- **SCREAM!!!:** Vòng báo động đỏ
- **FALL AND SCREAM!!!:** Cảnh báo tối đa

---

## IV. Luồng Hoạt Động

### Setup Phase (Khởi động)
1. Khởi tạo display (LVGL + GC9A01)
2. Kết nối WiFi & đồng bộ thời gian NTP
3. Cấp phát bộ nhớ cho buffers (PSRAM)
4. Khởi tạo IMU & Microphone
5. Hiệu chỉnh Gyro bias
6. Khởi chạy FreeRTOS tasks
7. Warm-up 4 giây

### Main Loop (Chạy liên tục)
```
loop() {
    lv_timer_handler();              // Cập nhật display
    processMQTT();                   // Duy trì MQTT
    updateTimeUI();                  // Cập nhật giờ
    
    if (motion_detected) {
        lastMotionTime = millis();   // Reset inactivity
    }
    
    if (millis() - lastMotionTime > 10s) {
        goToDeepSleep();             // Vào deep sleep nếu không có chuyển động
    }
    
    if (audioDataReady) {
        run_ai_inference();          // Chạy 2 mô hình ML
        updateAlertUI(...);          // Cập nhật giao diện
        publishAlert(...);           // Gửi MQTT nếu cảnh báo
    }
}
```

### Deep Sleep Mode
- **Trigger:** 10 giây không có chuyển động
- **Công suất:** ~100 μA
- **Wake-up:** Motion từ IMU interrupt
- **RTC Storage:** Lưu Gyro bias qua sleep

---

## V. Cấu Hình & Dependencies

### platformio.ini
```ini
[env:seeed_xiao_esp32s3]
platform = espressif32
board = seeed_xiao_esp32s3
framework = arduino

lib_deps =
    SparkFun LSM6DS3 Breakout@^1.0.0
    knolleary/PubSubClient@^2.8
    bblanchon/ArduinoJson@^6.21.3
    lovyan03/LovyanGFX@^1.1.12

build_flags =
    -I lib/deploy
    -D BOARD_HAS_PSRAM
    -D EI_PORTING_ESPRESSIF=1
    -O3
```

### config.h
```c
// WiFi credentials
#define WIFI_SSID "your_network"
#define WIFI_PASS "your_password"

// MQTT (HiveMQ)
#define MQTT_SERVER "your_hivemq_server.com"
#define MQTT_PORT 8883
#define MQTT_USER "mqtt_user"
#define MQTT_PASS "mqtt_password"

// Pins
#define SDA_PIN D4      // IMU
#define SCL_PIN D5
#define I2S_SCK D1      // Mic
#define I2S_WS D9
#define I2S_SD D6

// Alert threshold
#define ALERT_THRESHOLD 0.80f  // 80% confidence
```

---

## VI. Hiệu Năng & Tiêu Thụ Điện

| Chỉ số | Giá trị |
|--------|--------|
| **Inference latency** | ~150ms (cả 2 mô hình) |
| **Display refresh** | 60 FPS |
| **Memory PSRAM** | ~148 KB (IMU) + 32 KB (Audio) |
| **WiFi latency** | ~100ms publish |
| **Công suất chạy** | ~150 mA |
| **Công suất idle** | ~50 mA |
| **Công suất sleep** | ~100 μA |

---

## VII. Build & Deploy

### Compile
```bash
platformio run -e seeed_xiao_esp32s3
```

### Upload
```bash
platformio run --target upload -e seeed_xiao_esp32s3
```

### Serial Monitor
```bash
platformio device monitor -e seeed_xiao_esp32s3
```

---

## VIII. Kết Luận

Firmware Multi-Model cung cấp:
- ✅ **Xử lý real-time** trên edge device
- ✅ **Tiêu thụ điện tối ưu** với deep sleep mode
- ✅ **Kết nối đám mây** qua MQTT bảo mật
- ✅ **Giao diện trực quan** với LVGL animation
- ✅ **Mở rộng dễ dàng** với modular architecture

Hệ thống này là nền tảng ideal cho các ứng dụng IoT chăm sóc sức khỏe với khả năng phát hiện ngã và cảnh báo khẩn cấp.

---

**Tài liệu này phục vụ cho nghiên cứu khóa luận tốt nghiệp - Hệ thống Đeo tay Chăm sóc Sức khỏe**
