# Ứng Dụng Android

## Tổng Quan

`SmartWatchApp` là ứng dụng Android đi kèm hệ thống đeo tay chăm sóc sức khỏe. Ứng dụng dùng Firestore để đăng nhập, quản lý danh sách thiết bị và nhận cảnh báo khẩn cấp theo thời gian thực.

Thông tin chính:

| Mục | Giá trị |
| --- | --- |
| Package | `com.example.smartwatchapp` |
| Project Android Studio | `mobile_app/smartWatchApp` |
| Min SDK | 26 |
| Target SDK | 36 |
| Compile SDK | 36 |
| Ngôn ngữ | Java |
| Công cụ build | Gradle Kotlin DSL |
| Backend | Firebase Firestore |

## Cấu Trúc Project

```text
mobile_app/smartWatchApp/
|-- app/
|   |-- src/main/
|   |   |-- java/com/example/smartwatchapp/
|   |   |   |-- LoginActivity.java
|   |   |   |-- RegisterActivity.java
|   |   |   |-- MainActivity.java
|   |   |   |-- FirestoreAlertService.java
|   |   |   |-- EmergencyActivity.java
|   |   |   |-- Device.java
|   |   |   `-- DeviceAdapter.java
|   |   |-- res/layout/
|   |   |   |-- activity_login2.xml
|   |   |   |-- activity_register2.xml
|   |   |   |-- activity_main2.xml
|   |   |   |-- activity_emergency.xml
|   |   |   `-- item_device.xml
|   |   `-- AndroidManifest.xml
|   |-- build.gradle.kts
|   `-- google-services.json
`-- build.gradle.kts
```

## Cấu Trúc Firestore

Ứng dụng đang đọc và ghi theo schema sau:

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

Ví dụ user:

```json
{
  "Name": "Nguyen Duc Trieu",
  "password": "abc123",
  "DeviceId": ["xiao_esp32s3_01"]
}
```

Ví dụ device:

```json
{
  "name": "user1",
  "alert": false,
  "alert_type": "PHAT_HIEN_NGA",
  "last_alert_time": null
}
```

Các giá trị `alert_type`:

| Giá trị | Ý nghĩa |
| --- | --- |
| `PHAT_HIEN_NGA` | Phát hiện ngã |
| `PHAT_HIEN_TIENG_HET` | Phát hiện tiếng hét |
| `NGA_VA_HET` | Phát hiện cả ngã và tiếng hét |

## Các Thành Phần Chính

### `LoginActivity`

Chức năng:

- Kiểm tra trạng thái đăng nhập trong `SharedPreferences`.
- Đăng nhập bằng số điện thoại và mật khẩu lưu trong Firestore.
- Lưu `user_phone` và `is_logged_in` vào `SharedPreferences`.
- Khởi động `FirestoreAlertService`.
- Chuyển sang `MainActivity`.

Luồng đăng nhập:

```text
Mở app
  |
  |-- đã đăng nhập -> MainActivity
  `-- chưa đăng nhập -> nhập phone/password
          |
          `-- users/{phone}.password khớp -> lưu session -> khởi động service -> MainActivity
```

### `RegisterActivity`

Chức năng:

- Tạo document mới trong `users/{phone}`.
- Lưu `Name`, `password`, `DeviceId`.
- `DeviceId` được khởi tạo là mảng rỗng.

### `MainActivity`

Chức năng:

- Kiểm tra session `user_phone`.
- Xin các quyền cần thiết để cảnh báo hoạt động ổn định.
- Khởi động `FirestoreAlertService`.
- Hiển thị tên người dùng, số điện thoại và danh sách thiết bị.
- Thêm thiết bị bằng Device ID.
- Xóa thiết bị khỏi tài khoản bằng thao tác nhấn giữ.
- Lắng nghe real-time các document `devices/{deviceId}`.

Quản lý listener:

```text
users/{phone}.DeviceId
  |
  `-- manageDeviceListeners(deviceIds)
        |-- gỡ listener của thiết bị đã xóa
        `-- thêm listener cho thiết bị mới
```

Khi thêm thiết bị, app kiểm tra document `devices/{deviceId}` có tồn tại rồi mới thêm id vào mảng `DeviceId`.

### `Device`

Model ánh xạ dữ liệu thiết bị:

```java
private String id;
private String name;
private boolean alert;
private String alert_type;
private Object last_alert_time;
```

`id` lấy từ Firestore document id sau khi đọc snapshot.

### `DeviceAdapter`

Mỗi item thiết bị hiển thị:

```text
Tên thiết bị
ID: xiao_esp32s3_01
[AN TOÀN] hoặc [PHAT_HIEN_NGA]
```

Tương tác:

- Nhấn giữ item để xóa thiết bị khỏi user hiện tại.
- Thao tác xóa chỉ remove id khỏi `users/{phone}.DeviceId`, không xóa document trong `devices`.

### `FirestoreAlertService`

Đây là foreground service giám sát cảnh báo nền.

Chức năng:

- Chạy foreground service với notification nền.
- Lắng nghe `users/{phone}.DeviceId`.
- Với mỗi device id, lắng nghe `devices/{deviceId}`.
- Khi `alert == true`, tạo cảnh báo khẩn cấp.
- Tránh mở lại cùng một cảnh báo bằng `handledAlertKeys`.
- Tự restart service qua `onTaskRemoved()`.

Luồng cảnh báo:

```text
devices/{deviceId}.alert == true
  |
  |-- đọc alert_type
  |-- đọc last_alert_time
  |-- tạo alert key: deviceId + alert_type + timestamp
  |-- nếu alert key chưa xử lý
          |
          `-- hiển thị full-screen emergency notification
```

