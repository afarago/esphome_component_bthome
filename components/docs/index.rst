BTHome and Beethowen Components
===============================

This package implements ESPHome components to low-code support Bluetooth Low Eneregy BTHome device 
standalone reception, and a leverage this protocol over ad-hoc routerless WiFi transmission over 
ESP-NOW channel.

**BTHome**
  is an energy efficient but flexible BLE (Bluetooth Low Energy) format for devices to 
  broadcast their sensor data and  button presses. Devices can run over a year on a single battery.
  It allows data encryption and is supported by popular home automation platforms, 
  like `Home Assistant <https://www.home-assistant.io>`__, out of the box.

**ESP-NOW**
  is a wireless communication protocol defined by Espressif, which enables the direct, 
  quick and low-power control of smart devices, without the need of a router. ESP-NOW can work 
  with Wi-Fi and Bluetooth LE, and supports the ESP8266, ESP32, ESP32-S and ESP32-C series of SoCs. 
  It’s widely used in smart-home appliances, remote controlling, sensors, etc.

**Beethowen**
  is an energy efficient method to transfer data over 
  `ESP-NOW <https://www.espressif.com/en/solutions/low-power-solutions/esp-now>`_ Wireless 
  Communication Protocol by Espressif Systems between two devices.
  *The name came from a playful 2 am brainstorming from "BTHome" + "Over" + "Esp-NoW".*

**ESPHome**
  is a system to control your ESP8266/ESP32 and RP2040 by simple yet powerful 
  configuration files and control them remotely through Home Automation systems using fast-paced
  low-code development in YAML files.

Further development ideas
=========================

* Generate and unify all BTHome lists from the specification text,
  without and intermediate Excel sheet.
  Maybe create a separate python tool script.

  - including ``bthome_encoder.h`` defines
  - ``bthome_parser.cpp`` progmem array
  - python init/sensor MEASUREMENT_TYPES defines

* Handle multiple measurements of same type option - e.g. by postfixing with _<n>

* Dynamically add sensors after discovery -- optionnal -- could it be possible in ESPHome?

* HA proxy and integration / through ble_monitor

* Add channelid to found response.
  Maybe add random key for req-response, also include some security to match beethowen transmitter and receiver hubs.

* Separate basic Beethowen package formatting from ESP-NOW handling

* Add example yamls configurations.

* Beethowen / ESP-NOW support for ESP32.

* Submit components to ESPHome repository to better share.

Contributions are welcome
=========================

All contributions and pull-requests are welcome.
