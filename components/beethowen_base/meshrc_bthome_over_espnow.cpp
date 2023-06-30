//
// Author: Phong Vu
// Modified by Tom Rust for use with ESPhome
// Modified by Attila Farago for use with BTHome over WifiNow for ESPhome
//

#include <functional>
#include <string>
#include <memory>
#include <new> // std::nothrow

#include "meshrc_bthome_over_espnow.h"
#include "beethowen_base_common.h"

// #include "esphome\core\log.h"
// using namespace esphome;

namespace beethowen_base
{
	// Magic bytes 0xD2 0xFC 0x80
	// flags: 0x80 stands for BTHome V2 (bits 5-7) //0x40
	const uint8_t BEETHOWEN_MAGIC_HEADER[BEETHOWEN_MAGIC_HEADER_LEN] = {0xD2, 0xFC};

	struct esp_rc_event_t
	{
		esp_rc_command_callback_t callback_command;
	} events[MAX_CALLBACKS];

	beethowen_data_packet_t buffer;
	uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t events_num = 0;
	uint32_t sent_error = 0;
	uint32_t received;
	uint32_t ignored;
	uint8_t *sender;
	uint32_t sendTime;
	uint16_t duration;
	uint8_t *master = NULL; // TODO: ??
	bool sending;
	bool sending_success;
	bool esp_now_is_init = false;

	void setupwifi(int channel, bool persistent)
	{
		// Use this function when you want the node to only communicate over esp-now.
		if (persistent)
			WiFi.persistent(true); // reduce connect from 980ms -> 120ms //TODO: check

		WiFi.mode(WIFI_AP_STA);
		// WiFi.mode(WIFI_AP);
		WiFi.softAP("temp", "temppass", channel, true, 0); // flash write to optimize...
		WiFi.mode(WIFI_STA);
		WiFi.channel(channel);
	}

	bool send(uint8_t *dest, uint8_t *data, uint8_t size)
	{
		// Send to a specific node
		sending = true;
		sendTime = micros();
		bool success = false;
		sending_success = false;
		// ESP_LOGD("custom", "meshRC send");
		if (esp_now_is_init)
		{
			success = (esp_now_send(dest ? dest : broadcast, data, size) == OK); // TODO; ESP32 has ESP_OK
		}
		else
		{
			success = false;
			sending = false;
		}
		return success;
	}

	void fill_beethowen_header(uint8_t command, uint16_t passkey)
	{
		memcpy(buffer.command__header.magic, BEETHOWEN_MAGIC_HEADER, BEETHOWEN_MAGIC_HEADER_LEN);
		buffer.command__header.command = command;
		buffer.command__header.passkey = passkey;
	}

	bool send_command_find(uint8_t *dest, uint8_t server_channel, uint16_t passkey)
	{
		fill_beethowen_header(BeethowenCommand_FindServerRequest, passkey);
		buffer.command_find_found.server_channel = server_channel;

		return send(dest, buffer.raw, sizeof(beethowen_command_find_found_t));
	}

	bool send_command_found(uint8_t *dest, uint8_t server_channel, uint16_t passkey)
	{
		fill_beethowen_header(BeethowenCommand_FoundServerResponse, passkey);
		buffer.command_find_found.server_channel = server_channel;

		return send(dest, buffer.raw, sizeof(beethowen_command_find_found_t));
	}

	bool send_command_data(uint8_t *dest, uint8_t *data, uint8_t payload_len, uint16_t passkey)
	{
		if (payload_len > MAX_BEETHOWEN_PAYLOAD_LENGTH)
			return false;

		fill_beethowen_header(BeethowenCommand_Data, passkey);
		buffer.command_data.deviceinfo = 0x40;
		memcpy(buffer.command_data.data, data, payload_len);

		return send(dest, buffer.raw, payload_len + BEETHOWEN_MAGIC_HEADER_LEN + BEETHOWEN_MAGIC_DATA_EXTRA_HEADER_LEN);
	}

	void on_command(esp_rc_command_callback_t callback)
	{
		events[events_num++] = (esp_rc_event_t){callback};
	}

	void wait()
	{
		while (sending)
			yield(); // Wait while sending
	}

	void delayMs(uint32_t time)
	{
		uint32_t delayUntil = millis() + time;
		while (millis() < delayUntil)
			yield();
	}

