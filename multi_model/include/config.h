#ifndef CONFIG_H
#define CONFIG_H

// --- THÔNG TIN WIFI ---
#define WIFI_SSID "TP-LINK_01F0"
#define WIFI_PASS "vanhtrieuduc888@"

// --- THÔNG TIN HIVEMQ CLOUD (Bảo mật TLS/SSL) ---
#define MQTT_SERVER "08c8ad4b15ac4370b81835f72e145e5a.s1.eu.hivemq.cloud"
#define MQTT_PORT   8883
#define MQTT_USER   "esp32s3_client"
#define MQTT_PASS   "Abcd13579!"

#define MQTT_TOPIC_PUBLISH "wearable/xiao_esp32s3_01/alerts"

// --- CẤU HÌNH PHẦN CỨNG & PIN ---
// 1. IMU (I2C) - Giữ nguyên ở chân I2C mặc định
#define SDA_PIN D4
#define SCL_PIN D5

// 2. Microphone (I2S) - Chuyển sang các chân D0, D1, D2
#define I2S_WS  D0  // Word Select / L/R Clock
#define I2S_SCK D1  // Bit Clock (BCLK)
#define I2S_SD  D2  // Serial Data (DIN/DOUT)

// 3. TFT Display GC9A01 (Hardware SPI Mặc Định)
// Dùng phần cứng SPI chuẩn của XIAO ESP32S3
#define TFT_SCK   D8  // Chân SCK mặc định
#define TFT_MOSI  D10 // Chân MOSI mặc định (Trên XIAO ESP32S3, MOSI thường là D10)
#define TFT_MISO  -1  // D9 là MISO, nhưng màn hình không dùng tới
#define TFT_CS    D7  // Dùng D7 làm Chip Select
#define TFT_DC    D3  // Chân Data/Command tự chọn
#define TFT_RST   D6  // Chân Reset tự chọn

// --- NGƯỠNG CẢNH BÁO TỰ TIN CỦA AI ---
#define ALERT_THRESHOLD 0.70f

#endif
