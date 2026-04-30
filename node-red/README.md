# 🔴 Node-RED Flow Configuration

## Tổng Quan

Node-RED flow này xử lý dữ liệu từ ESP32 wearable device qua MQTT, lưu trữ vào Firestore, và kiểm soát các điều kiện cảnh báo.

## Architecture

```
MQTT Broker (HiveMQ)
        ↓
   Node-RED Server
        ↓
   ┌────────────────────┐
   │   Parse JSON       │
   │   Validate Data    │
   └────┬───────────────┘
        │
   ┌────┴─────────────────────┐
   │                          │
   ▼                          ▼
Firestore DB            Mobile App Alert
(Users/Devices)         (via Firebase)
```

## Flow Components

### 1. MQTT Input Node
- **Broker:** HiveMQ Cloud
- **Topic:** `healthcare/alert`
- **QoS:** 1 (At least once)
- **Payload:** JSON format
```json
{
  "deviceId": "xiao_esp32s3_01",
  "fall_confidence": 0.95,
  "scream_confidence": 0.0,
  "timestamp": 1704067200000
}
```

### 2. JSON Parser
- Xác thực cấu trúc dữ liệu
- Kiểm tra ngưỡng confidence (≥0.80)
- Trích xuất deviceId

### 3. Firestore Update Node
- **Collection:** `devices/{deviceId}`
- **Update fields:**
  - `alert`: true/false
  - `alert_type`: "fall_detection" | "scream_detection" | "both"
  - `last_alert_time`: Timestamp
  - `fall_confidence`: Float (0-1)
  - `scream_confidence`: Float (0-1)

### 4. Function Nodes
- **Alert Logger:** Ghi log cảnh báo
- **Notification Trigger:** Gửi thông báo tới Firebase
- **Data Processor:** Tính toán thống kê

## Cài Đặt Node-RED

### 1. Cài đặt Node-RED
```bash
npm install -g node-red
```

### 2. Cài đặt packages
```bash
node-red-admin install node-red-contrib-firebase-realtime
node-red-admin install node-red-contrib-mqtt-broker
```

### 3. Import flows.json
- Mở Node-RED dashboard (http://localhost:1880)
- Menu → Import → Paste flows.json
- Deploy

## Firestore Integration

### Credentials
1. Tạo Service Account từ Firebase Console
2. Download JSON key file
3. Thêm vào environment variables:
```bash
export GOOGLE_APPLICATION_CREDENTIALS="/path/to/serviceAccountKey.json"
```

### Database Structure
```
firestore/
├── users/{phoneNumber}
│   ├── Name: string
│   ├── DeviceId: array
│   └── password: string
│
└── devices/{deviceId}
    ├── name: string
    ├── is_online: boolean
    ├── alert: boolean
    ├── alert_type: string
    ├── fall_confidence: float
    ├── scream_confidence: float
    └── last_alert_time: timestamp
```

## Monitoring

### Debug Mode
```bash
node-red --userDir ~/.node-red --verbose
```

### Logs
- Xem real-time logs trong Node-RED dashboard
- File logs: `~/.node-red/logs/`

## Troubleshooting

| Vấn đề | Giải pháp |
|--------|----------|
| MQTT không kết nối | Kiểm tra broker URL, port, credentials |
| Firestore update thất bại | Kiểm tra Service Account permissions |
| Data không xuất hiện | Xem debug tab trong Node-RED |
| Cảnh báo bị trễ | Kiểm tra MQTT QoS, Firestore latency |

---

**Phục vụ cho khóa luận: Hệ thống Đeo tay Chăm sóc Sức khỏe**
