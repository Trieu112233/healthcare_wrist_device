# Hệ Thống Đeo Tay Chăm Sóc Sức Khỏe

## Tổng Quan

Dự án xây dựng hệ thống thiết bị đeo tay dùng **ESP32-S3** để phát hiện ngã và tiếng hét/kêu cứu bằng TinyML, gửi cảnh báo qua MQTT, xử lý bằng Node-RED, lưu trạng thái vào Firestore và hiển thị cảnh báo khẩn cấp trên ứng dụng Android.

```text
┌────────────────────────────────────────────────────────────────────┐
│              HỆ THỐNG ĐEO TAY CHĂM SÓC SỨC KHỎE                   │
└────────────────────────────────────────────────────────────────────┘

┌──────────────────┐      MQTT/TLS       ┌──────────────┐
│ XIAO ESP32-S3    │ ──────────────────> │ HiveMQ Cloud │
│ IMU + Mic + AI   │ topic wearable/...  │ MQTT Broker  │
└──────────────────┘                     └──────┬───────┘
                                                │
                                                │ subscribe
                                                v
                                         ┌──────────────┐
                                         │ Node-RED     │
                                         │ filter alert │
                                         └──────┬───────┘
                                                │ update
                                                v
                                         ┌──────────────┐
                                         │ Firestore    │
                                         │ devices/{id} │
                                         └──────┬───────┘
                                                │ realtime listener
                                                v
                                         ┌──────────────┐
                                         │ Android App  │
                                         │ Emergency UI │
                                         └──────────────┘
```

## Cấu Trúc Thư Mục

```text
.
|-- firmware/                  # Firmware PlatformIO cho Seeed XIAO ESP32-S3
|-- node-red/                  # Flow Node-RED nhận MQTT và update Firestore
|-- database/                  # Tài liệu schema Firestore
|-- mobile_app/smartWatchApp/  # Ứng dụng Android Java
`-- model_training/            # Tài liệu/dữ liệu huấn luyện Fall và Scream Detection
```

## Thành Phần Chính

### 1. Firmware ESP32-S3

Firmware chạy trên **Seeed Studio XIAO ESP32-S3** bằng Arduino + PlatformIO.

Chức năng chính:

- Đọc IMU **LSM6DS3** ở 50 Hz, cửa sổ 4 giây gồm 200 mẫu x 6 trục.
- Thu audio từ microphone **INMP441** ở 16 kHz, cửa sổ 1 giây.
- Chạy 2 mô hình Edge Impulse đã hợp nhất bằng multi-impulse deployment:
  - Fall Detection: labels `fall`, `normal`.
  - Scream Detection: labels `non-scream`, `scream`.
- Hiển thị trạng thái trên màn hình tròn **GC9A01 240x240** bằng LVGL + LovyanGFX.
- Cấu hình WiFi qua BLE provisioning nếu chưa có credentials.
- Publish cảnh báo lên HiveMQ khi vượt ngưỡng.
- Vào deep sleep sau 20 giây không có chuyển động và wake bằng interrupt từ IMU.

Ngưỡng cảnh báo hiện tại:

| Loại | Ngưỡng |
| --- | --- |
| Fall | `fall_confidence >= 0.80` |
| Scream | `scream_confidence >= 0.50` |

MQTT payload firmware gửi:

```json
{
  "deviceId": "xiao_esp32s3_01",
  "fall_confidence": 0.95,
  "scream_confidence": 0.0
}
```

Thư mục mã nguồn: `firmware/src/`

Tài liệu chi tiết: `firmware/README.md`

### 2. HiveMQ Cloud

HiveMQ Cloud đóng vai trò MQTT broker giữa firmware và Node-RED.

Cấu hình đang dùng:

| Mục | Giá trị |
| --- | --- |
| Host | `08c8ad4b15ac4370b81835f72e145e5a.s1.eu.hivemq.cloud` |
| Port | `8883` |
| TLS | Bật |
| Firmware publish topic | `wearable/xiao_esp32s3_01/alerts` |
| Node-RED subscribe topic | `wearable/+/alerts` |

### 3. Node-RED

Node-RED nhận alert từ MQTT, lọc dữ liệu và update Firestore.

Luồng chính:

```text
MQTT In -> Debug raw payload -> startTime
        -> Lọc & định dạng dữ liệu alert
        -> Firestore update devices/{deviceId}
        -> endTime -> Tính timediff -> Debug
