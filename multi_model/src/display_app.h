#ifndef DISPLAY_APP_H
#define DISPLAY_APP_H

#include <LovyanGFX.hpp>
#include "../include/config.h"

// Cấu hình linh kiện phần cứng cho LovyanGFX
class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_GC9A01 _panel_instance;
    lgfx::Bus_SPI      _bus_instance;

public:
    LGFX(void) {
        // Cấu hình bus SPI
        auto cfg = _bus_instance.config();
        cfg.spi_host = SPI2_HOST; // Dùng bus SPI2 (trên ESP32-S3 thường gọi là FSPI)
        cfg.spi_mode = 0;
        cfg.freq_write = 40000000; // Tốc độ SPI 40MHz cho mượt
        cfg.freq_read  = 16000000;
        cfg.spi_3wire  = true;
        
        // Gọi thẳng các chân từ config.h
        cfg.pin_sclk = TFT_SCK;
        cfg.pin_mosi = TFT_MOSI;
        cfg.pin_miso = TFT_MISO;
        cfg.pin_dc   = TFT_DC;
        _bus_instance.config(cfg);
        _panel_instance.setBus(&_bus_instance);

        // Cấu hình Panel GC9A01
        auto pcfg = _panel_instance.config();
        pcfg.pin_cs   = TFT_CS;
        pcfg.pin_rst  = TFT_RST;
        pcfg.pin_busy = -1;
        
        pcfg.panel_width  = 240;
        pcfg.panel_height = 240;
        pcfg.offset_x     = 0;
        pcfg.offset_y     = 0;
        pcfg.offset_rotation = 0;
        pcfg.dummy_read_pixel = 8;
        pcfg.dummy_read_bits  = 1;
        pcfg.readable     = true;
        pcfg.invert       = true;  // GC9A01 cần đảo màu
        pcfg.rgb_order    = false;
        pcfg.dlen_16bit   = false;
        pcfg.bus_shared   = true;  // Cho phép share chung SPI nếu có

        _panel_instance.config(pcfg);
        setPanel(&_panel_instance);
    }
};

void setupDisplay();
void updateDisplay(float fall_conf, float scream_conf);

#endif