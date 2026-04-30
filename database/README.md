# 🗄️ Firestore Database Schema

## Database Structure

### Collections Overview

```
firestore/
├── users/
│   └── {phoneNumber}
│       ├── Name: string
│       ├── password: string
│       ├── DeviceId: array<string>
│       ├── createdAt: timestamp
│       └── lastLogin: timestamp
│
├── devices/
│   └── {deviceId}
│       ├── name: string
│       ├── is_online: boolean
│       ├── alert: boolean
│       ├── alert_type: string
│       ├── fall_confidence: float
│       ├── scream_confidence: float
│       ├── last_alert_time: timestamp
│       └── lastSyncTime: timestamp
│
└── alerts/ (Optional - Historical data)
    └── {alertId}
        ├── deviceId: string
        ├── userId: string
        ├── type: string
        ├── confidence: float
        ├── timestamp: timestamp
        └── resolved: boolean
```

---

## Detailed Collection Schemas

### 1. Users Collection

**Document ID:** Phone number (e.g., "0987654321")

```json
{
  "Name": "Nguyễn Đức Triệu",
  "password": "$2b$10$...",  // bcrypt hashed
  "DeviceId": ["xiao_esp32s3_01", "xiao_esp32s3_02"],
  "createdAt": Timestamp(1704067200),
  "lastLogin": Timestamp(1704153600),
  "email": "user@example.com",  // Optional
  "emergencyContact": "0912345678"  // Optional
}
```

**Field Types:**
- `Name` (string): Tên người dùng
- `password` (string): Password đã hash (bcrypt)
- `DeviceId` (array): Danh sách device IDs
- `createdAt` (timestamp): Ngày tạo tài khoản
- `lastLogin` (timestamp): Lần đăng nhập cuối

---

### 2. Devices Collection

**Document ID:** Device ID (e.g., "xiao_esp32s3_01")

```json
{
  "name": "Wristband - Home",
  "is_online": true,
  "alert": true,
  "alert_type": "fall_detection",
  "fall_confidence": 0.95,
  "scream_confidence": 0.0,
  "last_alert_time": Timestamp(1704153645),
  "lastSyncTime": Timestamp(1704153698),
  "battery_level": 75,  // Optional
  "location": "Hanoi",  // Optional
  "owner": "0987654321"  // Reference to User
}
```

**Field Types:**
- `name` (string): Tên thiết bị
- `is_online` (boolean): Trạng thái kết nối
- `alert` (boolean): Có cảnh báo hiện tại?
- `alert_type` (string): Loại cảnh báo
  - "fall_detection"
  - "scream_detection"  
  - "both"
- `fall_confidence` (float): 0.0 - 1.0
- `scream_confidence` (float): 0.0 - 1.0
- `last_alert_time` (timestamp): Cảnh báo cuối cùng
- `lastSyncTime` (timestamp): Lần sync cuối cùng

---

### 3. Alerts Collection (Optional)

**Document ID:** Auto-generated

```json
{
  "deviceId": "xiao_esp32s3_01",
  "userId": "0987654321",
  "type": "fall",
  "confidence": 0.95,
  "timestamp": Timestamp(1704153645),
  "resolved": false,
  "resolvedAt": null,
  "resolvedBy": null
}
```

---

## Firestore Security Rules

```javascript
rules_version = '2';

service cloud.firestore {
  match /databases/{database}/documents {
    
    // Users collection - Only own data
    match /users/{userId} {
      allow read: if request.auth != null && request.auth.uid == userId;
      allow write: if request.auth != null && request.auth.uid == userId;
    }
    
    // Devices collection - Only devices belonging to user
    match /devices/{deviceId} {
      allow read: if request.auth != null;
      allow write: if request.auth != null;
    }
    
    // Alerts collection - Only own alerts
    match /alerts/{alertId} {
      allow read: if request.auth != null;
      allow write: if false;  // Only backend can write
    }
  }
}
```

---

## CRUD Operations

### Create (Firebase SDK)

#### Create User
```javascript
firebase.firestore().collection('users').doc(phoneNumber).set({
  Name: "User Name",
  password: hashedPassword,
  DeviceId: [],
  createdAt: firebase.firestore.FieldValue.serverTimestamp(),
  lastLogin: firebase.firestore.FieldValue.serverTimestamp()
})
```

#### Create Device
```javascript
firebase.firestore().collection('devices').doc(deviceId).set({
  name: "Device Name",
  is_online: true,
  alert: false,
  alert_type: "",
  fall_confidence: 0,
  scream_confidence: 0,
  last_alert_time: null,
  lastSyncTime: firebase.firestore.FieldValue.serverTimestamp()
})
```

