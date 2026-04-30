# 🏗️ System Architecture 

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
      │ (Fall + Scream                                    │
      │  Detection Alert)                  ┌──────────────┴
      │                                     │                        
      │                            ┌────────▼────────┐     
      │                            │  Firestore DB   │      
      │                            │  (Real-time)    │      
      │                            └─────────┬───────┘      
      │                                      │
      │                            ┌─────────┴
      │                            │                    
      │                    ┌───────▼──────┐    
      │                    │ Mobile App   │    
      │                    │ (Android)    │    
      │                    └──────────────┘    
```

## Components Chi Tiết

### 1. **Embedded System (ESP32-S3 Wearable)**

**Chức năng:**
- Đọc dữ liệu từ IMU (LSM6DS3) ở 50Hz
- Thu âm từ microphone (INMP441) ở 16kHz
- Chạy 2 mô hình ML (Fall + Scream Detection)
- Hiển thị UI trên màn hình tròn GC9A01
- Kết nối WiFi & gửi cảnh báo qua MQTT

**Output MQTT:**
```json
{
  "deviceId": "xiao_esp32s3_01",
  "fall_confidence": 0.95,
  "scream_confidence": 0.0
}
```

**Files:** `multi_model/src/`

---

### 2. **IoT Broker (HiveMQ Cloud)**

**Chức năng:**
- Nhận MQTT messages từ wearable device
- Định tuyến tới Node-RED
- Bảo mật TLS/SSL

**Configuration:**
- Port: 8883 (TLS)
- Topic: `healthcare/alert`

---

### 3. **Data Processing (Node-RED)**

**Chức năng:**
- Parse JSON từ MQTT
- Validate confidence threshold (≥ 0.80)
- Update Firestore devices collection

**Flow:**
```
MQTT Input → Parse JSON → Validate → Firestore Update
```

**File:** `node-red/flows.json`

---

### 4. **Database (Firestore)**

**Collections thực tế:**

#### users
```json
{
  "_id": "0122345678",
  "Name": "Nguyen Duc Trieu",
  "password": "abc123",
  "DeviceId": ["xiao_esp32s3_01"]
}
```

#### devices
```json
{
  "_id": "xiao_esp32s3_01",
  "name": "user1",
  "is_online": true,
  "alert": false,
  "alert_type": "TIENG_HET_LON",
  "last_alert_time": Timestamp,
  "phone": ["0122345678"]
}
```

---

### 5. **Mobile Application (Android)**

**Chức năng:**
- Đăng nhập người dùng
- Quản lý danh sách thiết bị
- Real-time monitoring
- Nhận cảnh báo khẩn cấp

**Tech Stack:**
- Android Studio
- Firebase Authentication
- Firestore Realtime Database

**Files:** `mobile_app/smartWatchApp/`

---

## Data Flow

### Alert Flow
```
1. WEARABLE (ESP32-S3)
   └─ ML Inference: confidence ≥ 0.80?
   └─ YES → Publish MQTT

2. HIVEMQ BROKER
   └─ Route to Node-RED

3. NODE-RED
   └─ Parse & Validate
   └─ Update Firestore

4. FIRESTORE
   └─ Update devices/{deviceId}
   └─ Trigger Realtime Listeners

5. MOBILE APP
   └─ Show alert screen
```

---

**Phục vụ cho khóa luận tốt nghiệp
