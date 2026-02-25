# DSMRlogger32

This is an improved and more modern version of my earlier DSMR loggers (energy meter readers for smart meters).
Due to the energy crisis, it has become more important to monitor energy consumption closely. Therefore, after almost four years, I developed a new version.

The DSMRlogger32 offers the same basic functionality as the previous version, but operates more reliably and features improved memory management thanks to the ESP32 with PSRAM module. Some features from earlier versions have been removed (such as integration with mindergas.nl and an ADC interface), as they were considered non-essential.

New features include the ability to connect a shield with additional hardware (for example Modbus or relays), and a secondary P1 port for connecting an additional reader. In addition, a hardware watchdog has been added to prevent the system from locking up.

The project includes both hardware design and firmware, with improved support for JSON data and a more stable logging experience. PCBs or complete kits are available.

**Note!**
You can flash the firmware for the ESP32 here, but if you build the entire project yourself, please note that the ATtiny85 (watchdog) must also be flashed separately.

