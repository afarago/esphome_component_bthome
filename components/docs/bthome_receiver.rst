ESP32 BTHome Receiver
=====================

**BTHome** is an energy efficient but flexible BLE (Bluetooth Low Energy) format for devices to 
broadcast their sensor data and  button presses. Devices can run over a year on a single battery.
It allows data encryption and is supported by popular home automation platforms, 
like `Home Assistant <https://www.home-assistant.io>`__, out of the box.

This component implements local reception and decoding without the need of a central hub.

Note: as for now this component is limited to V1 and V2 unencryted formats.
Encryption support might be implemented later on.

.. code-block:: yaml

    # Example configuration entry
    external_components:
      - source: github://afarago/esphome_component_bthome
    
    esp32_ble_tracker:

    bthome_receiver:
      dump: unmatched

    sensor:
      - platform: bthome
        mac_address: 11:22:33:44:55:55
        name_prefix: BTHome Living Room
        sensors:
          - measurement_type: temperature
            name: Temperature
          - measurement_type: humidity
            name: Humidity
          - measurement_type: 0x02
            name: Temperature_Second
            accuracy_decimals: 2
            unit_of_measurement: °C

      - platform: bthome
        mac_address: 22:33:44:55:55:66
        sensors:
          - measurement_type: temperature
            name: BTHome Garage Temperature

    binary_sensor:
      - platform: bthome
        mac_address: 11:22:33:44:55:55
        name_prefix: BTHome Living Room
        sensors:
          - measurement_type: opening
            name: BTHome Opening Garage

.. _bthome-component:

Component/Hub
-------------

The ``bthome_receiver`` component creates a global hub so that you can track bluetooth low
energy devices using your ESP32 node over the BTHome protocol using both v1 and v2 protocols.

The component depends on the ``esp32_ble_tracker`` component which needs to be added to the 
configuration.

The bthome receiver component is an internal model that acts as a central reception 
and dispatcher hub to which bthome virtual devices and sensors are connected to.

.. _config-bthome:

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

The ``bthome sensor`` allows you use a sensor to display received measurement from a remote 
BTHome device.
First, you need to define a :ref:`bthome hub component <bthome-component>`.

The bthome sensor component (or "device") is an internal model that acts as a central reception 
and dispatcher hub for a specific remote device identified by a ``mac_address`` to which bthome 
sensors are connected to.

To initialize a sensor, first supply ``mac_address`` to identify the remote BTHome device.

.. code-block:: yaml

    # Example configuration entry
    bthome:

    # Individual sensors
    sensor:
      - platform: bthome
        mac_address: 11:22:33:44:55:55
        name_prefix: BTHome Living Room
        sensors:
          - measurement_type: temperature
            name: Temperature
          - measurement_type: humidity
            name: Humidity
          - measurement_type: 0x02
            name: Temperature_Second
            accuracy_decimals: 2
            unit_of_measurement: °C


.. _config-bthome-sensor:

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
    If selected by name (string) the accuracy and unit of measurement are automatically defaulted to the 
    correct values.

See Also
--------

- `BTHome <https://bthome.io/>`__ by Ernst Klamer, Victor, Paulus Schoutsen.
