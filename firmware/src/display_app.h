#ifndef DISPLAY_APP_H
#define DISPLAY_APP_H

#include <LovyanGFX.hpp>
#include "../include/config.h"

class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_GC9A01 _panel_instance;
    lgfx::Bus_SPI      _bus_instance;

public:
    LGFX(void) {
        auto cfg = _bus_instance.config();
        cfg.spi_host = SPI2_HOST;
        cfg.spi_mode = 0;
        cfg.freq_write = 40000000;
        cfg.freq_read  = 16000000;
        cfg.spi_3wire  = false;
        
        cfg.pin_sclk = TFT_SCK;
        cfg.pin_mosi = TFT_MOSI;
        cfg.pin_miso = TFT_MISO;
        cfg.pin_dc   = TFT_DC;
        _bus_instance.config(cfg);
        _panel_instance.setBus(&_bus_instance);

        auto pcfg = _panel_instance.config();
        pcfg.pin_cs   = TFT_CS;
        pcfg.pin_rst  = TFT_RST;
        pcfg.pin_busy = -1;
        
        pcfg.memory_width  = 240;
        pcfg.memory_height = 240;
        pcfg.panel_width  = 240;
        pcfg.panel_height = 240;
        pcfg.offset_x     = 0;
        pcfg.offset_y     = 0;
        pcfg.offset_rotation = 0;
        pcfg.dummy_read_pixel = 8;
        pcfg.dummy_read_bits  = 1;
        pcfg.readable     = true;
        pcfg.invert       = true;
        pcfg.rgb_order    = false;
        pcfg.dlen_16bit   = false;
        pcfg.bus_shared   = true;

        _panel_instance.config(pcfg);
        setPanel(&_panel_instance);
    }
};

void setupDisplay();
void updateStatusUI(const char* text);
void updateAlertUI(float fall_conf, float scream_conf);
void updateTimeUI();
void sleepDisplay();
void update_ble_status_label(const char* text);

#endif
