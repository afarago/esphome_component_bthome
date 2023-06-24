# BTHome and Beethowen Custom Components for ESPHome

This package implements ESPHome components to low-code support Bluetooth Low Eneregy BTHome device 
standalone reception, and a leverage this protocol over ad-hoc routerless WiFi transmission over 
ESP-NOW channel.

## About
This repository contains external components for [ESPHome](https://esphome.io/).

## Installation
The installation procedure for external components is described [here](https://esphome.io/components/external_components.html).

## External components provided by this repository

- **[bthome_receiver](components/docs/bthome_receiver.rst)**

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