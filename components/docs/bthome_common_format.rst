BTHome format
=============

**BTHome**
  is an energy efficient but flexible format for devices to broadcast their sensor data and button presses 
  originally developed for BLE (Bluetooth Low Energy) devices. Devices can run over a year on a single battery.
  It allows data encryption and is supported by popular home automation platforms, 
  like `Home Assistant <https://www.home-assistant.io>`__, out of the box.

Measurement type
================
Measurement type as defined in `BTHome format specification <https://bthome.io/format>`__ either as a string or a numeric value.

Same property name for multiple object ids
-------------------------------------------

The specification assigns a 16-bit hexadecimal `Object id` (referred as `measurement_type`) to a `Property` name.
In some cases several different measurement types share the same property name.

The package resulves this situation by generating suffixes based on the generic logic:

* base property is used for lowest object id of the property - e.g. `temperature` = 0x02
* if there is a differentiator it is used - e.g. `volume_mL`
* otherwise factor is used relative to the base if possible `_coarse`` or `_precise`` - e.g. `temperature` = 0x02 and `temperature_coarse` = 0x45.
* last resort is use number of bytes used `count` = 0x09 (1 byte), `count_2` = 0x3d (2 bytes), `count_4` = 0x3e (4 bytes)

Most interesting case is (`volume`, `volume_precise`, `volume_mL`).

Multiple sensors of same type
-----------------------------

Multiple sensors of same type is supported by the specification, however at that point order or sensors plays an important role.
Both transmitter and receiver should list the sensors in the correct order. Transmitter should send packages with all sensors completed.

List of Measurement Types
-------------------------

.. include:: ./bthome_common_format_generated.rst
