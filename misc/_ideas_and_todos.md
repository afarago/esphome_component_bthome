# IDEAs and TODOs

- receiver: invalidate after x seconds  / add auto expiry somehow, sensor dows not support publishing "N/A" / has_value = false
- add icons - as of now I do not understand how default icon by device_class is selected
- add bthome BLE encryption
- add ESP-NOW security - though flow is not trivial yet
- Home Assistant parser integration / esp add beethowen "server" component option to proxy all incoming data to HA / with service to send data / proxy data to - problem:ESP32
- add event -- source: should I add an action to simply "add" an event? | destination is unclear, should I just create a trigger / or auto-proxy an event to Home Asistant
- add ACK / return value - addedd value is not clear
- auto find mechanism finetuning // e.g. if not found for first iteration just skip measurement and sleep for 10 iterations / thus have an unsuccess counter before reconnect attempt
- transmitter: invalidate auto_send / per sensor
- separate receiver / handlere with a semaphored queue + external thread

----------------------------------------------------------------
# Speedup the find server

WiFi.mode(WIFI_AP_STA);
WiFi.softAP("temp", "temppass", channel, true, 0); // this introduces ~1800ms delay - known issue https://github.com/espressif/arduino-esp32/issues/6706
[12:26:28][D][beethowen:049]: ..setupwifi 3 5206
[12:26:30][D][beethowen:052]: ..setupwifi 4 7046

-----------
# ESP-NOW security

* one hub PMK - 16 bit private master key -- esp_now_set_pmk((uint8_t *)PMK_KEY_STR);
* each device has LMK - If this key is not indicated, the action frame will not be encrypted
  esp_now_peer_info_t slaveInfo;  memcpy(slaveInfo.peer_addr, slaveAddress, 6);  slaveInfo.channel = 0;  for (uint8_t i = 0; i < 16; i++) {    slaveInfo.lmk[i] = LMK_KEY_STR[i];  }  slaveInfo.encrypt = true;  if (esp_now_add_peer(&slaveInfo) != ESP_OK) {}

Note: The maximum number of peers which may be registered is 20 (espnow.MAX_TOTAL_PEER_NUM), with a maximum of 6 (espnow.MAX_ENCRYPT_PEER_NUM) of those peers with encryption enabled (see ESP_NOW_MAX_ENCRYPT_PEER_NUM in the Espressif API docs).
ESPNow.del_peer(mac)
// maybe can be set up to 17 -- CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM

```
beethowen_transmitter:
	private_key: "PLEASE_CHANGE_ME" / 0x11223344556677889900AABBCCDDEEFF
	remote_key: "DONT_BE_LAZY_OK?"
	connect_persistent: true

beethowen_receiver:
  private_key: "DONT_BE_LAZY_OK?"
  devices:
    - mac_address: 48:3f:da:0c:72:32
      name_prefix: Beethowen TestDevice
      remote_key: "PLEASE_CHANGE_ME"
      dump: unmatched
```

There is a flaw: one can mimic the MAC address and send an unencrypted message. how will I know this on the callback?
- https://github.com/espressif/esp-idf/issues/8577
- https://github.com/espressif/ESP8266_NONOS_SDK/issues/311
- transmitter can safely remove broadcast peer / will not want to receive broadcast from anyone! / still can be fooled by a malicious MAC copy
- server side?: no security

## Flow: Transmitter/Receiver

### scenario 1
* T: find_server[encrypted]
* R: found_server[encrypted] / +channel
* T: data[encrypted]
  - A: success -> OK
  - B: failed send
Q:- does server receive a fuzzy message, but gets notified that MAC tries to reach it (chance to do del_peer for another and add_peer for this) -- if not, need a workaround
* T: want_to_send_data/ping/shoutout[unencrypted]
* R: pong[unencrypted] # + R: add_peer
* T: data[encrypted] # retry
A: success -> happy path
B: failure -> ?? can server assued to be lost ??

### scenario 2
* T: find_server[encrypted] on ch1-11
no success
* T: find_server[unencrypted] on ch1-11
* R: found_server[unencrypted] on chX

A: if: has key for server -> add_peer
* T: find_server[encrypted] on chX
* R: found_server[encrypted] on chX
* T: data[encrypted] on chX

B: else: doesnt have key for server
XXX // if we want secure comm scenario this is not acceptable


-----------
# esp_now_add_peer
`ifidx`: (ESP32 only) Index of the wifi interface which will be used to send data to this peer. Must be an integer set to network.STA_IF (=0) or network.AP_IF (=1). (default=0/network.STA_IF). See ESPNow and Wifi Operation below for more information.

-----------
# reduce power consumption at server
* esp_wifi_connectionless_module_set_wake_interval(100) // 100ms
* esp_now_set_wake_window(70) // 70ms -- 70% wake time


-----------
# OTA_SWITCH:
/probably bad and irrelevant idea/

