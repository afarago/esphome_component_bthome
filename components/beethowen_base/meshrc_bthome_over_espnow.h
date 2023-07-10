//
// Author: Phong Vu
// Modified by Tom Rust for use with ESPhome
// Modified by Attila Farago for use with BTHome over WifiNow for ESPhome
//

#pragma once

#include <functional>

#ifdef USE_ESP32
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#endif
#ifdef USE_ESP8266
// included due to compilation error in espnow for missing u8 type
// more info: https://community.platformio.org/t/error-u8-was-not-declared-in-this-scope/20348
#include "Arduino.h"
#include <espnow.h>
#include <ESP8266WiFi.h>
#endif

#include "beethowen_base_common.h"

namespace beethowen_base
{
#ifdef USE_ESP32
    typedef std::function<void(const uint8_t, const uint8_t *, const int)> esp_rc_command_callback_t;
#endif
#ifdef USE_ESP8266
    typedef std::function<void(const uint8_t, uint8_t *, const int)> esp_rc_command_callback_t;
#endif

    extern bool sending;
    extern bool sending_success;
    extern bool esp_now_is_init;
    extern uint8_t *sender;
    extern uint8_t broadcast[6];

    void setupwifi(int channel, bool persistent);
    bool send(uint8_t *dest, uint8_t *data, uint8_t size);
    bool on_command(esp_rc_command_callback_t callback);

    void wait();
    void delayMs(uint32_t time);
    void addPeer(uint8_t *addr);
    void begin(bool use_broadcast = true);
    void end();

} // namespace MeshRC
