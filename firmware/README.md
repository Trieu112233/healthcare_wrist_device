# Firmware Multi-Model

## I. Tổng Quan

Firmware này chạy trên **Seeed Studio XIAO ESP32-S3** bằng **Arduino + PlatformIO**. Thiết bị đọc IMU LSM6DS3 và microphone INMP441, chạy 2 mô hình Edge Impulse trực tiếp trên ESP32-S3, hiển thị trạng thái trên màn hình GC9A01/LVGL, và gửi cảnh báo lên MQTT khi xác suất vượt ngưỡng.

| Thông số | Giá trị trong code |
| --- | --- |
| Board | `seeed_xiao_esp32s3` |
| Framework | Arduino, PlatformIO |
| Cảm biến | LSM6DS3 qua I2C, INMP441 qua I2S |
| Display | GC9A01 240x240 qua SPI, LVGL 8.3.11, LovyanGFX |
| ML runtime | Edge Impulse SDK, compiled TFLite/EON models |
| Kết nối | WiFi STA, BLE provisioning, MQTT qua `WiFiClientSecure` |
| Ngưỡng cảnh báo | `ALERT_THRESHOLD = 0.80f` |

## II. Cấu Trúc Firmware

```text
firmware/
|-- include/config.h          # MQTT, chân phần cứng, ngưỡng cảnh báo
|-- platformio.ini            # cấu hình build PlatformIO
|-- src/
|   |-- main.cpp              # setup/loop, điều phối task, deep sleep
|   |-- sensors.cpp/.h        # IMU, I2S microphone, buffer, FreeRTOS tasks
|   |-- ai_inference.cpp/.h   # chạy 2 model Edge Impulse
|   |-- network_app.cpp/.h    # WiFi, NTP, MQTT publish
|   |-- ble_provisioning.cpp/.h
|   |-- display_app.cpp/.h    # LVGL + LovyanGFX
|   `-- ui/                   # UI generate từ SquareLine/LVGL
`-- lib/deploy/               # Edge Impulse SDK + model parameters + model compiled
```

## III. Module Chính

| Module | Vai trò |
| --- | --- |
| `main.cpp` | Khởi tạo display, network, sensor, task; cập nhật UI/time; gọi inference khi audio buffer sẵn sàng; vào deep sleep khi không có chuyển động. |
| `sensors.cpp` | Cấp phát buffer trong PSRAM nếu có, đọc IMU 50 Hz, đọc audio 16 kHz, hiệu chỉnh gyro bias và lưu bias trong RTC memory. |
| `ai_inference.cpp` | Snapshot ring buffer IMU, chạy Fall Detection rồi Scream Detection, cập nhật UI và publish MQTT nếu có cảnh báo. |
| `network_app.cpp` | Đọc WiFi credentials từ NVS namespace `wifi_creds`, đồng bộ NTP GMT+7, kết nối HiveMQ, publish JSON alert. |
| `ble_provisioning.cpp` | Bật BLE provisioning khi chưa có WiFi hoặc WiFi kết nối thất bại; service name `ESP32S3_01`, PoP `12345678`. |
| `display_app.cpp` | Khởi tạo GC9A01, LVGL draw buffer, hiển thị giờ/ngày và các trạng thái SAFE/FALL/SCREAM. |

## IV. Cảm Biến Và Buffer

### LSM6DS3 IMU

- Địa chỉ I2C: `0x6B`
- Chân: `SDA=D5`, `SCL=D4`, `INT1=D2`
- Task: `IMU_Task`, stack `4096`, priority `4`, pinned core `0`
- Chu kỳ đọc: `20 ms` tương đương `50 Hz`
- Buffer: `IMU_TOTAL_SAMPLES = 200`, mỗi mẫu 6 trục
- Input model: `200 x 6 = 1200` feature, tương đương 4 giây
- Accel được đổi từ `g` sang `m/s^2`
- Gyro được trừ bias và đổi từ `deg/s` sang `rad/s`

### INMP441 Microphone

