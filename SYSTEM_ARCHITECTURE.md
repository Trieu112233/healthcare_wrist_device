# 🏗️ System Architecture - Hệ thống Đeo tay Chăm sóc Sức khỏe

## Kiến Trúc Tổng Thể

```
┌─────────────────────────────────────────────────────────────────┐
│                    HEALTHCARE WRISTBAND SYSTEM                  │
└─────────────────────────────────────────────────────────────────┘

┌──────────────┐         ┌──────────────┐         ┌──────────────┐
│   ESP32-S3   │ ──WiFi──│  HiveMQ IoT  │ ──HTTP──│  Node-RED    │
│   Wearable   │  MQTT   │   Broker     │ Flows   │   Server     │
└──────────────┘         └──────────────┘         └──────────────┘
      │                                                    │
      │ (Fall Detection                                   │
      │  + Scream Alert)                    ┌──────────────┴──────────┐
      │                                     │                         │
      │                            ┌────────▼────────┐      ┌────────▼─────┐
      │                            │  Firestore DB   │      │ Firebase Auth│
      │                            │  (Real-time)    │      │ (Backend)    │
      │                            └─────────┬───────┘      └──────────────┘
      │                                      │
      │                            ┌─────────┴──────────┐
      │                            │                    │
      │                    ┌───────▼──────┐    ┌────────▼──────┐
      │                    │ Mobile App   │    │ Desktop Web   │
      │                    │ (Android)    │    │ (Dashboard)   │
      │                    └──────────────┘    └───────────────┘
      │
      └──(Optional)──> ┌──────────────┐
                       │ Local Server │
                       │ (Backup)     │
                       └──────────────┘
```

## Components Chi Tiết

### 1. **Embedded System (ESP32-S3 Wearable)**

**Chức năng:**
- Đọc dữ liệu từ IMU (LSM6DS3) ở 50Hz
- Thu âm từ microphone (INMP441) ở 16kHz
- Chạy 2 mô hình ML (Fall + Scream Detection)
- Hiển thị UI trên màn hình tròn GC9A01
- Kết nối WiFi & gửi cảnh báo qua MQTT

**Files:**
- `multi_model/src/main.cpp` - Entry point
- `multi_model/src/sensors.cpp` - IMU & Mic handling
- `multi_model/src/ai_inference.cpp` - ML inference
- `multi_model/src/network_app.cpp` - WiFi + MQTT
- `multi_model/src/display_app.cpp` - LVGL UI

**Output:** JSON alert qua MQTT
```json
{
  "deviceId": "xiao_esp32s3_01",
  "fall_confidence": 0.95,
  "scream_confidence": 0.0
}
```

---

### 2. **IoT Broker (HiveMQ Cloud)**

**Chức năng:**
- Nhận MQTT messages từ wearable device
- Định tuyến tới Node-RED
- Bảo mật TLS/SSL

**Configuration:**
- Server: `your-hivemq.cloud.hivemq.com`
- Port: 8883 (TLS)
- Topic: `healthcare/alert`

---

### 3. **Data Processing (Node-RED)**

**Chức năng:**
- Parse JSON từ MQTT
- Validate confidence threshold
- Update Firestore database
- Trigger notifications

**Flow:**
```
MQTT Input
    ↓
Parse JSON
    ↓
Validate (confidence ≥ 0.80)
    ↓
Firestore Update
    ↓
Firebase Notification
    ↓
Mobile App Alert
```

**Files:** `node-red/flows.json`

---

### 4. **Database (Firestore)**

**Collections:**

#### users
```json
{
  "_id": "0987654321",
  "Name": "Nguyễn Đức Triệu",
  "password": "hashed",
  "DeviceId": ["xiao_esp32s3_01"]
}
```

#### devices
```json
{
  "_id": "xiao_esp32s3_01",
  "name": "My Watch",
  "is_online": true,
  "alert": true,
  "alert_type": "fall_detection",
  "fall_confidence": 0.95,
  "last_alert_time": Timestamp
}
```

#### alerts (Optional)
```json
{
  "_id": "auto_generated",
  "deviceId": "xiao_esp32s3_01",
  "userId": "0987654321",
  "type": "fall",
  "confidence": 0.95,
  "timestamp": Timestamp,
  "resolved": false
}
```

**Real-time:** Mobile app listen tới `devices/{deviceId}` → instant update

---

### 5. **Mobile Application (Android)**

**Chức năng:**
- Đăng nhập người dùng
- Quản lý danh sách thiết bị
- Real-time monitoring status
- Nhận cảnh báo khẩn cấp

