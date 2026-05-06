#include "display_app.h"
#include <lvgl.h>
#include <ui/ui.h>
#include <time.h>

// Các state để tránh gọi hàm update animation và text liên tục (tránh flicking)
static int last_state = -1;

LGFX lcd;

// cấu hình buffer cho LVGL
static const uint32_t screenWidth  = 240;
static const uint32_t screenHeight = 240;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    // Sử dụng pushImage - Cách tiêu chuẩn và an toàn nhất để đẩy buffer từ LVGL
    // Việc ép kiểu thẳng sang (uint16_t *) sẽ đẩy byte nguyên gốc lên màn hình
    // tránh lỗi lệch khung do kích cỡ struct màu của LovyanGFX
    lcd.pushImage(area->x1, area->y1, w, h, (uint16_t *)&color_p->full);

    lv_disp_flush_ready(disp_drv);
}

void setupDisplay() {
    // 1. Khởi tạo LovyanGFX
    lcd.init();
    lcd.setRotation(0); 
    lcd.setBrightness(128);

    // 2. Khởi tạo lõi LVGL
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

    // 3. Khởi tạo và thiết lập Display Driver cho LVGL
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // 4. Khởi tạo UI (Gọi giao diện vẽ từ SquareLine)
    ui_init();

    // 5. THIẾT LẬP GIAO DIỆN KHỞI ĐỘNG BAN ĐẦU
    lv_label_set_text(ui_uiLabelAlert, "Setting up...");
    
    // Ẩn vòng báo động, hiện vòng xoay "safe" biểu thị đang load
    lv_obj_add_flag(ui_arcAlert, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_arcSafe, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_arcSafe1, LV_OBJ_FLAG_HIDDEN);
    animSafe1_Animation(ui_arcSafe1, 0); 
}

void updateAlertUI(float fall_conf, float scream_conf) {
    // cap nhat canh bao
    int current_state = 0;

    if (fall_conf < ALERT_THRESHOLD && scream_conf < ALERT_THRESHOLD) {
        current_state = 0; // SAFE
    } else if (fall_conf >= ALERT_THRESHOLD && scream_conf >= ALERT_THRESHOLD) {
        current_state = 1; // FALL AND SCREAM
    } else if (scream_conf >= ALERT_THRESHOLD && fall_conf < ALERT_THRESHOLD) {
        current_state = 2; // SCREAM ONLY
    } else if (fall_conf >= ALERT_THRESHOLD && scream_conf < ALERT_THRESHOLD) {
        current_state = 3; // FALL ONLY
    }

    // Chỉ cập nhật giao diện khi trạng thái thay đổi để tránh nháy màn hình hoặc reset animation liên tục
    if (current_state == last_state) {
        return;
    }
    last_state = current_state;

    switch (current_state) {
        case 0: // Cả hai < ALERT_THRESHOLD => SAFE
            lv_label_set_text(ui_uiLabelAlert, "SAFE");
            
            // Ẩn cảnh báo
            lv_obj_add_flag(ui_arcAlert, LV_OBJ_FLAG_HIDDEN);
            
            // Hiện vòng cung an toàn
            lv_obj_clear_flag(ui_arcSafe, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_arcSafe1, LV_OBJ_FLAG_HIDDEN);
            
            // Chạy animation
            animSafe1_Animation(ui_arcSafe1, 0);
            break;

        case 1: // Cả hai >= ALERT_THRESHOLD => FALL AND SCREAM !!!
            lv_label_set_text(ui_uiLabelAlert, "FALL AND SCREAM !!!");
            
            // Ẩn vòng cung an toàn
            lv_obj_add_flag(ui_arcSafe, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_arcSafe1, LV_OBJ_FLAG_HIDDEN);
            
            // Hiện cảnh báo
            lv_obj_clear_flag(ui_arcAlert, LV_OBJ_FLAG_HIDDEN);
            
            // Chạy animation
            animAlert_Animation(ui_arcAlert, 0);
            break;

        case 2: // Chỉ Scream >= ALERT_THRESHOLD
            lv_label_set_text(ui_uiLabelAlert, "SCREAM !!!");
            
            // Ẩn vòng cung an toàn
            lv_obj_add_flag(ui_arcSafe, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_arcSafe1, LV_OBJ_FLAG_HIDDEN);
            
            // Hiện cảnh báo
            lv_obj_clear_flag(ui_arcAlert, LV_OBJ_FLAG_HIDDEN);
            
            // Chạy animation
            animAlert_Animation(ui_arcAlert, 0);
            break;

        case 3: // Chỉ Fall >= ALERT_THRESHOLD
            lv_label_set_text(ui_uiLabelAlert, "FALL !!!");
            
            // Ẩn vòng cung an toàn
            lv_obj_add_flag(ui_arcSafe, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_arcSafe1, LV_OBJ_FLAG_HIDDEN);
            
            // Hiện cảnh báo
            lv_obj_clear_flag(ui_arcAlert, LV_OBJ_FLAG_HIDDEN);
            
            // Chạy animation
            animAlert_Animation(ui_arcAlert, 0);
            break;
    }
}

void updateTimeUI() {
    struct tm timeinfo;
    
    // Mỗi lần gọi hàm sẽ thử lấy giờ, time out 10ms (non-blocking)
    if(getLocalTime(&timeinfo, 10)){
        lv_label_set_text_fmt(ui_uiLabelTime, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
        lv_label_set_text_fmt(ui_uiLabelDate, "%02d/%02d/%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
        
        // Cập nhật trạng thái "SAFE" khi có giờ và thoát khỏi chế độ setup
        if (strcmp(lv_label_get_text(ui_uiLabelAlert), "Setting up...") == 0) {
            lv_label_set_text(ui_uiLabelAlert, "SAFE");
        }
    }
}

void sleepDisplay() {
    lcd.setBrightness(0); 
    lcd.sleep();
}