BTHome and Beethowen Library
============================

For the main components of **BTHome Receiver Hub**, **Beethowen Receiver Hub**, **Beethowen Transmitter Hub** a number of generalized funcionalities were implemented separately.

In the future they might be moved to individual libraries to support generic Arduino development, however this was not in focus now.

The libraries are completely separated from ESPHome, and contain no dependencies for the sake of architectural beauty.

BTHome library
--------------

BTHome library, currently implemented as ``bthome_base`` component contains key BTHome functionalities

* BTHome V1 and V2 package assembly and encoding in ``bthome_encoder.cpp``
* BTHome V1 and V2 package decoding and parsing in ``bthome_parser.cpp``

Beethowen library
-----------------

Beethowen library, currently implemented as ``beethowen_base`` component contains key Beethowen functionalities

* ESP-NOW connection with basic Beethowen package formatting ``meshrc_bthome_over_espnow.cpp``
* BTHome V1 and V2 package decoding and parsing in ``bthome_parser.cpp``