Service dùng 2 notification channel:

| Channel | Mục đích |
| --- | --- |
| `AlertServiceChannel` | Notification nền báo service đang giám sát |
| `EmergencyAlertChannel` | Notification khẩn cấp có full-screen intent |

### `EmergencyActivity`

`EmergencyActivity` được mở từ full-screen notification khi có cảnh báo.

Chức năng:

- Hiển thị loại cảnh báo, device id và thời gian.
- Bật âm báo động.
- Rung 3 giây.
- Hiển thị trên màn hình khóa bằng `setShowWhenLocked(true)` và `setTurnScreenOn(true)`.
- Lắng nghe chính document device để tự đóng khi `alert = false`.
- Khi người dùng bấm xác nhận, update `devices/{deviceId}.alert = false`.
- Hủy notification khẩn cấp sau khi xác nhận hoặc sau khi cảnh báo được reset.

## Cảnh Báo Toàn Màn Hình

Service không gọi trực tiếp `startActivity()` từ nền khi có cảnh báo. Thay vào đó, app tạo notification khẩn cấp:

```java
.setCategory(NotificationCompat.CATEGORY_ALARM)
.setPriority(NotificationCompat.PRIORITY_MAX)
.setFullScreenIntent(fullScreenIntent, true)
```

Cách này giúp cảnh báo ổn định hơn khi:

- Màn hình điện thoại đang tắt.
- Điện thoại đang khóa.
- App đang chạy nền.
- Android hạn chế việc mở Activity trực tiếp từ background.

Trên Android 14+, người dùng có thể cần bật quyền thông báo toàn màn hình trong Settings. App kiểm tra bằng:

```java
notificationManager.canUseFullScreenIntent()
```

Nếu chưa được phép, app mở:

```java
Settings.ACTION_MANAGE_APP_USE_FULL_SCREEN_INTENT
```

## Quyền Cần Cấp

Manifest hiện khai báo:

```xml
<uses-permission android:name="android.permission.FOREGROUND_SERVICE" />
<uses-permission android:name="android.permission.FOREGROUND_SERVICE_DATA_SYNC" />
<uses-permission android:name="android.permission.USE_FULL_SCREEN_INTENT" />
<uses-permission android:name="android.permission.REQUEST_IGNORE_BATTERY_OPTIMIZATIONS" />
<uses-permission android:name="android.permission.POST_NOTIFICATIONS" />
<uses-permission android:name="android.permission.VIBRATE" />
<uses-permission android:name="android.permission.WAKE_LOCK" />
<uses-permission android:name="android.permission.INTERNET" />
<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />
<uses-permission android:name="android.permission.SYSTEM_ALERT_WINDOW" />
```

`MainActivity.requestAppPermissions()` xử lý:

| Phiên bản Android | Quyền/cài đặt |
| --- | --- |
| Android 13+ | Quyền thông báo |
| Android 14+ | Quyền thông báo toàn màn hình |
| Android 6+ | Quyền hiển thị trên ứng dụng khác |
| Android 6+ | Bỏ qua tối ưu pin |

Luồng cảnh báo chính dùng full-screen notification. 

## Cấu Hình Build

`app/build.gradle.kts`:

```kotlin
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
}
```

Dependencies chính:

```kotlin
implementation(platform("com.google.firebase:firebase-bom:34.12.0"))
implementation("com.google.firebase:firebase-firestore")
implementation("androidx.lifecycle:lifecycle-service:2.10.0")
```

## Build Và Cài Đặt Bằng Android Studio

1. Mở Android Studio.
2. Chọn **Open**.
3. Mở thư mục:

```text
mobile_app/smartWatchApp
```

4. Đợi Android Studio sync Gradle.
5. Kết nối điện thoại Android bằng USB và bật **USB debugging**.
6. Chọn thiết bị thật trong thanh run target.
7. Bấm **Run** để build và cài app vào thiết bị.

Khi chạy trên thiết bị thật, cần cấp các quyền mà app yêu cầu, đặc biệt là:

- Quyền thông báo.
- Quyền thông báo toàn màn hình trên Android 14+.
- Quyền bỏ qua tối ưu pin.
- Quyền hiển thị trên ứng dụng khác nếu app mở màn hình Settings yêu cầu.

## Cách Sử Dụng

1. Cài app từ Android Studio lên điện thoại.
2. Đăng ký tài khoản hoặc tạo sẵn user trong Firestore.
3. Đăng nhập bằng số điện thoại và mật khẩu.
4. Cấp các quyền được app yêu cầu.
5. Thêm Device ID đã tồn tại trong collection `devices`.
6. Khi Node-RED update `devices/{deviceId}.alert = true`, app mở cảnh báo khẩn cấp.
7. Người dùng bấm xác nhận để set `alert = false`.