### Read (Real-time Listener)

#### Android (Mobile App)
```java
db.collection("devices").document(deviceId)
  .addSnapshotListener((doc, error) -> {
    if (doc != null && doc.exists()) {
      Device device = doc.toObject(Device.class);
      updateUI(device);
    }
  });
```

#### Web (JavaScript)
```javascript
db.collection("devices").doc(deviceId)
  .onSnapshot(doc => {
    console.log("Device data:", doc.data());
  });
```

### Update

#### Update Device Alert Status
```javascript
firebase.firestore().collection('devices').doc(deviceId).update({
  alert: true,
  alert_type: "fall_detection",
  fall_confidence: 0.95,
  last_alert_time: firebase.firestore.FieldValue.serverTimestamp()
})
```

#### Add Device to User
```javascript
firebase.firestore().collection('users').doc(userId).update({
  DeviceId: firebase.firestore.FieldValue.arrayUnion(deviceId)
})
```

### Delete

#### Remove Device from User
```javascript
firebase.firestore().collection('users').doc(userId).update({
  DeviceId: firebase.firestore.FieldValue.arrayRemove(deviceId)
})
```

---

## Real-time Listener Examples

### Python (Backend)
```python
from firebase_admin import firestore

db = firestore.client()

def on_device_update(doc, changes, read_time):
    for change in changes:
        if change.type.name == 'ADDED':
            print(f"New alert: {change.document.get('alert_type')}")
        elif change.type.name == 'MODIFIED':
            print(f"Device updated: {change.document.get('is_online')}")

# Listen to all devices
db.collection('devices').on_snapshot(on_device_update)
```

### Node-RED (Flow)
```json
[
  {
    "id": "firestore-listener",
    "type": "firebase-realtime-listener",
    "path": "devices/{deviceId}",
    "operation": "snapshot"
  }
]
```

---

## Indexes

### Recommended Indexes for Performance

```
Collection: devices
Fields:
- is_online (Ascending)
- alert (Ascending)
- last_alert_time (Descending)
```

---

## Data Model - Sample Data

### Sample User
```json
{
  "Name": "Nguyễn Đức Triệu",
  "password": "$2b$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcg7b3XeKeUxWdeS86.CHyVKSDm",
  "DeviceId": ["xiao_esp32s3_01"],
  "createdAt": "2024-01-01T12:00:00Z",
  "lastLogin": "2024-01-10T08:30:00Z"
}
```

### Sample Device - Safe State
```json
{
  "name": "My Smart Watch",
  "is_online": true,
  "alert": false,
  "alert_type": "",
  "fall_confidence": 0.15,
  "scream_confidence": 0.05,
  "last_alert_time": null,
  "lastSyncTime": "2024-01-10T08:35:00Z"
}
```

### Sample Device - Alert State
```json
{
  "name": "My Smart Watch",
  "is_online": true,
  "alert": true,
  "alert_type": "fall_detection",
  "fall_confidence": 0.95,
  "scream_confidence": 0.0,
  "last_alert_time": "2024-01-10T08:40:15Z",
  "lastSyncTime": "2024-01-10T08:40:18Z"
}
```

---

## Setup Guide

### 1. Create Firebase Project
1. Go to [Firebase Console](https://console.firebase.google.com)
2. Create new project
3. Enable Firestore Database
4. Start in **Test Mode** (for development)

### 2. Configure Security Rules
```javascript
rules_version = '2';
service cloud.firestore {
  match /databases/{database}/documents {
    match /{document=**} {
      allow read, write: if request.auth != null;
    }
  }
}
```

### 3. Download Service Account Key
1. Project Settings → Service Accounts
2. Generate new private key
3. Save as `serviceAccountKey.json`

### 4. Initialize Firestore (Python)
```python
import firebase_admin
from firebase_admin import credentials, firestore

cred = credentials.Certificate('serviceAccountKey.json')
firebase_admin.initialize_app(cred)
db = firestore.client()
```

---

## Performance Tips

✅ Use field indexes for frequently queried fields  
✅ Batch writes for multiple updates  
✅ Use document snapshots instead of multiple reads  
✅ Enable offline persistence (mobile)  
✅ Use pagination for large result sets  

---

## References

- [Firestore Documentation](https://firebase.google.com/docs/firestore)
- [Security Rules Guide](https://firebase.google.com/docs/firestore/security/start)
- [Real-time Listeners](https://firebase.google.com/docs/firestore/query-data/listen)

---

**Phục vụ cho khóa luận tốt nghiệp: Hệ thống Đeo tay Chăm sóc Sức khỏe**
