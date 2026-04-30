# 📱 Chi Tiết Mobile App - Healthcare Wrist Device

Dưới đây là mô tả chi tiết về ứng dụng này:

---

## 🎯 Tổng Quan

**Mobile App Name:** SmartWatchApp  
**Package:** com.example.smartwatchapp  
**Platform:** Android  
**Min SDK:** 26 (Android 8.0)  
**Target SDK:** 36 (Android 15)  
**Build System:** Gradle (Kotlin DSL)  
**Backend:** Firebase (Firestore + Authentication)

---

## 🏗️ Kiến Trúc Ứng Dụng

```
smartWatchApp/
├── app/
│   ├── src/main/
│   │   ├── java/com/example/smartwatchapp/
│   │   │   ├── LoginActivity.java          (Đăng nhập)
│   │   │   ├── RegisterActivity.java       (Đăng ký)
│   │   │   ├── MainActivity.java           (Màn hình chính)
│   │   │   ├── EmergencyActivity.java      (Cảnh báo khẩn cấp)
│   │   │   ├── FirestoreAlertService.java  (Foreground Service)
│   │   │   ├── Device.java                 (Data Model)
│   │   │   └── DeviceAdapter.java          (RecyclerView Adapter)
│   │   ├── res/
│   │   │   ├── layout/                     (UI Layouts)
│   │   │   ├── drawable/                   (Icons & Images)
│   │   │   ├── mipmap-*/                   (App Icons)
│   │   │   ├── values/                     (Strings, Colors, Styles)
│   │   │   └── xml/                        (Configurations)
│   │   └── AndroidManifest.xml
│   ├── build.gradle.kts                    (Dependencies)
│   └── google-services.json                (Firebase Config)
└── settings.gradle.kts
```

---

## 📦 Công Nghệ & Dependencies

```gradle
// Android Framework
- androidx.core.ktx
- androidx.appcompat
- androidx.constraintlayout
- androidx.navigation (Fragment & UI)
- androidx.activity
- androidx.lifecycle

// UI Components
- Material Design (com.google.android.material)

// Firebase
- firebase-firestore (Real-time Database)
- firebase-core

// Testing
- JUnit
- Espresso (Instrumented Tests)
```

---

## 🔑 Các Component Chính

### 1️⃣ **LoginActivity.java** - Đăng Nhập
- 📱 Giao diện đăng nhập bằng số điện thoại + mật khẩu
- 🔐 Xác thực người dùng từ Firestore
- 💾 Lưu thông tin đăng nhập vào SharedPreferences
- 🔄 Kiểm tra trạng thái đăng nhập khi khởi động
- ➡️ Chuyển hướng đến MainActivity hoặc RegisterActivity

```java
// Luồng đăng nhập:
1. Kiểm tra SharedPreferences (nếu đã đăng nhập → skip)
2. Nhập số điện thoại + mật khẩu
3. Query Firestore collection "users"
4. Kiểm tra mật khẩu
5. Lưu phone vào SharedPreferences
6. Khởi động FirestoreAlertService
7. Chuyển sang MainActivity
```

### 2️⃣ **MainActivity.java** - Màn Hình Chính ⭐
**Chức năng chính:**
- 📋 Hiển thị danh sách thiết bị được kết nối
- ➕ Thêm thiết bị mới bằng Device ID
- ❌ Xóa thiết bị (nhấn giữ)
- 📡 Real-time monitoring với Firestore Snapshot Listener
- 🔄 Cập nhật trang thái thiết bị (Online/Offline, Alert Status)
- 🚪 Đăng xuất tài khoản

**Key Features:**
```java
// Quản lý Listeners thông minh
- Dùng Map<String, ListenerRegistration> để tránh listener lặp
- Tự động thêm/gỡ listener khi danh sách thiết bị thay đổi
- Cleanup listeners khi Activity destroy

// Real-time Data Sync
- Lắng nghe user document từ Firestore
- Lấy danh sách DeviceId
- Tự động lắng nghe từng device document
- Update UI ngay lập tức khi có thay đổi
```

**Giao diện:**
- RecyclerView hiển thị danh sách thiết bị
- Floating Action Button (FAB) để thêm thiết bị
- Logout button
- Thông tin user (Tên, SĐT)

### 3️⃣ **FirestoreAlertService.java** - Foreground Service 🔔
**Mục đích:** Chạy ngầm để giám sát cảnh báo từ thiết bị

```java
// Chức năng:
1. Khởi động Foreground Service (chạy luôn ngay cả khi app bị đóng)
2. Lắng nghe collection "devices" qua Firestore Snapshot Listener
3. Khi phát hiện alert=true:
   - Trigger EmergencyActivity
   - Truyền device_id, alert_type, last_alert_time
4. Tự động restart nếu bị kill (onTaskRemoved)
5. Tạo Notification Channel để hiển thị thông báo

// Permissions yêu cầu:
- FOREGROUND_SERVICE (Chạy foreground service)
- FOREGROUND_SERVICE_DATA_SYNC (Data sync service)
- POST_NOTIFICATIONS (Gửi thông báo)
```

