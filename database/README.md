# Firestore Database Schema

## Tổng Quan

Firestore hiện được dùng làm lớp dữ liệu real-time giữa Node-RED và Android app.

Luồng dữ liệu chính:

```text
ESP32-S3 firmware
  |
  | MQTT alert
  v
HiveMQ broker
  |
  v
Node-RED
  |
  | update devices/{deviceId}
  v
Firestore
  |
  | SnapshotListener
  v
Android app
```

## Collections

```text
firestore/
|-- users/
|   `-- {phoneNumber}
|       |-- Name: string
|       |-- password: string
|       `-- DeviceId: array<string>
|
`-- devices/
    `-- {deviceId}
        |-- name: string
        |-- alert: boolean
        |-- alert_type: string
        `-- last_alert_time: timestamp | null
```

## Users Collection

Document ID là số điện thoại người dùng.

Ví dụ:

```json
{
  "Name": "Nguyen Duc Trieu",
  "password": "abc123",
  "DeviceId": ["xiao_esp32s3_01"]
}
```

Các field:

| Field | Type | Ý nghĩa |
| --- | --- | --- |
| `Name` | string | Tên người dùng hiển thị trong app. |
| `password` | string | Mật khẩu đăng nhập hiện tại của app. |
| `DeviceId` | array<string> | Danh sách thiết bị người dùng đang theo dõi. |

App Android hiện dùng `DeviceId` để biết cần listen những document nào trong collection `devices`.

```text
users/{phoneNumber}.DeviceId -> devices/{deviceId}
```

## Devices Collection

Document ID là mã thiết bị, ví dụ `xiao_esp32s3_01`.

Ví dụ trạng thái bình thường:

```json
{
  "name": "user1",
  "alert": false,
  "alert_type": "PHAT_HIEN_NGA",
  "last_alert_time": "May 20, 2026 at 10:55:17 PM UTC+7"
}
```

Ví dụ khi có cảnh báo:

```json
{
  "name": "user1",
  "alert": true,
  "alert_type": "PHAT_HIEN_TIENG_HET",
  "last_alert_time": "May 20, 2026 at 10:58:12 PM UTC+7"
}
```

Các field:

| Field | Type | Ý nghĩa |
| --- | --- | --- |
| `name` | string | Tên thiết bị hiển thị trong app. |
| `alert` | boolean | `true` khi đang có cảnh báo cần app xử lý; `false` khi người dùng đã xác nhận hoặc đang an toàn. |
| `alert_type` | string | Loại cảnh báo gần nhất. |
| `last_alert_time` | timestamp/null | Thời điểm cảnh báo gần nhất, do Node-RED ghi bằng server timestamp. |

Các giá trị `alert_type` hiện dùng:

| Giá trị | Ý nghĩa |
| --- | --- |
| `PHAT_HIEN_NGA` | Phát hiện ngã. |
| `PHAT_HIEN_TIENG_HET` | Phát hiện tiếng hét. |
| `NGA_VA_HET` | Vừa phát hiện ngã vừa phát hiện tiếng hét. |

Lưu ý: `alert_type` có thể vẫn giữ giá trị cảnh báo cũ khi `alert = false`. Điều này không sai, vì app chỉ xem là cảnh báo đang hoạt động khi `alert == true`.

## Node-RED Update

Node-RED hiện update document:

```text
devices/{deviceId}
```

Payload ghi vào Firestore:

```json
{
  "alert": true,
  "alert_type": "PHAT_HIEN_NGA",
  "last_alert_time": "_serverTimestamp"
}
```

Trong function node, `deviceId` lấy từ MQTT payload:

```json
{
  "deviceId": "xiao_esp32s3_01",
  "fall_confidence": 0.95,
  "scream_confidence": 0.0
}
```

## Android App Mapping

Model `Device` trong app đang đọc các field:

```java
private String name;
private boolean alert;
private String alert_type;
private Object last_alert_time;
```

Luồng listen hiện tại:

1. App đọc `users/{phoneNumber}`.
2. Lấy mảng `DeviceId`.
3. Với mỗi id, app listen `devices/{deviceId}`.
4. Nếu `alert == true`, foreground service mở màn hình cảnh báo.
5. Khi người dùng xác nhận, app update `devices/{deviceId}.alert = false`.
