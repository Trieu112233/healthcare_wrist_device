#include "ble_provisioning.h"
#include "display_app.h"
#include <WiFi.h>
#include <WiFiProv.h>
#include <Preferences.h>

static const char* BLE_POP_CODE    = "12345678"; 
static const char* BLE_SERVICE_NAME = "ESP32S3_01";
static const char* BLE_SERVICE_KEY  = NULL;

static void SysProvEvent(arduino_event_t *sys_event) {
    switch (sys_event->event_id) {
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.printf("\n[BLE Prov] Đã kết nối, IP: %s\n", IPAddress(sys_event->event_info.got_ip.ip_info.ip.addr).toString().c_str());
            update_ble_status_label("Connected!");
            break;

        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("[BLE Prov] Mất kết nối. Đang thử lại...");
            break;

        case ARDUINO_EVENT_PROV_START:
            Serial.println("\n[BLE Prov] Bắt đầu Provisioning. Vui lòng nhập thông tin WiFi qua app smartphone.");
            update_ble_status_label("Provisioning...");
            break;

        case ARDUINO_EVENT_PROV_CRED_RECV: {
            const char* ssid = (const char*)sys_event->event_info.prov_cred_recv.ssid;
            const char* pass = (const char*)sys_event->event_info.prov_cred_recv.password;
            
            Serial.printf("\n[BLE Prov] Đã nhận thông tin: SSID = %s\n", ssid);
            
            // Store WiFi credentials in NVS so normal boot can skip provisioning.
            Preferences preferences;
            preferences.begin("wifi_creds", false);
            preferences.putString("ssid", ssid);
            preferences.putString("pass", pass);
            preferences.end();

            update_ble_status_label("Connecting...");
            break;
        }

        case ARDUINO_EVENT_PROV_CRED_FAIL: {
            Serial.println("\n[BLE Prov] Provisioning thất bại!");
            if (sys_event->event_info.prov_fail_reason == WIFI_PROV_STA_AUTH_ERROR) {
                Serial.println("[BLE Prov] Lỗi: Sai mật khẩu WiFi.");
                update_ble_status_label("Wrong Pass");
            } else {
                Serial.println("[BLE Prov] Lỗi: Không tìm thấy Access Point.");
                update_ble_status_label("AP not found");
            }
            break;
        }

        case ARDUINO_EVENT_PROV_CRED_SUCCESS:
            Serial.println("\n[BLE Prov] Provisioning thành công!");
            update_ble_status_label("Success!");
            break;

        case ARDUINO_EVENT_PROV_END:
            Serial.println("\n[BLE Prov] Kết thúc quá trình Provisioning. Đang khởi động lại...");
            delay(500);
            ESP.restart();
            break;

        default:
            break;
    }
}

void setupBLEProvisioning() {
    Serial.println("[BLE Prov] Khởi động chế độ chờ BLE Provisioning...");
    
    WiFi.onEvent(SysProvEvent);
    update_ble_status_label("BLE Provisioning");

    WiFiProv.beginProvision(
        WIFI_PROV_SCHEME_BLE, 
        WIFI_PROV_SCHEME_HANDLER_FREE_BTDM,
        WIFI_PROV_SECURITY_1, 
        BLE_POP_CODE, 
        BLE_SERVICE_NAME, 
        BLE_SERVICE_KEY, 
        NULL, 
        true
    );
}

void loopBLEProvisioning() {
}