**Features:**
- Login screen (phone + password)
- Device list (online/offline status)
- Alert popup khi có cảnh báo
- History & statistics

**Tech Stack:**
- Android Studio
- Firebase Authentication
- Firestore Realtime Database
- LVGL UI rendering

**Files:** `mobile_app/smartWatchApp/`

---

## Data Flow

### Luồng Cảnh Báo (Alert Flow)

```
1. WEARABLE DEVICE
   └─ IMU + Audio Data
   └─ ML Inference (Fall/Scream)
   └─ Confidence ≥ 0.80?
      └─ YES → Publish MQTT

2. HIVEMQ BROKER
   └─ Receive MQTT
   └─ Route to Node-RED

3. NODE-RED
   └─ Parse JSON
   └─ Validate data
   └─ Update Firestore

4. FIRESTORE
   └─ Update devices/{deviceId}
   └─ Set alert=true
   └─ Trigger Realtime Listeners

5. MOBILE APP
   └─ Receive update
   └─ Show emergency screen
   └─ Play sound + vibrate
   └─ Send user notification
```

### Luồng Dữ Liệu (Data Flow)

```
Wearable (50Hz IMU + 16kHz Audio)
    ↓
Ring Buffers (4s IMU + 1s Audio)
    ↓
ML Models (Edge Impulse)
    ↓
MQTT/HiveMQ
    ↓
Node-RED (Parse + Validate)
    ↓
Firestore (Real-time Update)
    ↓
Mobile App (Firebase Realtime Listener)
    ↓
User Interface (Alert/History)
```

---

## Deployment Stack

| Component | Technology | Hosting |
|-----------|-----------|---------|
| **Wearable** | ESP32-S3 + Arduino | Local Device |
| **Broker** | HiveMQ Cloud MQTT | Cloud (AWS) |
| **Middleware** | Node-RED | VPS / Cloud Server |
| **Database** | Firebase Firestore | Google Cloud |
| **Mobile App** | Android + Firebase | Play Store / APK |
| **Backend Auth** | Firebase Auth | Google Cloud |

---

## Security Measures

✅ **MQTT:** TLS/SSL encryption  
✅ **Firestore:** Security Rules (user authentication)  
✅ **WiFi:** WPA2 encryption  
✅ **API:** Firebase token validation  
✅ **Data:** Password hashing (bcrypt)  

---

## Performance Metrics

| Metric | Value |
|--------|-------|
| **ML Inference Latency** | ~150ms |
| **MQTT Publish Latency** | ~100ms |
| **Firestore Update** | ~500ms |
| **Mobile App Response** | ~1s (Realtime Listener) |
| **Total Alert Delay** | ~2s end-to-end |

---

## Scaling Strategy

### Hiện tại
- 1 wearable device
- 1 user account
- Real-time processing

### Tương lai
- Multiple users → User management system
- Multiple devices per user → Device pooling
- Data analytics → Time-series database (InfluxDB)
- Machine learning → Model improvement pipeline
- Web dashboard → Real-time analytics

---

## Development Setup

### Prerequisites
```bash
# Node.js & npm
node --version  # v16+
npm --version   # v8+

# Arduino IDE / PlatformIO
platformio --version

# Firebase CLI
firebase --version
```

### Installation
```bash
# Clone repository
git clone https://github.com/Trieu112233/healthcare_wrist_device.git

# Install dependencies
cd healthcare_wrist_device/multi_model
platformio install

# Configure
cp include/config.h.template include/config.h
# Edit WIFI_SSID, WIFI_PASS, MQTT_* credentials
```

---

## Troubleshooting Guide

| Issue | Solution |
|-------|----------|
| ESP32 không kết nối WiFi | Kiểm tra SSID/password, signal strength |
| MQTT publish thất bại | Verify HiveMQ credentials, broker URL |
| Firestore update slow | Check network latency, database indexes |
| Mobile app không update | Verify Firebase config, user authentication |
| Cảnh báo bị mất | Check MQTT QoS, Node-RED logs |

---

## References

- [Edge Impulse Documentation](https://docs.edgeimpulse.com)
- [Firebase Documentation](https://firebase.google.com/docs)
- [Node-RED Documentation](https://nodered.org/docs)
- [HiveMQ Cloud](https://www.hivemq.com/cloud)

---

**Phục vụ cho khóa luận tốt nghiệp: Hệ thống Đeo tay Chăm sóc Sức khỏe**

*Nguyễn Đức Triệu - VNU University of Engineering and Technology*
