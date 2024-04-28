***Deprecation warning***

I have not been putting much effort to this project lately mainly due to the lack of possibility to easy integration with Home Assistant and also the tedious process of setting up sensors in bot sides.

I think the good direction would be 
* initial and later on demand handshake, also with a full description of the sensor (sensor name, maybe even type and misc attributes) - custom protocol, even on wifi if available or sensor by sensor in separate ESPNOW packages
* regular update of the sensors - current concept, one message batching many sensors
* allow multiple proxies to receive the message
* separate broadcast messages (no server confirmation required similar to BTHome over BLE) and messages where confirmation is needed.

If you feel the energy to take over, kindly fork and bring this forward!

---

# BTHome and Beethowen Custom Components for ESPHome

This package implements ESPHome components to low-code support Bluetooth Low Eneregy BTHome device 
standalone reception, and a leverage this protocol over ad-hoc routerless WiFi transmission over 
ESP-NOW channel.

## About
This repository contains external components for [ESPHome](https://esphome.io/).

## Installation
The installation procedure for external components is described [here](https://esphome.io/components/external_components.html).

## External components provided by this repository

- **[bthome_ble_receiver](components/docs/bthome_ble_receiver.rst)**

    This external component implements Bluetooth Low Energy BTHome local reception and decoding 
    without the need of a central hub.
    
    See also: [BTHome protocol](https://bthome.io/)

- **[beethowen_receiver](components/docs/beethowen_receiver.rst)**

    This external component implements BTHome over ESP-NOW local reception and decoding 
    without the need of a central hub.
    
    See also: [BTHome protocol](https://bthome.io/) over [ESP-NOW communication protocol](https://www.espressif.com/en/solutions/low-power-solutions/esp-now)

- **[beethowen_transmitter](components/docs/beethowen_transmitter.rst)**

    This external component implements BTHome over ESP-NOW local encoding and transmission 
    for low energy smart devices.

    See also: [BTHome protocol](https://bthome.io/) over [ESP-NOW communication protocol](https://www.espressif.com/en/solutions/low-power-solutions/esp-now)

## Overview
![Overview](components/docs/bthome_and_beethowen.drawio.png) 
