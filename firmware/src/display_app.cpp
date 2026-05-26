#include "display_app.h"
#include <lvgl.h>
#include <ui/ui.h>
#include <time.h>

// Track UI state to avoid restarting LVGL animations every inference cycle.
static int last_state = -1;

LGFX lcd;

static const uint32_t screenWidth  = 240;
static const uint32_t screenHeight = 240;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    // Push LVGL's 16-bit color buffer directly to LovyanGFX.
    lcd.pushImage(area->x1, area->y1, w, h, (uint16_t *)&color_p->full);

    lv_disp_flush_ready(disp_drv);
}

void updateStatusUI(const char* text) {
    lv_label_set_text(ui_uiLabelAlert, text);
    lv_timer_handler();
}

void setupDisplay() {
    lcd.init();
    lcd.setRotation(0); 
    lcd.setBrightness(128);

    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    ui_init();

    updateStatusUI("Display: ready");
    
    lv_obj_add_flag(ui_arcAlert, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_arcSafe, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_arcSafe1, LV_OBJ_FLAG_HIDDEN);
    animSafe1_Animation(ui_arcSafe1, 0); 
}

void updateAlertUI(float fall_conf, float scream_conf) {
    int current_state = 0;

    bool fallAlert = fall_conf >= FALL_ALERT_THRESHOLD;
    bool screamAlert = scream_conf >= SCREAM_ALERT_THRESHOLD;

    if (!fallAlert && !screamAlert) {
        current_state = 0;
    } else if (fallAlert && screamAlert) {
        current_state = 1;
    } else if (screamAlert) {
        current_state = 2;
    } else if (fallAlert) {
        current_state = 3;
    }

    if (current_state == last_state) {
        return;
    }
    last_state = current_state;

    switch (current_state) {
        case 0:
            lv_label_set_text(ui_uiLabelAlert, "SAFE");
            lv_obj_add_flag(ui_arcAlert, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_arcSafe, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_arcSafe1, LV_OBJ_FLAG_HIDDEN);
            animSafe1_Animation(ui_arcSafe1, 0);
            break;

        case 1:
            lv_label_set_text(ui_uiLabelAlert, "FALL AND SCREAM !!!");
            lv_obj_add_flag(ui_arcSafe, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_arcSafe1, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_arcAlert, LV_OBJ_FLAG_HIDDEN);
            animAlert_Animation(ui_arcAlert, 0);
            break;

        case 2:
            lv_label_set_text(ui_uiLabelAlert, "SCREAM !!!");
            lv_obj_add_flag(ui_arcSafe, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_arcSafe1, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_arcAlert, LV_OBJ_FLAG_HIDDEN);
            animAlert_Animation(ui_arcAlert, 0);
            break;

        case 3:
            lv_label_set_text(ui_uiLabelAlert, "FALL !!!");
            lv_obj_add_flag(ui_arcSafe, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_arcSafe1, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_arcAlert, LV_OBJ_FLAG_HIDDEN);
            animAlert_Animation(ui_arcAlert, 0);
            break;
    }
}

void updateTimeUI() {
    struct tm timeinfo;
    
    if(getLocalTime(&timeinfo, 10)){
        lv_label_set_text_fmt(ui_uiLabelTime, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
        lv_label_set_text_fmt(ui_uiLabelDate, "%02d/%02d/%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
        
        if (strcmp(lv_label_get_text(ui_uiLabelAlert), "Setting up...") == 0) {
            lv_label_set_text(ui_uiLabelAlert, "SAFE");
        }
    }
}

void sleepDisplay() {
    lcd.setBrightness(0); 
    lcd.sleep();
}

void update_ble_status_label(const char* text) {
    lv_label_set_text(ui_uiLabelAlert, text);
}