**Notification:**
```
"Hệ thống cảnh báo đang chạy"
"Đang giám sát thiết bị của bạn..."
```

### 4️⃣ **EmergencyActivity.java** - Cảnh Báo Khẩn Cấp 🚨
**Kích hoạt khi:** Phát hiện alert từ thiết bị

```java
// Giao diện:
- Hiển thị kiểu cảnh báo (Fall_Detection, Scream_Detection, etc.)
- Hiển thị Device ID
- Hiển thị thời gian cảnh báo
- Nút "Acknowledge" để xác nhận

// Hiệu ứng cảnh báo:
- Âm báo động (Alarm ringtone)
- Rung điện thoại (3 giây)
- Sáng màn hình kể cả khi đang khóa (FLAG_SHOW_WHEN_LOCKED)
- Mở khóa tự động nếu có thể

// Xử lý:
1. Người dùng nhấn "Acknowledge"
2. Update Firestore: alert = false
3. Đóng activity
4. Hoặc service tự đóng khi alert được reset từ thiết bị
```

### 5️⃣ **Device.java** - Data Model
```java
class Device {
    String id;              // ID thiết bị (VD: xiao_esp32s3_03)
    String name;            // Tên thiết bị
    boolean is_online;      // Trạng thái kết nối
    boolean alert;          // Có cảnh báo?
    String alert_type;      // Loại cảnh báo (Fall, Scream, etc)
    Object last_alert_time; // Timestamp cảnh báo cuối
}
```

### 6️⃣ **DeviceAdapter.java** - RecyclerView Adapter
**Hiển thị từng device item:**

```java
// Mỗi item hiển thị:
┌─────────────────────────┐
│ Device Name             │
│ ● Online  |  AN TOÀN    │  (or ● Offline | CẢNH BÁO)
└─────────────────────────┘

// Màu sắc:
- Online: Xanh (#00E676)
- Offline: Đỏ (#FF5252)
- Safe: Xanh (#00E676)
- Alert: Đỏ + alert_type (#FF5252)

// Tương tác:
- Long Press: Xóa thiết bị (hiển thị confirm dialog)
```

---

## 📋 Luồng Hoạt Động Chi Tiết

### 1. **Khởi Động App**
```
App Launch
    ↓
LoginActivity.onCreate()
    ├─ Kiểm tra SharedPreferences("AUTH")
    ├─ Nếu đã đăng nhập → MainActivity
    └─ Nếu chưa → Hiển thị form đăng nhập
```

### 2. **Đăng Nhập**
```
User nhập Phone + Password
    ↓
loginUser(phone, password)
    ├─ Query: db.collection("users").document(phone).get()
    ├─ Kiểm tra password match
    ├─ Lưu: SharedPreferences.putString("user_phone", phone)
    ├─ startForegroundService(FirestoreAlertService)
    └─ Intent → MainActivity
```

### 3. **MainActivity - Real-time Sync**
```
MainActivity.onCreate()
    ├─ Lấy userPhone từ SharedPreferences
    ├─ requestAppPermissions() [Notifications, Overlay, Battery]
    ├─ startAlertService()
    ├─ Setup RecyclerView + DeviceAdapter
    └─ loadUserDevices()
            ↓
        addSnapshotListener("users/{userPhone}")
            ├─ Lấy danh sách DeviceId
            └─ manageDeviceListeners(deviceIds)
                    ├─ Gỡ listener của device đã xóa
                    └─ Thêm listener cho device mới
                            ↓
                        listenToDeviceDetails(deviceId)
                            ├─ addSnapshotListener("devices/{deviceId}")
                            └─ Cập nhật UI: updateList(device)
```

### 4. **Thêm Thiết Bị Mới**
```
User nhấn FAB "Thêm thiết bị"
    ↓
showAddDeviceDialog()
    ├─ Input Device ID (VD: xiao_esp32s3_03)
    └─ Nhấn "Thêm"
            ↓
        checkAndAddDevice(deviceId)
            ├─ Query: db.collection("devices").document(deviceId).get()
            ├─ Nếu tồn tại:
            │   └─ addDeviceIdToUser(deviceId)
            │       └─ db.collection("users/{userPhone}")
            │           .update("DeviceId", arrayUnion(deviceId))
            └─ Nếu không: Toast "Mã thiết bị không tồn tại"
                    ↓
            MainActivity Snapshot Listener tự cập nhật
```

### 5. **Cảnh Báo Khẩn Cấp**
```
FirestoreAlertService lắng nghe devices
    ↓
Phát hiện: device.alert = true
    ↓
triggerEmergencyAlert(deviceId, alertType, time)
    ├─ Intent("device_id", "alert_type", "last_alert_time")
    └─ startActivity(EmergencyActivity)
            ↓
        EmergencyActivity.onCreate()
            ├─ Hiển thị Alert Dialog
            ├─ Play Ringtone + Vibrate
            ├─ Sáng màn hình (mở khóa nếu cần)
            └─ Lắng nghe listenToSelfClose()
                    ↓
                Khi alert reset từ thiết bị
                    └─ finish() (đóng EmergencyActivity)
```

