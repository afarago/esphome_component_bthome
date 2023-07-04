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
    bool send_command_find(uint8_t *dest, uint8_t server_channel, uint16_t passkey);
    bool send_command_found(uint8_t *dest, uint8_t server_channel, uint16_t passkey);
    bool send_command_data(uint8_t *dest, uint8_t *data, uint8_t payload_len, uint16_t passkey);
    // bool send_data(uint8_t *dest, uint8_t *data, uint8_t size);
    bool on_command(esp_rc_command_callback_t callback);
    void wait();
    void delayMs(uint32_t time);
    void addPeer(uint8_t *addr);
    void begin(bool use_broadcast = true);
    void end();

#define ESP_NOW_MAX_DATA_LEN 250

#define BEETHOWEN_MAGIC_HEADER_LEN 2
#define BEETHOWEN_HEADER_LEN (sizeof(beethowen_base::beethowen_command_header_t)) /* +2 magic, +1 command, +2 passkey */
#define BEETHOWEN_DATA_EXTRA_LEN (sizeof(uint8_t))                                /* +1 deviceinfo */
#define MAX_BEETHOWEN_PAYLOAD_LENGTH (ESP_NOW_MAX_DATA_LEN - BEETHOWEN_HEADER_LEN - BEETHOWEN_DATA_EXTRA_LEN)
#define MAX_CALLBACKS 10
#define BEETHOWEN_BTHOME_DEVICEINFO 0x40

    typedef struct __attribute__((packed))
    {
        uint8_t magic[BEETHOWEN_MAGIC_HEADER_LEN];
        uint8_t command;
        uint16_t passkey;
    } beethowen_command_header_t;

    typedef struct __attribute__((packed))
    {
        beethowen_command_header_t header;
        uint8_t server_channel;
    } beethowen_command_find_found_t;

    typedef struct __attribute__((packed))
    {
        beethowen_command_header_t header;
        uint8_t deviceinfo; // BTHome Device Information (0x40, b01000000) - unencrypted, regular updates, BTHome V2
        uint8_t data[MAX_BEETHOWEN_PAYLOAD_LENGTH];
    } beethowen_command_data_t;

    typedef union __attribute__((packed))
    {
        uint8_t raw[ESP_NOW_MAX_DATA_LEN];
        beethowen_command_header_t command__header;
        beethowen_command_find_found_t command_find_found;
        beethowen_command_data_t command_data;
    } beethowen_data_packet_t;

} // namespace MeshRC