	bool equals(const uint8_t *a, const uint8_t *b, uint8_t size, uint8_t offset = 0)
	{
		for (auto i = offset; i < offset + size; i++)
			if (a[i] != b[i])
				return false;
		return true;
	}
	bool equals(uint8_t *a, uint8_t *b, uint8_t size, uint8_t offset = 0)
	{
		for (auto i = offset; i < offset + size; i++)
			if (a[i] != b[i])
				return false;
		return true;
	}

#ifdef USE_ESP32
	void setAddr(uint8_t *addr)
	{
		if (esp_now_is_peer_exist(addr))
			esp_now_del_peer(addr);
		esp_now_peer_info_t peerInfo;
		peerInfo.channel = 0;
		peerInfo.encrypt = false;
		memcpy(peerInfo.peer_addr, addr, 6);
		esp_now_add_peer(&peerInfo);
	}

	void sendHandler(const uint8_t *addr, esp_now_send_status_t sendStatus)
	{
		if (sendStatus == ESP_NOW_SEND_SUCCESS)
		{
			// ESP_LOGD("custom", "meshRC message succesfully sent");
		}
		else
		{
			// ESP_LOGD("custom", "meshRC message not succesfully sent");
			sent_error++;
		}
		sending_success = sendStatus == ESP_NOW_SEND_SUCCESS;
		sending = false;
		duration = micros() - sendTime;
	}

	void recvHandler(const uint8_t *addr, const uint8_t *data, int size)
	{
		static uint8_t offset, i;
		// Only receives from master if set
		if (addr == NULL || master == NULL || equals(addr, master, 6))
		{
			received++;
			sender = (uint8_t *)addr;
			for (i = 0; i < events_num; i++)
			{
				offset = events[i].prefix.length();
				if (equals(data, (uint8_t *)events[i].prefix.c_str(), offset))
				{
					if (events[i].callback)
						events[i].callback();
					if (events[i].callback2)
						events[i].callback2(&data[offset], size - offset);
				}
			}
		}
		else
		{
			ignored++;
		}
	}

	void begin()
	{
		if (esp_now_init() == ESP_OK)
		{
			esp_now_is_init = true;
			if (esp_now_is_peer_exist(broadcast))
			{
				esp_now_del_peer(broadcast);
			}
			esp_now_peer_info_t peerInfo = {};
			peerInfo.channel = 0;
			peerInfo.encrypt = false;
			memcpy(peerInfo.peer_addr, broadcast, 6);
			esp_now_add_peer(&peerInfo);
			esp_now_register_send_cb(sendHandler);
			esp_now_register_recv_cb(recvHandler);
		}
	}
#endif

#ifdef USE_ESP8266
	void setAddr(uint8_t *addr)
	{
		if (esp_now_is_peer_exist(addr))
			esp_now_del_peer(addr);
		esp_now_add_peer(addr, ESP_NOW_ROLE_COMBO, 1, 0, 0);
	}

	esp_now_send_cb_t sendHandler = [](uint8_t *addr, uint8_t sendStatus)
	{
		if (sendStatus == 0)
		{
			// NOOP
			// ESP_LOGD("custom", "meshRC message succesfully sent");
		}
		if (sendStatus == 1)
		{
			// ESP_LOGD("custom", "meshRC message not succesfully sent %d", sendStatus);
			sent_error++;
		}
		sending_success = sendStatus == 0;
		sending = false;
		duration = micros() - sendTime;
	};

	esp_now_recv_cb_t recvHandler = [](uint8_t *addr, uint8_t *data, uint8_t size)
	{
		static uint8_t offset, i;
		// Only receives from master if set
		if (addr == NULL || master == NULL || equals(addr, master, 6))
		{
			received++;
			sender = addr;

			uint8_t command = data[BEETHOWEN_MAGIC_HEADER_LEN];
			if (equals(data, BEETHOWEN_MAGIC_HEADER, BEETHOWEN_MAGIC_HEADER_LEN))
			{
				uint8_t command = data[BEETHOWEN_MAGIC_HEADER_LEN];
				for (i = 0; i < events_num; i++)
				{
					if (events[i].callback_command)
						events[i].callback_command(command, data, size);
				}
			}
		}
		else
		{
			ignored++;
		}
	};

	void end()
	{
		// TODO: add to ESP32
		esp_now_unregister_recv_cb();
		esp_now_unregister_send_cb();
	}

	void begin()
	{
		if (esp_now_init() == OK)
		{
			esp_now_is_init = true;
			if (esp_now_is_peer_exist(broadcast))
			{
				esp_now_del_peer(broadcast);
			}
			esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
			esp_now_add_peer(broadcast, ESP_NOW_ROLE_COMBO, 1, 0, 0);
			esp_now_register_send_cb(sendHandler);
			esp_now_register_recv_cb(recvHandler);
		}
	}

#endif
} // namespace MeshRC