---

## 🔐 Permissions Yêu Cầu

```xml
<!-- AndroidManifest.xml -->
<uses-permission android:name="android.permission.FOREGROUND_SERVICE" />
<uses-permission android:name="android.permission.FOREGROUND_SERVICE_DATA_SYNC" />
<uses-permission android:name="android.permission.REQUEST_IGNORE_BATTERY_OPTIMIZATIONS" />
<uses-permission android:name="android.permission.POST_NOTIFICATIONS" />
<uses-permission android:name="android.permission.VIBRATE" />
<uses-permission android:name="android.permission.WAKE_LOCK" />
<uses-permission android:name="android.permission.INTERNET" />
<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />
<uses-permission android:name="android.permission.SYSTEM_ALERT_WINDOW" />
```

---

## 📱 Activities & Services

### Activities:
1. **LoginActivity** - Màn hình đăng nhập (Entry point)
2. **RegisterActivity** - Đăng ký tài khoản (chưa implement đầy đủ)
3. **MainActivity** - Màn hình chính, quản lý thiết bị
4. **EmergencyActivity** - Hiển thị cảnh báo khẩn cấp (showOnLockScreen=true, turnScreenOn=true)

### Services:
1. **FirestoreAlertService** - Foreground service giám sát cảnh báo 24/7

---

## 🎨 UI Resources

```
res/
├── layout/
│   ├── activity_login2.xml          (Login screen)
│   ├── activity_main2.xml           (Main screen with RecyclerView)
│   ├── activity_emergency.xml       (Emergency alert)
│   └── item_device.xml              (Device list item)
├── drawable/                         (Icons)
├── mipmap-{hdpi,mdpi,xhdpi,...}/   (App icons - multiple resolutions)
├── values/
│   ├── strings.xml                  (Text resources - Vietnamese)
│   ├── colors.xml                   (Color definitions)
│   ├── themes.xml                   (App themes)
│   └── dimens.xml                   (Dimensions)
└── xml/
    ├── backup_rules.xml             (Data backup policy)
    └── data_extraction_rules.xml    (GDPR compliance)
```

---

## 🔗 Firebase Integration

### Firestore Collections Structure:

```
Firestore Database
├── users/
│   └── {phone}/
│       ├── Name: "Nguyễn Đức Triệu"
│       ├── password: "hashed_password"
│       └── DeviceId: ["xiao_esp32s3_01", "xiao_esp32s3_02", ...]
│
└── devices/
    └── {deviceId}/
        ├── name: "Device 1"
        ├── is_online: true/false
        ├── alert: true/false
        ├── alert_type: "Fall_Detection" | "Scream_Detection"
        └── last_alert_time: Timestamp
```

---

## ⚙️ Build Configuration

```gradle kts
// app/build.gradle.kts
android {
    namespace = "com.example.smartwatchapp"
    compileSdk = 36
    
    defaultConfig {
        applicationId = "com.example.smartwatchapp"
        minSdk = 26
        targetSdk = 36
        versionCode = 1
        versionName = "1.0"
    }
    
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }
    
    buildFeatures {
        viewBinding = true  // View Binding enabled
    }
}
```

---

## 🎯 Tính Năng Chính

✅ **Authentication** - Đăng nhập bằng phone + password  
✅ **Device Management** - Thêm/xóa thiết bị theo dõi  
✅ **Real-time Monitoring** - Cập nhật trạng thái thiết bị ngay lập tức  
✅ **Emergency Alerts** - Cảnh báo khẩn cấp khi phát hiện sự cố  
✅ **Background Service** - Giám sát 24/7 ngay cả khi app bị đóng  
✅ **Multi-device Support** - Theo dõi nhiều thiết bị cùng lúc  
✅ **Push Notifications** - Thông báo cảnh báo trên lock screen  
✅ **Vietnamese Localization** - Giao diện tiếng Việt  

---

## 🚀 Cách Sử Dụng

1. **Cài đặt:** Compile project bằng Android Studio
2. **Chạy:** Deploy trên emulator hoặc thiết bị thật (Android 8.0+)
3. **Đăng nhập:** Sử dụng phone + password có trong Firestore
4. **Thêm thiết bị:** Nhập Device ID từ wearable device
5. **Giám sát:** Theo dõi trạng thái real-time
6. **Cảnh báo:** Nhận thông báo khẩn cấp khi có sự cố

---

Đây là một ứng dụng **Health Monitoring Companion** đầy đủ với focus vào **Real-time Alert System** cho các thiết bị đeo tay chuyên về phát hiện ngã và cảnh báo khẩn cấp! 🏥📱