- add OTA_SWITCH to communication channel
  	```
	ota::OTAComponent *ota_;
	from esphome.components.ota import OTAComponent
	this->get_ota()->set_safe_mode_pending(true);
	
	// Enqueue message as this is called from a high priority wifi thread, and documentation says not to spend too long in here...
      proxy_message *message = (proxy_message *)malloc(sizeof(proxy_message));

      memcpy(message, incomingData, sizeof(proxy_message));
      proxy_message_incoming_queue_->push(message);

      // This seems to be needed to have ESPNow and WiFi working together
      proxy_base::ESPResultDecoder::check_esp_result_bool(WiFi.mode(WIFI_AP_STA), "WiFi.mode");

    void ProxyTransmitterComponent::proxy_setup()
    {
      // This runs before wifi component starts up...
      get_wifi()->mark_failed(); // To kill it

      int channel = peer_receiver_->get_espnow_channel();

      ESP_ERROR_CHECK(esp_netif_init());
      ESP_ERROR_CHECK(esp_event_loop_create_default());
      wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
      ESP_ERROR_CHECK(esp_wifi_init(&cfg));
      ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
      ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
      ESP_ERROR_CHECK(esp_wifi_start());

      setup_espnow();

      peer_receiver_->espnow_add_peer();

      proxy_base::ESPResultDecoder::check_esp_result_code(esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE), "WiFi.channel");
    }
	```

- OTA remote from URL?
- OTA remote mode with AP?
- transmitter_sensor:
	```
	proxy_id: ?
	
		beethowen_receiver:

		sensor:
		  - platform: beethowen_receiver
			mac_address: 11:22:33:44:55:66
			passkey: 0xABCD 						# only accept measurement with this key
			name_prefix: Beethowen TestDevice
			sensors:
			  - measurement_type: temperature
				name: Temperature
		
		############

		ota:

		beethowen_transmitter:
			id: my_beethowen_transmitter
			proxied_sensors:	# sensors and binary_sensors
				- measurement_type: temperature
				  sensor_id: bmp085_temperature_sensor ## "implement sensor holder"
			auto_send: true 				# should check if all sensors have state // not outstanding reads then send it automatically
			auto_deepsleep: true			# capture deepsleep and call on all sensors read / do a read and send if not done before sleep!
			ota-ap:
				ssid: "$systemName"
				password: !secret wifi_password
			persistent: true
	```

----------------------------------------------------------------

# COMMUNICATION:
```
client->bcast:	0xD2FC, 0x01, 0xABCD 				== find server with passkey 0xABCD
server->client:	0xD2FC, 0x02, 0xABCD, 0x0B 			== server found or channel 11
//server->client:	0xD2FC, 0xFF, 0xABCD, 0x22 			== error code 0x22 // e.g. unexpected client key

//server->client:	0xD2FC, 0x03, 0xABCD 				== start local OTA with pre-configured AP
//server->client:	0xD2FC, 0x03, 0xABCD, URL/nullterm 	== start local OTA with pre-configured AP

client->server:	0xD2FC, 0x16, 0xABCD, 0x40, <BTHomeData> == data packet

//server->client:	0xD2FC, 0x00, 0xABCD				== request client system_info
//client->server:	0xD2FC, 0x00, 0xABCD, 0x00 [system_version], 0x01 [v1], 0x01 [system_name], 0x10 [len=16], "beethowen_remote" == response system_info

//check: can I extend sensor.SENSOR_SCHEMA model with a field e.g. beethowen_measurement_type: temperature + modify def to_code? --? probably not
```

----------------------------------------------------------------
# Event handling

decode:
0x3A

3A00 --> binary sensor 3A00
3A01 --> binary sensor 3A01

3C0103 --> ??? dimmer rotated left by 3

3C0103 -->

beethowen_transmitter
	id(my_beethowen_transmitter)

script
	id(my_beethowen_transmitter)->send_event(0x01, 0x03)


```
beethowen_receiver
	devices:
		- mac: aa:bb:cc:dd:ee:ff
		  on_dimmer_event: (subtype, value)
		  on_button_event: (subtype)
```

??how is it repeated if not received properly??

```
typedef enum {
  BTHOME_BUTTON_EVENT_NONE = 0x00, 
  BTHOME_BUTTON_EVENT_PRESS = 0x01, 
  BTHOME_BUTTON_EVENT_LONGPRESS = 0x02, 
} BTHome_Button_Event_Types_e;

typedef enum {
  BTHOME_BUTTON_EVENT_NONE = 0x00, 
  BTHOME_BUTTON_EVENT_PRESS = 0x01, 
  BTHOME_BUTTON_EVENT_LONGPRESS = 0x02, 
} BTHome_Button_Event_Types_e;
```

----------------------------------------------------------------

Event handling

transmitter:
action:
	- beethowen_transmitter.event: button_press
	- beethowen_transmitter.event: button_double_press
	- beethowen_transmitter.event: button_long_press
	- beethowen_transmitter.event: button
		event_type: press
	- beethowen_transmitter.event: dimmer_rotate_left
		steps: 3
	- beethowen_transmitter.event: dimmer
    	event_type: rotate_left
    	steps: 3

vector on events to transmit
remove only upon successful delivery
can add multiple events as well
no need to restore from persistent storage
max queue size ~5

max_event_len: 3bytes

vector<uint32> queued_events_;
queued_events_.push_back(0x3A01)
queued_events_.push_back(0x3C0103)

send_data()
	encoder.addMeasurementEvent(0x3A, 0x01)
	encoder.addMeasurementEvent(0x3C, 0x01, 0x03)

receiver:
	::report_measurements_
		on_event_received(bthome_measurement_t sensor_id, uint8_t event_id, uint8_t steps) trigger 


------------