- Chân I2S: `WS=D9`, `SCK=D1`, `SD=D6`
- I2S port: `I2S_NUM_0`
- Sample rate: `16000 Hz`
- I2S input: 32-bit, lấy `raw >> 16` thành `int16_t`
- Buffer: `AUDIO_SAMPLES_PER_CYCLE = 16000`, tương đương 1 giây
- Task: `Mic_Task`, stack `8192`, priority `3`, pinned core `1`
- Chế độ buffer hiện tại là single-buffer: task microphone chờ AI đọc xong rồi mới ghi cửa sổ audio tiếp theo.

## V. Edge Impulse Inference

Firmware include các model trong `lib/deploy/model-parameters/model_variables.h` và `lib/deploy/tflite-model/`.

| Model | Handle | Input | Label |
| --- | --- | --- | --- |
| Fall Detection | `impulse_handle_868007_1` | 200 mẫu IMU, 6 trục, 50 Hz | `fall`, `normal` |
| Scream Detection | `impulse_handle_916888_1` | 16000 mẫu audio, 16 kHz | `non-scream`, `scream` |

Quy trình trong `run_ai_inference()`:

1. Snapshot `imuBuffer` và `imu_head` để model fall nhìn thấy một cửa sổ 4 giây nhất quán.
2. Chạy `run_classifier(&impulse_handle_868007_1, ...)`.
3. Chạy `run_classifier(&impulse_handle_916888_1, ...)`.
4. Lấy confidence của label `fall` và `scream`.
5. Gọi `updateAlertUI(fall, scream)`.
6. Nếu một trong hai confidence `>= 0.80`, gọi `publishAlert(fall, scream)`.

## VI. Network, BLE Provisioning Và MQTT

### WiFi

- WiFi chạy mode `WIFI_STA`.
- Firmware không hardcode SSID/password WiFi; credentials được lưu trong NVS bằng `Preferences`, namespace `wifi_creds`, key `ssid` và `pass`.
- Nếu không có credentials hoặc kết nối thất bại sau 10 lần thử, firmware tắt WiFi và vào BLE provisioning.
- TX power đang cấu hình `WIFI_POWER_8_5dBm`.
- NTP dùng `pool.ntp.org`, múi giờ GMT+7.

### BLE Provisioning

- Scheme: `WIFI_PROV_SCHEME_BLE`
- Security: `WIFI_PROV_SECURITY_1`
- Service name: `ESP32S3_01`
- PoP: `12345678`
- Khi provisioning thành công, firmware lưu credentials vào NVS và restart.

### MQTT

Thông tin MQTT nằm trong `include/config.h`:

| Thông số | Giá trị |
| --- | --- |
| Server | `08c8ad4b15ac4370b81835f72e145e5a.s1.eu.hivemq.cloud` |
| Port | `8883` |
| User | `esp32s3_client` |
| Topic publish | `wearable/xiao_esp32s3_01/alerts` |
| Client id prefix | `esp32s3_01_Client` |

`WiFiClientSecure` hiện đang gọi `setInsecure()`, nghĩa là kết nối dùng TLS nhưng không xác thực certificate.

Payload publish:

```json
{
  "deviceId": "xiao_esp32s3_01",
  "fall_confidence": 0.95,
  "scream_confidence": 0.0
}
```

MQTT reconnect được thử lại mỗi 5 giây trong `processMQTT()` để không block UI/inference loop.

## VII. Display Và UI

- Driver: LovyanGFX custom class `LGFX`
- Panel: `Panel_GC9A01`
- Độ phân giải: `240 x 240`
- SPI host: `SPI2_HOST`
- SPI write frequency: `40 MHz`
- LVGL draw buffer: `screenWidth * screenHeight / 10`
- Brightness mặc định: `128`

Chân display trong `config.h`:

| Tín hiệu | Chân |
| --- | --- |
| SCK | `D8` |
| MOSI | `D10` |
| MISO | `-1` |
| CS | `D7` |
| DC | `D3` |
| RST | `-1` |

Trạng thái UI:

| Điều kiện | Text |
| --- | --- |
| Fall < 0.80 và Scream < 0.80 | `SAFE` |
| Fall >= 0.80 | `FALL !!!` |
| Scream >= 0.80 | `SCREAM !!!` |
| Fall >= 0.80 và Scream >= 0.80 | `FALL AND SCREAM !!!` |

