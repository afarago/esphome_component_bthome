Beethowen Receiver
==================

**Beethowen** is an energy efficient method to transfer data over 
`ESP-NOW <https://www.espressif.com/en/solutions/low-power-solutions/esp-now>`_ Wireless 
Communication Protocol by Espressif Systems between two devices.
*The name came from a playful 2 am brainstorming from "BTHome" + "Over" + "Esp-NoW".*

This component implements Bluetooth Low Energy BTHome local reception and decoding 
without the need of a central hub.

**ESP-NOW**
  is a wireless communication protocol defined by Espressif, which enables the direct, 
  quick and low-power control of smart devices, without the need of a router. ESP-NOW can work 
  with Wi-Fi and Bluetooth LE, and supports the ESP8266, ESP32, ESP32-S and ESP32-C series of SoCs. 
  It’s widely used in smart-home appliances, remote controlling, sensors, etc.

**BTHome**
  is an energy efficient but flexible format for devices to broadcast their sensor data and button presses 
  originally developed for BLE (Bluetooth Low Energy) devices. Devices can run over a year on a single battery.
  It allows data encryption and is supported by popular home automation platforms, 
  like `Home Assistant <https://www.home-assistant.io>`__, out of the box.

.. code-block:: yaml

    # Example configuration entry
    external_components:
      - source: github://afarago/esphome_component_bthome

    wifi:
      ssid: !secret wifi_ssid
      password: !secret wifi_password
      ap:
        ssid: "$systemName"
        password: !secret wifi_password

    beethowen_receiver:
      dump: unmatched

    sensor:
      - platform: beethowen_receiver
        mac_address: 11:22:33:44:55:55
        name_prefix: Beethowen TestDevice
        sensors:
          - measurement_type: temperature
            name: Temperature
          - measurement_type: 0x02
            name: Temperature_Second
            accuracy_decimals: 2
            unit_of_measurement: °C
          - measurement_type: count_large
            name: Count
            unit_of_measurement: cycles

Notes:

- Supports V1 and V2 **unencryted** formats as for now.

- Home Assistant support is planned, yet not implemented yet. Passive BLE Monitor integration 
  can take RAW HCI packets, that provide an easy way to channel in this payload.

.. _beethowen-component:

Component/Hub
-------------

The ``beethowen_receiver`` component creates a global hub so that you can track beethowen
devices using your ESP node over the BTHome V2 protocol.

The beethowen component is an internal model that acts as a central reception 
and dispatcher hub to which beethowen virtual devices and sensors are connected to.

As for now the ``wifi`` component with an access point setting should be included.

.. _config-beethowen:

Configuration variables:
************************

- **dump** (*Optional*): Decode and dumpincoming remote readings codes in the logs 
  (at log.level=DEBUG) for any device.
  
  - **all**: Decode and dump all readings.
  - **unmatched**: Decode and dump readings that are not mapped in configuration.
  - **none**: (default) Decode but do not dump any readings.

- **id** (*Optional*): Manually specify the ID for this Hub.



.. _bthome-sensor:

Sensor and Binary Sensor as *virtual device*
--------------------------------------------

The ``beethowen sensor`` allows you use a sensor to display received measurement from a remote 
Beethowen device.
First, you need to define a :ref:`beethowen hub component <beethowen-component>`.

The beethowen sensor component (or "device") is an internal model that acts as a central reception 
and dispatcher hub for a specific remote device identified by a ``mac_address`` to which beethowen 
sensors are connected to.

To initialize a sensor, first supply ``mac_address`` to identify the remote Beethowen device.

.. code-block:: yaml

    # Example configuration entry
    beethowen:

    # Individual sensors
    sensor:
      - platform: beethowen
        mac_address: 11:22:33:44:55:55
        name_prefix: Beethowen Test Device
        sensors:
          - measurement_type: temperature
            name: Temperature
          - measurement_type: 0x02
            name: Temperature_Second
            accuracy_decimals: 2
            unit_of_measurement: °C


.. _config-beethowen-sensor:

Configuration variables:
************************

- **mac_address** (**Required**, mac-address): The address of the sensor.

- **name_prefix** (*Optional*): Device name to append before any sensor name as a prefix.

- **dump** (*Optional*): Decode and dump incoming remote readings codes in the logs 
  (at log.level=DEBUG) for this device.
  
  - **all**: Decode and dump all readings.
  - **unmatched**: Decode and dump readings that are not mapped in configuration.
  - **none**: (default) Decode but do not dump any readings.

- **id** (*Optional*): Manually specify the ID for this Hub.

- **sensors** (*Required*): List of remote sensor connected to this virtual device.
  
  - **name** (*Optional*): The name for the sensor. At least one of **id** and **name** must be specified.

  - **measurement_type** (*Required*, int **or** string): Measurement type as defined in 
    `BTHome format specification <https://bthome.io/format>`__ either as a string or a numeric value. 
    If selected by name (string) the accuracy and unit of measurement are automatically defaulted to 
    the correct values.

See Also
--------

- `BTHome <https://bthome.io>`__ by Ernst Klamer, Victor, Paulus Schoutsen.
- `ESP_NOW <https://www.espressif.com/en/solutions/low-power-solutions/esp-now>`__ by Espressif Systems.
- `Passive BLE Monitor integration <https://github.com/custom-components/ble_monitor>`__ .
