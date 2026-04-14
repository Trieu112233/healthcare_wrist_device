#include "display_app.h"

LGFX lcd;
LGFX_Sprite canvas(&lcd); // Khung hình đệm (Sprite buffer)

void setupDisplay() {
    lcd.init();
    lcd.setRotation(0); 
    lcd.setBrightness(128);

    // Khởi tạo Canvas (Buffer) độ phân giải 240x240, dùng 16 bit màu
    // Yêu cầu khoảng 115KB RAM, ESP32-S3 cân thoải mái
    canvas.setColorDepth(16);
    canvas.createSprite(240, 240);
}

void updateDisplay(float fall_conf, float scream_conf) {
    // 1. Xóa riêng vẽ buffer, không làm màn hình nháy
    canvas.fillSprite(lcd.color565(0, 0, 0)); 

    // 2. Vẽ các thông tin lên canvas trước
    canvas.setTextDatum(middle_center);
    canvas.setFont(&fonts::FreeSansBold12pt7b);
    
    canvas.setTextColor(lcd.color565(255, 255, 255));
    canvas.drawString("AI STATUS", 120, 30);

    // ==========================================
    // 1. VẼ THÔNG TIN FALL (Ngã) - Nửa trên màn hình
    // ==========================================
    canvas.setFont(&fonts::FreeSans9pt7b);
    if (fall_conf >= ALERT_THRESHOLD) {
        canvas.setTextColor(lcd.color565(255, 0, 0)); // Đỏ nều cảnh báo
        canvas.drawString("FALL: ALERT!", 120, 80);
    } else {
        canvas.setTextColor(lcd.color565(0, 255, 0)); // Xanh lều an toàn
        canvas.drawString("FALL: OK", 120, 80);
    }
    
    // In % confidence của Fall
    char fall_text[16];
    sprintf(fall_text, "%.1f %%", fall_conf * 100.0f);
    canvas.setTextColor(lcd.color565(200, 200, 200));
    canvas.drawString(fall_text, 120, 105);


    // ==========================================
    // 2. VẼ THÔNG TIN SCREAM (La Hét) - Nửa dưới màn hình
    // ==========================================
    canvas.setFont(&fonts::FreeSans9pt7b);
    if (scream_conf >= ALERT_THRESHOLD) {
        canvas.setTextColor(lcd.color565(255, 0, 0)); // Đỏ
        canvas.drawString("SCREAM: ALERT!", 120, 150);
    } else {
        canvas.setTextColor(lcd.color565(0, 255, 0)); // Xanh
        canvas.drawString("SCREAM: OK", 120, 150);
    }

    // In % confidence của Scream
    char scream_text[16];
    sprintf(scream_text, "%.1f %%", scream_conf * 100.0f);
    canvas.setTextColor(lcd.color565(200, 200, 200));
    canvas.drawString(scream_text, 120, 175);


    // Đẩy khung hình ra màn hình vật lý
    canvas.pushSprite(0, 0); 
}