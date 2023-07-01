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
		// ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
		WiFi.channel(channel);
	}

	bool send(uint8_t *dest, uint8_t *data, uint8_t size)
	{
		// Send to a specific node
		sending = true;
		sendTime = micros();
		bool success = false;
		sending_success = false;
		// ESP_LOGD("custom", "meshRC send {size}:%d", size);
		if (esp_now_is_init)
		{
#if defined(USE_ESP8266)
			success = esp_now_send(dest ? dest : broadcast, data, size) == OK;
#elif defined(USE_ESP32)
			uint8_t *addr = dest ? dest : broadcast;
			if (!esp_now_is_peer_exist(addr))
				addPeer(addr);

			/*
			** On ESP32 devices, the Espressif ESP-NOW software requires that other devices (peers) must be registered using add_peer() before we can send() them messages (this is not enforced on ESP8266 devices). It is not necessary to register a peer to receive an un-encrypted message from that peer.
			** Encrypted messages: To receive an encrypted message, the receiving device must first register the sender and use the same encryption keys as the sender (PMK and LMK) (see set_pmk() and add_peer().
			*/

			success = esp_now_send(addr, data, size) == ESP_OK;
#endif
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
		buffer.command_data.deviceinfo = BEETHOWEN_BTHOME_DEVICEINFO;
		memcpy(buffer.command_data.data, data, payload_len);

		return send(dest, buffer.raw, payload_len + BEETHOWEN_HEADER_LEN + BEETHOWEN_DATA_EXTRA_LEN);
	}

	bool on_command(esp_rc_command_callback_t callback)
	{
		if (events_num == MAX_CALLBACKS - 1)
			return false;

		events[events_num++] = (esp_rc_event_t){callback};
		return true;
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

#if defined(USE_ESP8266) or defined(USE_ESP32)
	void addPeer(uint8_t *addr)
	{
		if (esp_now_is_peer_exist(addr))
			esp_now_del_peer(addr);
#if defined(USE_ESP32)
		esp_now_peer_info_t peerInfo = {};
		peerInfo.channel = 0;
		peerInfo.encrypt = false;
		memcpy(peerInfo.peer_addr, addr, 6);
		esp_now_add_peer(&peerInfo);
#elif defined(USE_ESP8266)
		esp_now_add_peer(addr, ESP_NOW_ROLE_COMBO, 1, 0, 0);
#endif
	}

#if defined(USE_ESP32)
	void sendHandler(const uint8_t *addr, esp_now_send_status_t sendStatus)
#elif defined(USE_ESP8266)
	esp_now_send_cb_t sendHandler = [](uint8_t *addr, uint8_t sendStatus)
#endif
	{
		if
#if defined(USE_ESP32)
			(sendStatus == ESP_NOW_SEND_SUCCESS)
#elif defined(USE_ESP8266)
			(sendStatus == 0)
#endif
		{
			// ESP_LOGD("custom", "meshRC message succesfully sent");
			sending_success = true;
		}
		else
		{
			// ESP_LOGD("custom", "meshRC message not succesfully sent, status:%d", sendStatus);
			sending_success = false;
			sent_error++;
		}
		sending = false;
		duration = micros() - sendTime;
	};

#if defined(USE_ESP32)
	void recvHandler(const uint8_t *addr, const uint8_t *data, int size)
#elif defined(USE_ESP8266)
	esp_now_recv_cb_t recvHandler = [](uint8_t *addr, uint8_t *data, uint8_t size)
#endif
	{
		static uint8_t offset, i;
		// Only receives from master if set
		if (addr == NULL || master == NULL || equals(addr, master, 6))
		{
			received++;
			sender = (uint8_t *)addr;

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
		esp_now_unregister_recv_cb();
		esp_now_unregister_send_cb();
	}

	void begin()
	{
		if
#if defined(USE_ESP32)
			(esp_now_init() == ESP_OK)
#elif defined(USE_ESP8266)
			(esp_now_init() == OK)
#endif
		{
			// uint32_t version;
			// esp_now_get_version(&version);
			// ESP_LOGD("custom", "{esp-now version}:%d", version);

			esp_now_is_init = true;
#if defined(USE_ESP8266)
			esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
#endif
			addPeer(broadcast);
			esp_now_register_send_cb(sendHandler);
			esp_now_register_recv_cb(recvHandler);
		}
	}
#endif // ESP32 or ESP8266

} // namespace beethowen_base
