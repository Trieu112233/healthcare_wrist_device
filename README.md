# Kiến trúc hệ thống

## Tổng quan

```
┌─────────────────────────────────────────────────────────────────┐
│          HỆ THỐNG VÒNG ĐEO TAY CHĂM SÓC SỨC KHỎE                │
└─────────────────────────────────────────────────────────────────┘

┌──────────────┐         ┌──────────────┐         ┌──────────────┐
│   ESP32-S3   │ ──WiFi──│  HiveMQ IoT  │ ──HTTP──│  Node-RED    │
│ Thiết bị đeo │  MQTT   │   Broker     │  Flow   │   Server     │
└──────────────┘         └──────────────┘         └──────────────┘
      │                                                    │
      │ (Cảnh báo phát hiện                               │
      │  ngã + tiếng hét)                  ┌──────────────┴
      │                                     │
      │                            ┌────────▼────────┐
      │                            │  Firestore DB   │
      │                            │  (thời gian thực)│
      │                            └─────────┬───────┘
      │                                      │
      │                            ┌─────────┴
      │                            │
      │                    ┌───────▼──────┐
      │                    │ Ứng dụng     │
      │                    │ Android      │
      │                    └──────────────┘
```

## Các thành phần chính

### 1. Hệ thống nhúng ESP32-S3

**Chức năng:**

- Đọc dữ liệu từ cảm biến IMU LSM6DS3 ở tần số 50 Hz.
- Thu âm từ microphone INMP441 ở tần số 16 kHz.
- Chạy 2 mô hình học máy: phát hiện ngã và phát hiện tiếng hét.
- Hiển thị giao diện trên màn hình tròn GC9A01.
- Kết nối WiFi và gửi cảnh báo qua MQTT.

**Dữ liệu MQTT gửi đi:**

```json
{
  "deviceId": "xiao_esp32s3_01",
  "fall_confidence": 0.95,
  "scream_confidence": 0.0
}
```

**Thư mục mã nguồn:** `multi_model/src/`

---

### 2. Broker IoT HiveMQ Cloud

**Chức năng:**

- Nhận bản tin MQTT từ thiết bị đeo.
- Chuyển tiếp dữ liệu đến Node-RED.
- Bảo mật kết nối bằng TLS/SSL.

**Cấu hình:**

- Port: 8883 (TLS).
- Topic: `healthcare/alert`.

---

### 3. Xử lý dữ liệu bằng Node-RED

**Chức năng:**

- Phân tích JSON nhận từ MQTT.
- Kiểm tra ngưỡng tin cậy của cảnh báo (`confidence >= 0.80`).
- Cập nhật collection `devices` trên Firestore.

**Luồng xử lý:**

```
MQTT Input -> Parse JSON -> Validate -> Firestore Update
```

**File cấu hình:** `node-red/flows.json`

---

### 4. Cơ sở dữ liệu Firestore

**Các collection thực tế:**

#### `users`

```json
{
  "_id": "0122345678",
  "Name": "Nguyen Duc Trieu",
  "password": "abc123",
  "DeviceId": ["xiao_esp32s3_01"]
}
```

#### `devices`

```json
{
  "_id": "xiao_esp32s3_01",
  "name": "user1",
  "is_online": true,
  "alert": false,
  "alert_type": "TIENG_HET_LON",
  "last_alert_time": "Timestamp",
  "phone": ["0122345678"]
}
```

---

### 5. Ứng dụng di động Android

**Chức năng:**

- Đăng nhập người dùng.
- Quản lý danh sách thiết bị được liên kết.
- Theo dõi trạng thái thiết bị theo thời gian thực.
- Nhận và hiển thị cảnh báo khẩn cấp.

**Công nghệ sử dụng:**

- Android Studio.
- Firebase Authentication.
- Firestore Realtime Database.

**Thư mục mã nguồn:** `mobile_app/smartWatchApp/`

---

## Luồng dữ liệu cảnh báo

```
1. THIẾT BỊ ĐEO ESP32-S3
   └─ Chạy suy luận AI: confidence >= 0.80?
   └─ Có -> Gửi bản tin MQTT

2. HIVEMQ BROKER
   └─ Chuyển tiếp dữ liệu đến Node-RED

3. NODE-RED
   └─ Phân tích và kiểm tra dữ liệu
   └─ Cập nhật Firestore

4. FIRESTORE
   └─ Cập nhật devices/{deviceId}
   └─ Kích hoạt realtime listeners

5. ỨNG DỤNG ANDROID
   └─ Hiển thị màn hình cảnh báo
```

---

Tài liệu phục vụ cho đồ án tốt nghiệp.
