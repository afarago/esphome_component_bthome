/*
 Beethowen
 Beethowen over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include <string>

namespace beethowen_base
{
  typedef enum
  {
    BeethowenCommand_None = 0x00,
    BeethowenCommand_FindServerRequest = 0x01,
    BeethowenCommand_FindServerRequestAck = 0x02,
    BeethowenCommand_Data = 0x16,
    BeethowenCommand_DataAck = 0x17,
  } BeethowenCommand_e;

#define ESP_NOW_MAX_DATA_LEN 250
#define BEETHOWEN_MAGIC_HEADER_LEN 2
#define BEETHOWEN_HEADER_LEN (sizeof(beethowen_base::beethowen_command_header_t)) /* +2 magic, +1 command, +2 passkey */
#define BEETHOWEN_DATA_EXTRA_LEN (sizeof(uint8_t))                                /* +1 deviceinfo */
#define MAX_BEETHOWEN_PAYLOAD_LENGTH (ESP_NOW_MAX_DATA_LEN - BEETHOWEN_HEADER_LEN - BEETHOWEN_DATA_EXTRA_LEN)

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
    } beethowen_command_find_t;

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
        beethowen_command_find_t command_find;
        beethowen_command_data_t command_data;
    } beethowen_data_packet_t;

    BeethowenCommand_e get_beethowen_command(const uint8_t *data, const int size);
    bool send_command_find(uint8_t *dest, uint16_t passkey, uint8_t server_channel, bool is_request_not_ack);
    bool send_command_data(uint8_t *dest, uint16_t passkey, uint8_t *data, uint8_t payload_len, bool is_request_not_ack);
}
