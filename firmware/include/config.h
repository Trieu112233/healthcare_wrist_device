#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials are stored by BLE provisioning, not hardcoded in firmware.

#define MQTT_SERVER "08c8ad4b15ac4370b81835f72e145e5a.s1.eu.hivemq.cloud"
#define MQTT_PORT   8883
#define MQTT_USER   "esp32s3_client"
#define MQTT_PASS   "Abcd13579!"

#define MQTT_TOPIC_PUBLISH "wearable/xiao_esp32s3_01/alerts"

// Hardware pins
#define SDA_PIN         D5
#define SCL_PIN         D4
#define IMU_INT1_PIN    D2

#define I2S_WS  D9
#define I2S_SCK D1
#define I2S_SD  D6

#define TFT_SCK   D8
#define TFT_MOSI  D10
#define TFT_MISO  -1
#define TFT_CS    D7
#define TFT_DC    D3
#define TFT_RST   -1

#define ALERT_THRESHOLD 0.80f

#endif
