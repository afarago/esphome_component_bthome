Beethowen Transmitter
=====================

**Beethowen** is an energy efficient method to transfer data over `ESP-NOW <https://www.espressif.com/en/solutions/low-power-solutions/esp-now>`
Wireless Communication Protocol by Espressif Systems between two devices.
*The name came from a playful 2 am brainstorming from "BTHome" + "Over" + "Esp-NoW".*

This component implements local trasmitter and encoding hub that can be used both on ESP8266 and ESP32 devices.

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
      on_loop:
        - component.update: my_bmp085

    beethowen_transmitter:
      id: my_beethowen_transmitter
      connect_persistent: true
      sensors:
        - measurement_type: temperature
          sensor_id: bmp085_temperature_sensor
        - measurement_type: pressure
          sensor_id: bmp085_pressure_sensor
      auto_send: true
      on_send_finished:
        - lambda: |-
            ESP_LOGD("app", "on_send_finished lambda");
            id(my_deep_sleep).begin_sleep(true);

    sensor:
      - platform: bmp085
        id: my_bmp085
        temperature:
          id: bmp085_temperature_sensor
        pressure:
          id: bmp085_pressure_sensor

    deep_sleep:
      run_duration: 20s # max run duration for safeguarding
      sleep_duration: 60s
      id: my_deep_sleep

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

- **id** (*Optional*, string): Manually specify the ID for this Hub.

- **connect_persistent** (*Optional*, boolean): ESP8266 is able to reconnect to the last used Wi-Fi network or establishes the same Access Point upon power up or reset. By default, these settings are NOT written to specific sectors of flash memory every time they are changed. Frequently calling these functions could cause wear on the flash memory. See `WiFi library <https://arduino-esp8266.readthedocs.io/en/2.5.2/esp8266wifi/generic-class.html#persistent>`_ reference.

- **sensors** (*Optional*): Specify sensors to proxy through the beethowen channel. See :ref:`beethowen-sensor` for more information.

- **auto_send** (*Optional*, boolean): Regularly check all connected sensors and once all provide a valid state automatically send data.

- **local_passkey** (*Optional*, int, 16-bit): Local passkey that serves as an identification or authorization of the node.

- **expected_remote_passkey** (*Optional*, int, 16-bit): Remote passkey that identifies or authorizes the incoming communication packet.

- **restore_from_flash** (*Optional*, boolean): Whether to store teh server data and channel as persistent preferences in flash memory. Defaults to true.

Automations
***********
- **on_send_finished** (*Optional*, Automation): An automation to perform when a transmission is finished.

- **on_send_failed** (*Optional*, Automation): An automation to perform when a transmission is failed.

Authorization with a premature security concept:
************************************************

As ESP-NOW is a a highly insecure channel it is not recommended to use it for purposes above sensing and broacasting.
Opposed to the BLE broadcast mechanism I have implemented a handshake in which remote client send directed data to preidentified servers.

An optional handshake mechanism is easing this process where each node owns a local passkey that is included in the ransmission and checked on the recepient side.
Proposed scenario is as follows:

- `beethowen_transmitter` client is looking for a server with find_server message using its local passkey of 0x1234

- `beethowen_receiver` server receives the command, validates the passkey against the expected passkey and answers only if it matches the passkey specified for the selected mac_address client 

- `beethowen_receiver` server answers with a server found response with its local passkey of 0x4567

- `beethowen_transmitter` client receives the command, validates the passkey against the expected passkey and answers only if it matches the passkey specified.
  As it matches it accepts the server as the valid recepient for the sensing data.

- `beethowen_transmitter` client sends sensor data using its local passkey of 0x1234

- `beethowen_receiver` server receives the sensor data, validates the passkey against the expected passkey and accepts only if it matches the passkey specified for the selected mac_address client.
  As it matches it accepts the sensor data as the valid recepient for the sensing data.



.. _beethowen-sensor:

Sensor
------

The ``beethowen transmitter sensor`` allows you use a sensor to collect and transfer data to a remote 
Beethowen receiver hub and are listed below the hub.

The beethowen sensor component acts as a proxy sensor that receives data from other pyhsical sensors.

.. code-block:: yaml

  beethowen_transmitter:
    sensors:
      - measurement_type: temperature
        sensor_id: bmp085_temperature_sensor
      - measurement_type: pressure
        sensor_id: bmp085_pressure_sensor

Configuration variables:

- **measurement_type** (*Required*, int **or** string): Measurement type as defined in 
  `BTHome format specification <https://bthome.io/format>`__ either as a string or a numeric value.
    
  Measurement type `further details <bthome_common_format.rst>`__ to be taken into account.

- **sensor_id** (*Required*, string): A sensor that is defined in the configuration. Its value will 
  be retrieved and proxied through the beethowen channel.

Sensor Automation
-----------------

on_send_finished
~~~~~~~~~~~~~~~~
This automation will be triggered when a transmission is finished though the beethowen channel.
In Lambdas you can get the result and check if there are any outstanding measurements not yet transferred 
with *has_outstanding_measurements*.

on_send_failed
~~~~~~~~~~~~~~~~
This automation will be triggered when a transmission is failed though the beethowen channel.

Actions
-------

send
~~~~
This action collects all connected sensor data and sends them thorugh the beethowen channel.

Configuration variables:
************************

- **complete_only** (*Optional*, bool): Whether to send any available measurements or start sending only if all sensors have valid measurements. Defaults to `true`.

See Also
--------

- `BTHome <https://bthome.io>`__ by Ernst Klamer, Victor, Paulus Schoutsen.
- `ESP_NOW <https://www.espressif.com/en/solutions/low-power-solutions/esp-now>`__ by Espressif Systems.
- `Passive BLE Monitor integration <https://github.com/custom-components/ble_monitor>`__.
