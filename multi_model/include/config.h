#ifndef CONFIG_H
#define CONFIG_H

// --- THÔNG TIN WIFI ---
#define WIFI_SSID "***"
#define WIFI_PASS "***"

// --- THÔNG TIN HIVEMQ CLOUD (Bảo mật TLS/SSL) ---
#define MQTT_SERVER "***"
#define MQTT_PORT   8883
#define MQTT_USER   "***"
#define MQTT_PASS   "***"

#define MQTT_TOPIC_PUBLISH "***"

// --- CẤU HÌNH PHẦN CỨNG & PIN ---
// 1. IMU (I2C) LSM6DS3
#define SDA_PIN         D4
#define SCL_PIN         D5
#define IMU_INT1_PIN    D2

// 2. Microphone (I2S) INMP441
#define I2S_WS  D9  // Word Select / L/R Clock
#define I2S_SCK D1  // Bit Clock (BCLK)
#define I2S_SD  D6  // Serial Data (DIN/DOUT)

// 3. TFT Display GC9A01 (Hardware SPI Mặc Định)
#define TFT_SCK   D8  // Chân SCK mặc định
#define TFT_MOSI  D10 // Chân MOSI mặc định 
#define TFT_MISO  -1  // D9 là MISO, màn hình không dùng tới
#define TFT_CS    D7  // Chân Chip Select
#define TFT_DC    D3  // Chân Data/Command  
#define TFT_RST   -1  // Nối chân RST của màn hình vào 3V3

// --- NGƯỠNG CẢNH BÁO TỰ TIN CỦA AI ---
#define ALERT_THRESHOLD 0.80f

#endif