```

Function node hiện xử lý:

- Bỏ qua message rỗng hoặc thiếu `deviceId`.
- Bỏ qua nếu `fall_confidence < 0.8` và `scream_confidence < 0.5`.
- Xác định `alert_type`:

| Điều kiện | `alert_type` |
| --- | --- |
| Fall >= 0.8 và Scream >= 0.5 | `NGA_VA_HET` |
| Fall >= 0.8 | `PHAT_HIEN_NGA` |
| Scream >= 0.5 | `PHAT_HIEN_TIENG_HET` |

- Chặn lặp riêng cảnh báo `PHAT_HIEN_TIENG_HET` trong 30 giây theo từng thiết bị.
- Update `devices/{deviceId}` với `alert = true`, `alert_type` và `last_alert_time`.

File flow: `node-red/flows.json`

Tài liệu chi tiết: `node-red/README.md`

### 4. Firestore Database

Firestore là lớp dữ liệu realtime cho Android app.

Schema chính:

```text
users/{phoneNumber}
|-- Name: string
|-- password: string
`-- DeviceId: array<string>

devices/{deviceId}
|-- name: string
|-- alert: boolean
|-- alert_type: string
`-- last_alert_time: timestamp | null
```

Node-RED update:

```json
{
  "alert": true,
  "alert_type": "PHAT_HIEN_NGA",
  "last_alert_time": "_serverTimestamp"
}
```

Android app reset cảnh báo bằng cách set:

```json
{
  "alert": false
}
```

Tài liệu chi tiết: `database/README.md`

### 5. Ứng Dụng Android

Ứng dụng Android nằm trong `mobile_app/smartWatchApp`, viết bằng Java.

Chức năng chính:

- Đăng ký và đăng nhập bằng document `users/{phoneNumber}` trong Firestore.
- Quản lý danh sách thiết bị qua mảng `users/{phoneNumber}.DeviceId`.
- Lắng nghe realtime các document `devices/{deviceId}`.
- Chạy foreground service để nhận cảnh báo khi app ở nền.
- Mở full-screen emergency notification/activity khi `alert == true`.
- Cho người dùng xác nhận cảnh báo và reset `devices/{deviceId}.alert = false`.

Các class chính:

| Class | Vai trò |
| --- | --- |
| `LoginActivity` | Đăng nhập và tạo session. |
| `RegisterActivity` | Tạo user mới. |
| `MainActivity` | Hiển thị user, danh sách thiết bị, listener realtime. |
| `FirestoreAlertService` | Foreground service giám sát cảnh báo nền. |
| `EmergencyActivity` | Màn hình cảnh báo khẩn cấp, âm báo, rung và xác nhận. |
| `DeviceAdapter` | Hiển thị danh sách thiết bị. |

Tài liệu chi tiết: `mobile_app/README.md`

### 6. Model Training

Hai mô hình TinyML được huấn luyện độc lập trên Edge Impulse và hợp nhất để chạy trong cùng firmware.

| Mô hình | Dữ liệu | Input | Kết quả test độc lập |
| --- | --- | --- | --- |
| Fall Detection | WEDA-FALL IMU 6 trục | 4 giây, 50 Hz | Accuracy 85,96%, F1-score 86% |
| Scream Detection | Audio scream/non-scream | 1 giây, 16 kHz, MFCC | Accuracy 89,23%, F1-score 90% |

Triển khai:

- Xuất model Edge Impulse dạng C++ library, int8, EON Compiler.
- Hợp nhất bằng `edgeimpulse/multi-impulse-deployment-block`.
- Đưa thư viện kết quả vào `firmware/lib/deploy/`.

Tài liệu chi tiết:

- `model_training/README.md`
- `model_training/fall_detection/README.md`
- `model_training/scream_detection/README.md`

## Luồng Dữ Liệu Cảnh Báo

```text
1. ESP32-S3 đọc IMU/audio và chạy AI tại thiết bị.
2. Nếu fall >= 0.80 hoặc scream >= 0.50, firmware publish MQTT.
3. HiveMQ Cloud chuyển message đến Node-RED qua topic wearable/+/alerts.
4. Node-RED lọc message, xác định alert_type và update devices/{deviceId}.
5. Firestore kích hoạt realtime listener trong Android app.
6. App mở cảnh báo khẩn cấp khi devices/{deviceId}.alert == true.
7. Người dùng xác nhận cảnh báo, app set alert = false.
```

## Build Và Chạy

### Firmware

Chạy trong thư mục `firmware/`:

```bash
platformio run -e seeed_xiao_esp32s3
platformio run --target upload -e seeed_xiao_esp32s3
platformio device monitor -e seeed_xiao_esp32s3
```

### Node-RED

```bash
npm install -g node-red
node-red-admin install node-red-contrib-cloud-firestore
node-red
```

Sau đó mở `http://localhost:1880`, import `node-red/flows.json`, cấu hình Firebase Admin credentials cho node `fall_scream_detection` và deploy.

### Android

Mở project `mobile_app/smartWatchApp` bằng Android Studio, cấu hình Firebase qua `google-services.json`, sau đó build/run app trên thiết bị Android.

## Ghi Chú

Tài liệu này là README tổng quan của toàn bộ đồ án. Các chi tiết triển khai, schema và thông số kỹ thuật nằm trong README của từng thư mục con.
