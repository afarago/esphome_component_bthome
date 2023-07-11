/*
 Beethowen
 Beethowen over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#include <functional>
#include <string>
#include <memory>
#include <new> // std::nothrow

#include "meshrc_bthome_over_espnow.h"
#include "beethowen_base_common.h"

// #include "esphome\core\log.h" // debug mode only
// using namespace esphome;

namespace beethowen_base
{
    // Magic bytes 0xD2 0xFC 0x80
    // flags: 0x40 stands for BTHome V2 (bits 5-7)
    const uint8_t BEETHOWEN_MAGIC_HEADER[BEETHOWEN_MAGIC_HEADER_LEN] = {0xD2, 0xFC};
    const uint8_t Beethowen_BTHome_DeviceInfo_Flags = 0x40;
    beethowen_data_packet_t buffer;

    void fill_beethowen_header(uint8_t command, uint16_t passkey)
    {
        memcpy(buffer.command__header.magic, BEETHOWEN_MAGIC_HEADER, BEETHOWEN_MAGIC_HEADER_LEN);
        buffer.command__header.command = command;
        buffer.command__header.passkey = passkey;
    }

    BeethowenCommand_e get_beethowen_command(const uint8_t *data, const int size)
    {
        if (size <= BEETHOWEN_MAGIC_HEADER_LEN || memcmp(data, BEETHOWEN_MAGIC_HEADER, BEETHOWEN_MAGIC_HEADER_LEN) != 0)
            return BeethowenCommand_None;

        uint8_t command = data[BEETHOWEN_MAGIC_HEADER_LEN];
        return (BeethowenCommand_e)command;
    }

    bool send_command_find(uint8_t *dest, uint16_t passkey, uint8_t server_channel, bool is_request_not_ack)
    {
        fill_beethowen_header(is_request_not_ack ? BeethowenCommand_FindServerRequest : BeethowenCommand_FindServerRequestAck, passkey);
        buffer.command_find.server_channel = server_channel;

        return send(dest, buffer.raw, sizeof(beethowen_command_find_t));
    }

    bool send_command_data(uint8_t *dest, uint16_t passkey, uint8_t *data, uint8_t payload_len)
    {
        if (payload_len > MAX_BEETHOWEN_PAYLOAD_LENGTH)
            return false;

        fill_beethowen_header(BeethowenCommand_Data, passkey);
        buffer.command_data.deviceinfo = Beethowen_BTHome_DeviceInfo_Flags;
        memcpy(buffer.command_data.data, data, payload_len);

        return send(dest, buffer.raw, payload_len + BEETHOWEN_HEADER_LEN + BEETHOWEN_DATA_EXTRA_LEN);
    }

    bool send_command_data_ack(uint8_t *dest, uint16_t passkey, uint8_t packet_id_acked)
    {
        fill_beethowen_header(BeethowenCommand_DataAck, passkey);
        buffer.command_data_ack.packet_id_acked = packet_id_acked;

        return send(dest, buffer.raw, sizeof(beethowen_command_data_ack_t));
    }
}