## VIII. Luồng Hoạt Động

### Setup

1. Khởi tạo Serial 115200.
2. Khởi tạo display và LVGL.
3. Gọi `setupNetwork()`.
4. Nếu network thất bại, vào BLE provisioning và giữ firmware trong vòng lặp provisioning.
5. Cập nhật giờ/ngày từ NTP nếu đồng bộ thành công.
6. Cấp phát `audioBuffer` và `imuBuffer`, ưu tiên PSRAM.
7. Khởi tạo I2S microphone và IMU.
8. Hiệu chỉnh gyro nếu cold boot; nếu wake từ deep sleep thì dùng RTC-retained gyro bias.
9. Tạo `IMU_Task` và `Mic_Task`.
10. Warm-up IMU ring buffer 4 giây.
11. Đặt UI về `SAFE`.

### Loop

1. Gọi `lv_timer_handler()`.
2. Gọi `processMQTT()`.
3. Cập nhật time UI mỗi 1 giây.
4. Đọc mẫu IMU mới nhất, tính `abs(totalG - 1.0)`.
5. Nếu delta lớn hơn `MOTION_THRESHOLD = 0.15f`, cập nhật `lastMotionTime`.
6. Nếu không có chuyển động qua `SLEEP_TIMEOUT = 20000 ms`, gọi `goToDeepSleep()`.
7. Khi `audioDataReady == true`, chạy `run_ai_inference()` và reset flag.
8. Delay loop 10 ms.

## IX. Deep Sleep

Deep sleep được kích hoạt sau 20 giây không có chuyển động theo logic trong `main.cpp`.

Trước khi sleep, firmware:

- Suspend `IMU_Task` và `Mic_Task`.
- Cấu hình motion interrupt trên LSM6DS3 qua ghi thanh ghi I2C.
- Cấu hình `IMU_INT1_PIN` làm wake source bằng `esp_sleep_enable_ext0_wakeup(..., 1)`.
- Tắt/sleep display.
- Uninstall I2S driver.
- Gọi `esp_deep_sleep_start()`.

Gyro bias và flag `isCalibrated` được khai báo `RTC_DATA_ATTR`, nên có thể được giữ lại sau deep sleep.

## X. Build, Upload, Monitor

Chạy các lệnh từ thư mục `firmware/`:

```bash
platformio run -e seeed_xiao_esp32s3
```

```bash
platformio run --target upload -e seeed_xiao_esp32s3
```

```bash
platformio device monitor -e seeed_xiao_esp32s3
```

## XI. Dependencies Và Build Flags

Thư viện trong `platformio.ini`:

```ini
lib_deps =
    SparkFun LSM6DS3 Breakout@^1.0.0
    knolleary/PubSubClient@^2.8
    bblanchon/ArduinoJson@^6.21.3
    lovyan03/LovyanGFX@^1.1.12
    lvgl/lvgl@^8.3.11
```

Các điểm build quan trọng:

- `lib_archive = no`
- `lib_ldf_mode = deep+`
- Include Edge Impulse: `lib/deploy`, `lib/deploy/model-parameters`, `lib/deploy/tflite-model`
- PSRAM: `BOARD_HAS_PSRAM`, `board_build.arduino.memory_type = qio_opi`
- Edge Impulse: `EI_PORTING_ARDUINO=0`, `EI_PORTING_ESPRESSIF=1`
- LVGL: `LV_CONF_SKIP`, `LV_COLOR_DEPTH=16`, `LV_COLOR_16_SWAP=1`, font Montserrat 20 và 48
- Partition: `huge_app.csv`
- `build_src_filter` biên dịch thêm `../lib/deploy/` và loại `../lib/deploy/source/`

## XII. Ghi Chú Bảo Mật

- MQTT password hiện đang nằm trong `include/config.h`. Nếu repo được public hoặc share, nên đưa secret ra file local không commit hoặc cơ chế provisioning riêng.
- MQTT TLS hiện dùng `setInsecure()`. Nếu cần triển khai thật, nên pin CA certificate hoặc dùng certificate validation đầy đủ.
