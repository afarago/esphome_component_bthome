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
#endif
#ifdef USE_ESP8266
// included due to compilation error in espnow for missing u8 type
// more info: https://community.platformio.org/t/error-u8-was-not-declared-in-this-scope/20348
#include "Arduino.h"
#include <espnow.h>
#include <ESP8266WiFi.h>
#endif

namespace beethowen_base
{

#ifdef USE_ESP32
    typedef std::function<void(const uint8_t *, int)> esp_rc_data_callback_t; //!!TODO
#endif
#ifdef USE_ESP8266
    typedef std::function<void(uint8_t *, uint8_t)> esp_rc_data_callback_t;
#endif
    typedef std::function<void(uint8_t, uint8_t *)> esp_rc_command_callback_t;

    extern bool sending;
    extern bool sending_success;
    extern bool esp_now_is_init;
    extern uint8_t *sender;
    extern uint8_t broadcast[6];

    void setupwifi(int channel, bool persistent);
    bool send(uint8_t *dest, uint8_t *data, uint8_t size);
    bool send_find(uint8_t *dest, uint8_t server_channel, uint16_t passkey);
    bool send_command_found(uint8_t *dest, uint8_t server_channel, uint16_t passkey);
    bool send_with_header(uint8_t *dest, uint8_t *data, uint8_t size);
    //bool send_data(uint8_t *dest, uint8_t *data, uint8_t size);
    void on_data(esp_rc_data_callback_t callback);
    void on_command(esp_rc_command_callback_t callback);
    void wait();
    void setAddr(uint8_t *addr);
    void begin();
    void end();

#define BEETHOWEN_MAGIC_HEADER_LEN 2
#define BEETHOWEN_MAGIC_DATA_HEADER_LEN (BEETHOWEN_MAGIC_HEADER_LEN + 1)
#define COMMAND_FIND_REQUEST 0x01
#define COMMAND_FOUND_RESPONSE 0x02
// TODO: check, this should be av
#define ESP_NOW_MAX_DATA_LEN 250
#define MAX_BEETHOWEN_PAYLOAD_LENGTH (ESP_NOW_MAX_DATA_LEN - BEETHOWEN_MAGIC_DATA_HEADER_LEN)
#define MAX_CALLBACKS 10

	typedef struct __attribute__((packed))
	{
		uint8_t header[BEETHOWEN_MAGIC_HEADER_LEN];
		uint8_t command;
		uint16_t passkey;
		uint8_t server_channel;
	} beethowen_command_find_found_t;

	typedef struct __attribute__((packed))
	{
		uint8_t header[BEETHOWEN_MAGIC_HEADER_LEN];
		uint8_t deviceinfo; // BTHome Device Information (0x40, b01000000) - unencrypted, regular updates, BTHome V2
		uint8_t *data[MAX_BEETHOWEN_PAYLOAD_LENGTH];
	} beethowen_data_t;

	typedef union __attribute__((packed))
	{
		uint8_t raw[ESP_NOW_MAX_DATA_LEN];
		beethowen_command_find_found_t command_find_found;
		beethowen_data_t data;
	} beethowen_data_packet_t;

} // namespace MeshRC
