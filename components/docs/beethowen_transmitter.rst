Beethowen Transmitter
=====================

**Beethowen** is an energy efficient method to transfer data over 
`ESP-NOW <https://www.espressif.com/en/solutions/low-power-solutions/esp-now>`_ Wireless 
Communication Protocol by Espressif Systems between two devices.
*The name came from a playful 2 am brainstorming from "BTHome" + "Over" + "Esp-NoW".*

This component implements local trasmitter and encoding hub.

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
    
    esphome:
      name: $systemName
      libraries:
        - ESP8266WiFi

    beethowen_transmitter:
      id: my_beethowen_transmitter
      connect_persistent: true
  
    sensor:
      - platform: beethowen_transmitter
        name: Beethowen TestDevice Temperature
        measurement_type: temperature
        lambda: |-
          return id(bmp085_temperature_sensor).state;

      - platform: bmp085
        id: my_bmp085
        temperature:
          id: bmp085_temperature_sensor
          internal: true

.. _beethowen-component:

Component/Hub
-------------

The ``beethowen_transmitter`` component creates a global hub so that you can collect beethowen
sensor data from you ESP node and send them to Beethowen receiver remote node.

The beethowen transmitter hub component is an internal model that acts as a central collection
hub to which beethowen transmitter sensors are connected to.

The component depends on the standard WiFi library e.g. ``ESP8266WiFi`` that needs to be added to the 
configuration.
As for now the ``wifi`` component usage is untested and should be avoided.

Auto-find server
~~~~~~~~~~~~~~~~

The ``beethowen_transmitter`` automatically checks each channel to find a node with a ``beethowen_receiver`` 
on the vicinity.

Note: some form of authentication or code-matching will be added soon.

.. _config-beethowen:

Configuration variables:
************************

- **connect_persistent** (*Optional*): ESP8266 is able to reconnect to the last used Wi-Fi network or establishes the same Access Point upon power up or reset. By default, these settings are NOT written to specific sectors of flash memory every time they are changed. Frequently calling these functions could cause wear on the flash memory. See `WiFi library <https://arduino-esp8266.readthedocs.io/en/2.5.2/esp8266wifi/generic-class.html#persistent>`_ reference.

- **id** (*Optional*): Manually specify the ID for this Hub.


.. _bthome-sensor:

Sensor
------

The ``beethowen transmitter sensor`` allows you use a sensor to collect and transfer data to a remote 
Beethowen receiver hub.
First, you need to define a :ref:`beethowen hub component <beethowen-component_>`.

The beethowen sensor component acts as a template sensor that receives data from other pyhiscal sensors.

.. code-block:: yaml

    # Example configuration entry
    sensor:
      - platform: beethowen_transmitter
        name: Beethowen TestDevice Temperature
        measurement_type: temperature
        lambda: |-
          return id(bmp085_temperature_sensor).state;

Warning
-------
  
* Supports value based sensors as for now, binary sensor support will be added shortly.
* Supports ESP8266 is completed ESP32 support will be added shortly.

See Also
--------

- `BTHome <https://bthome.io>`__ by Ernst Klamer, Victor, Paulus Schoutsen.
- `ESP_NOW <https://www.espressif.com/en/solutions/low-power-solutions/esp-now>`__ by Espressif Systems.
- `Passive BLE Monitor integration <https://github.com/custom-components/ble_monitor>`__ .
