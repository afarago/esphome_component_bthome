//
// Author: Phong Vu
// Modified by Tom Rust for use with ESPhome
// Modified by Attila Farago for use with BTHome over WifiNow for ESPhome
//

#pragma once

#ifndef __MESH_RC_BEETHOWEN_H__
#define __MESH_RC_BEETHOWEN_H__

#include <functional>

#ifdef USE_ESP32
#include <esp_now.h>
#include <WiFi.h>
#endif
#ifdef USE_ESP8266
// included due to compilation error in espnow for missing u8 type
// more info: https://community.platformio.org/t/error-u8-was-not-declared-in-this-scope/20348
#include "Arduino.h"
#include <espnow.h>
#include <ESP8266WiFi.h>
#endif

namespace beethowen_lib
{

#ifdef USE_ESP32
    typedef std::function<void(const uint8_t *, int)> esp_rc_data_callback_t; //!!TODO
#endif
#ifdef USE_ESP8266
    typedef std::function<void(uint8_t *, uint8_t)> esp_rc_data_callback_t;
#endif
    typedef std::function<void(uint8_t)> esp_rc_command_callback_t;

    extern bool sending;
    extern bool sending_success;
    extern bool esp_now_is_init;
    extern uint8_t *sender;
    extern uint8_t broadcast[6];

    void setupwifi(int channel, bool persistent);
    bool send(uint8_t *dest, uint8_t *data, uint8_t size);
    bool send_find(uint8_t *dest);
    bool send_command_found(uint8_t *dest);
    bool send_with_header(uint8_t *dest, uint8_t *data, uint8_t size);
    void on_data(esp_rc_data_callback_t callback);
    void on_command(esp_rc_command_callback_t callback);
    void wait();
    void setAddr(uint8_t *addr);
    void begin();
    void end();

} // namespace MeshRC

#endif //__MESH_RC_BEETHOWEN_H__
