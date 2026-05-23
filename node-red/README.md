# Node-RED Flow

## Tổng Quan

Flow Node-RED này nhận cảnh báo từ thiết bị ESP32-S3 qua MQTT, lọc dữ liệu cảnh báo, ghi trạng thái cảnh báo vào Firestore, và đo độ trễ từ lúc nhận MQTT đến sau khi ghi Firestore.

Kiến trúc hiện tại:

```text
ESP32-S3
  |
  | MQTT topic: wearable/+/alerts
  v
HiveMQ Cloud
  |
  v
Node-RED
  |-- Debug thông điệp gốc
  |-- Ghi startTime
  |-- Lọc alert + định dạng Firestore payload
  |-- Firestore update devices/{deviceId}
  |-- Ghi endTime
  `-- Tính timediff
```

## Flow Hiện Tại

File flow: `node-red/flows.json`

Các node chính:

| Node | Loại | Vai trò |
| --- | --- | --- |
| `Nhận Alert Đồng Hồ` | `mqtt in` | Subscribe MQTT topic `wearable/+/alerts` từ HiveMQ. |
| `Thông điệp gốc (JSON)` | `debug` | Hiển thị payload MQTT gốc. |
| Change node `startTime` | `change` | Ghi timestamp bắt đầu xử lý vào `msg.startTime`. |
| `Lọc & định dạng dữ liệu alert` | `function` | Kiểm tra threshold, xác định loại cảnh báo, tạo payload Firestore. |
| `Data đẩy lên firestore` | `debug` | Hiển thị payload sẽ ghi Firestore. |
| `đẩy alert lên firestore` | `Firestore out` | Update document `devices/{deviceId}`. |
| Change node `endTime` | `change` | Ghi timestamp sau khi Firestore node xử lý xong. |
| `Tính timediff` | `function` | Tính `msg.endTime - msg.startTime`. |
| `timediff` | `debug` | Hiển thị độ trễ xử lý theo mili-giây. |

## MQTT

### Broker

- Broker: HiveMQ Cloud
- Host: `08c8ad4b15ac4370b81835f72e145e5a.s1.eu.hivemq.cloud`
- Port: `8883`
- TLS: bật
- MQTT protocol version: 4
- Keepalive: 60 giây
- QoS của MQTT input node: `2`

### Topic

Node MQTT input đang subscribe:

```text
wearable/+/alerts
```

Ký tự `+` cho phép flow nhận alert từ nhiều thiết bị theo cùng format topic.

### Payload Từ Firmware

```json
{
  "deviceId": "xiao_esp32s3_01",
  "fall_confidence": 0.95,
  "scream_confidence": 0.0
}
```

## Firestore

Firestore node hiện dùng package:

```text
node-red-contrib-cloud-firestore@3.2.0
```

Function node đặt `msg.firestore` trước khi đưa vào `Firestore out`:

```js
msg.firestore = {
    operation: "update",
    collection: "devices",
    document: targetDevice
};
```

Document được cập nhật:

```text
devices/{deviceId}
```

Payload hiện ghi vào Firestore:

```json
{
  "alert": true,
  "alert_type": "PHAT_HIEN_NGA",
  "last_alert_time": "_serverTimestamp"
}
```

Các loại `alert_type` trong flow:

| Điều kiện | `alert_type` |
| --- | --- |
| Fall >= 0.8 và Scream >= 0.8 | `NGA_VA_HET` |
| Fall >= 0.8 | `PHAT_HIEN_NGA` |
| Scream >= 0.8 | `PHAT_HIEN_TIENG_HET` |

## Chặn Cảnh Báo Lặp Cho Tiếng Hét

Vấn đề: nếu một tiếng hét kéo dài, firmware có thể publish nhiều alert liên tiếp. Nếu Node-RED ghi tất cả vào Firestore, app sẽ liên tục nhận snapshot update và gây khó chịu cho người dùng.

Cách xử lý trong flow hiện tại: chỉ chặn lặp alert `PHAT_HIEN_TIENG_HET` theo từng thiết bị. Cảnh báo ngã (`PHAT_HIEN_NGA`) và ngã kèm hét (`NGA_VA_HET`) không bị cooldown.

Ví dụ:

- Thiết bị `xiao_esp32s3_01` vừa gửi `PHAT_HIEN_TIENG_HET`.
- Trong 30 giây tiếp theo, các alert `PHAT_HIEN_TIENG_HET` từ cùng thiết bị sẽ bị bỏ qua.
- Nếu trong thời gian đó có `PHAT_HIEN_NGA`, flow vẫn cho qua vì đây là loại alert khác và quan trọng hơn.
- Nếu trong thời gian đó có `PHAT_HIEN_NGA` hoặc `NGA_VA_HET`, flow vẫn cho qua ngay.

## Đo Độ Trễ

Flow hiện có các node:

```text
startTime -> Firestore out -> endTime -> Tính timediff -> debug timediff
```

Function `Tính timediff`:

```js
msg.payload = msg.endTime - msg.startTime;
return msg;
```

Kết quả debug là độ trễ tính bằng mili-giây từ lúc message đi qua change node `startTime` đến sau khi Firestore node trả output.

## Cài Đặt

### 1. Cài Node-RED

```bash
npm install -g node-red
```

### 2. Cài Firestore node

```bash
node-red-admin install node-red-contrib-cloud-firestore
```

### 3. Import Flow

1. Mở Node-RED dashboard: `http://localhost:1880`
2. Chọn menu -> Import
3. Import nội dung từ `node-red/flows.json`
4. Cấu hình Firebase Admin credentials cho node `fall_scream_detection`
5. Deploy