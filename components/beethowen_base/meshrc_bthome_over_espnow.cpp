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

// #include "esphome\core\log.h" // debug mode only
// using namespace esphome;

namespace beethowen_base
{

#define MAX_CALLBACKS 10

	esp_rc_command_callback_t events[MAX_CALLBACKS];
	uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t events_num = 0;
	uint32_t sent_error = 0;
	uint32_t received;
	uint32_t ignored;
	uint8_t *sender;
	uint32_t sendTime;
	uint16_t duration;
	uint8_t *master = NULL;
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

	bool on_command(esp_rc_command_callback_t callback)
	{
		if (events_num == MAX_CALLBACKS - 1)
			return false;

		events[events_num++] = callback;
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
	}; // NOTE: ; is important to keep for ESP8266 lambda function type

#if defined(USE_ESP32)
	void recvHandler(const uint8_t *addr, const uint8_t *data, int size)
#elif defined(USE_ESP8266)
	esp_now_recv_cb_t recvHandler = [](uint8_t *addr, uint8_t *data, uint8_t size)
#endif
	{
		// Only receives from master if set
		if (addr == NULL || master == NULL || memcmp(addr, master, 6) == 0)
		{
			received++;
			sender = (uint8_t *)addr;

			BeethowenCommand_e command = get_beethowen_command(data, size);
			if (command != BeethowenCommand_None)
			{
				for (auto i = 0; i < events_num; i++)
					if (events[i])
						events[i](command, data, size);
			}
			else
			{
				ignored++;
			}
		}
	}; // NOTE: ; is important to keep for ESP8266 lambda function type

	void end()
	{
		esp_now_deinit();
		esp_now_unregister_recv_cb();
		esp_now_unregister_send_cb();
		esp_now_is_init = false;
	}

	void begin(bool use_broadcast)
	{
		if
#if defined(USE_ESP32)
			(esp_now_init() == ESP_OK)
#elif defined(USE_ESP8266)
			(esp_now_init() == OK)
#endif
		{
			esp_now_is_init = true;
#if defined(USE_ESP8266)
			esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
#endif
			if (use_broadcast)
				addPeer(broadcast);
			esp_now_register_send_cb(sendHandler);
			esp_now_register_recv_cb(recvHandler);
		}
	}
#endif // ESP32 or ESP8266

} // namespace beethowen_base
