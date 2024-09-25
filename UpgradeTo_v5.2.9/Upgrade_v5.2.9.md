# Upgrade DSMRlogger32 v5.0.n

Het programma `Upgrade_v5.2.9.py` dient om  de firmware en het bestandssysteem van de DSMRlogger32 te *Upgraden* naar **v5.2.9**.

## Waarom wil je dat?

Vanaf **v5.2.9** (wat eigenlijk een *dummy* versie is als voorbereiding op **v5.3.0**) is het mogelijk
om firmware updates op te halen vanaf een **Remote Firmware Server**.

Het voordeel hiervan is dat je zelf geen ontwikkel omgeving hoeft op te tuigen. De maintainer van
de DSMRlogger32 zal updates van de firmware op zijn remote server zetten waarna je jouw
DSMRlogger32 eenvoudig kunt updaten.

Helaas zijn de firmware versies van vóór v5.2.9 niet geschikt om hier gebruik van de maken. Vandaar
dat je éénmalig de hier beschreven handelingen moet uitvoeren.

## Hoe doe je dat?

Download een **`.zip`** bestand van deze repository naar je computer of **`clone`** de repository met het volgende commando:
```
   git clone https://github.com/mrWheel/DSMRlogger32.git
```
Alleen de folder `UpgradeTo_v5.2.9`is van belang. 

## voer nu de volgende stappen één voor één en in deze volgorde uit
*  Start de FSmanager (het icoontje van het *Filing Cabinet*)
*  Download de drie `RING-bestanden` naar je computer.
*  Download eventueel het `DSMRdevSettings.json` bestand naar je computer.
*  Sluit de DSMRlogger32 nog even niet aan op je computer.
*  Start een ***Command Window***
*  Zorg dat je in de map `UpgradeTo_v5.2.9` staat (`cd`)
*  Nu toets je in:
```
           python3 Upgrade_v5.2.9.py <COM poort>
```
<space>   Je moet &lt;COM poort&gt; uiteraard vervangen door de poort waar de DSMRlogger32 op is (wordt) aangesloten)

<space>    De uitvoer die je nu ziet moet er ongeveer zo uitzien:
```
      python3 Upgrade_v5.2.9.py /dev/cu.usbserial-3224142
      2024-09-25 14:12:10,915 - DSMRlogger32_Flasher - INFO - Starting DSMRlogger32 flashing process on port: /dev/cu.usbserial-3224142
      2024-09-25 14:12:10,916 - DSMRlogger32_Flasher - INFO - Creating virtual environment...
      2024-09-25 14:12:15,279 - DSMRlogger32_Flasher - INFO - Installing required packages...
      Collecting esptool
        Using cached esptool-4.8.0-py3-none-any.whl.metadata (3.5 kB)
      Collecting pyserial
        Using cached pyserial-3.5-py2.py3-none-any.whl.metadata (1.6 kB)
      Collecting bitstring!=4.2.0,>=3.1.6 (from esptool)
        Using cached bitstring-4.2.3-py3-none-any.whl.metadata (5.0 kB)
      .
      .
      Collecting six>=1.9.0 (from ecdsa>=0.16.0->esptool)
        Using cached six-1.16.0-py2.py3-none-any.whl.metadata (1.8 kB)
      Collecting pycparser (from cffi>=1.12->cryptography>=2.1.4->esptool)
        Using cached pycparser-2.22-py3-none-any.whl.metadata (943 bytes)
      Using cached esptool-4.8.0-py3-none-any.whl (534 kB)
      Using cached pyserial-3.5-py2.py3-none-any.whl (90 kB)
      .
      .
      Using cached cffi-1.17.1-cp312-cp312-macosx_10_9_x86_64.whl (183 kB)
      Using cached six-1.16.0-py2.py3-none-any.whl (11 kB)
      Using cached pycparser-2.22-py3-none-any.whl (117 kB)
      Installing collected packages: reedsolo, pyserial, intelhex, bitarray, six, PyYAML, pycparser, bitstring, argcomplete, ecdsa, cffi, cryptography, esptool
      Successfully installed PyYAML-6.0.2 argcomplete-3.5.0 bitarray-2.9.2 bitstring-4.2.3 cffi-1.17.1 cryptography-43.0.1 ecdsa-0.19.0 esptool-4.8.0 intelhex-2.3.0 pycparser-2.22 pyserial-3.5 reedsolo-1.7.0 six-1.16.0
      
      [notice] A new release of pip is available: 24.0 -> 24.2
      [notice] To update, run: /Users/WillemA/tmp/UpgradeTo_v5.2.9/venv/bin/python3.12 -m pip install --upgrade pip
      2024-09-25 14:12:19,796 - DSMRlogger32_Flasher - INFO - Working directory: /Users/WillemA/tmp/UpgradeTo_v5.2.9
      2024-09-25 14:12:19,796 - DSMRlogger32_Flasher - INFO - Executing esptool with arguments: /Users/WillemA/tmp/UpgradeTo_v5.2.9/venv/bin/python -m esptool --chip esp32 --port /dev/cu.usbserial-3224142 --baud 230400 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 firmware_v5.2.9.bin 0x310000 spiffs_v5.2.9.bin
      esptool.py v4.5.1
      Serial port /dev/cu.usbserial-3224142
      
      A fatal error occurred: Could not open /dev/cu.usbserial-3224142, the port doesn't exist
      2024-09-25 14:12:20,155 - DSMRlogger32_Flasher - ERROR - Error flashing DSMRlogger32: Command '['/Users/WillemA/tmp/UpgradeTo_v5.2.9/venv/bin/python', '-m', 'esptool', '--chip', 'esp32', '--port', '/dev/cu.usbserial-3224142', '--baud', '230400', '--before', 'default_reset', '--after', 'hard_reset', 'write_flash', '-z', '--flash_mode', 'dio', '--flash_freq', '80m', '--flash_size', '4MB', '0x1000', 'bootloader.bin', '0x8000', 'partitions.bin', '0x10000', 'firmware_v5.2.9.bin', '0x310000', 'spiffs_v5.2.9.bin']' returned non-zero exit status 2.
      2024-09-25 14:12:20,155 - DSMRlogger32_Flasher - INFO - DSMRlogger32 flashing process completed.
```
<space>     Het process loopt op het einde vast omdat de communicatie met de DSMRlogger32 er niet is.
*  Sluit nu de DSMRlogger32 aan op de gebruikte seriele poort
*  Toets weer het volgende commando in:
```
           python3 Upgrade_v5.2.9.py <COM poort>
```
<space>    De uitvoer moet er nu zo uitzien:
```
      python3 Upgrade_v5.2.9.py /dev/cu.usbserial-3224142
      2024-09-25 14:05:13,228 - DSMRlogger32_Flasher - INFO - Starting DSMRlogger32 flashing process on port: /dev/cu.usbserial-3224142
      2024-09-25 14:05:13,229 - DSMRlogger32_Flasher - INFO - Working directory: /Users/WillemA/tmp/UpgradeTo_v5.2.9
      2024-09-25 14:05:13,229 - DSMRlogger32_Flasher - INFO - Executing esptool with arguments: /Users/WillemA/tmp/UpgradeTo_v5.2.9/venv/bin/python -m esptool --chip esp32 --port /dev/cu.usbserial-3224142 --baud 230400 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 firmware_v5.2.9.bin 0x310000 spiffs_v5.2.9.bin
      esptool.py v4.5.1
      Serial port /dev/cu.usbserial-3224142
      Connecting....
      Chip is ESP32-D0WD-V3 (revision v3.0)
      Features: WiFi, BT, Dual Core, 240MHz, VRef calibration in efuse, Coding Scheme None
      Crystal is 40MHz
      MAC: 8c:4b:14:81:ff:e8
      Uploading stub...
      Running stub...
      Stub running...
      Changing baud rate to 230400
      Changed.
      Configuring flash size...
      Flash will be erased from 0x00001000 to 0x00005fff...
      Flash will be erased from 0x00008000 to 0x00008fff...
      Flash will be erased from 0x00010000 to 0x0016efff...
      Flash will be erased from 0x00310000 to 0x0038ffff...
      Compressed 18992 bytes to 13110...
      Wrote 18992 bytes (13110 compressed) at 0x00001000 in 0.9 seconds (effective 167.8 kbit/s)...
      Hash of data verified.
      Compressed 3072 bytes to 128...
      Wrote 3072 bytes (128 compressed) at 0x00008000 in 0.1 seconds (effective 418.9 kbit/s)...
      Hash of data verified.
      Compressed 1436192 bytes to 884738...
      Wrote 1436192 bytes (884738 compressed) at 0x00010000 in 43.4 seconds (effective 265.0 kbit/s)...
      Hash of data verified.
      Compressed 524288 bytes to 48967...
      Wrote 524288 bytes (48967 compressed) at 0x00310000 in 4.1 seconds (effective 1011.7 kbit/s)...
      Hash of data verified.
      
      Leaving...
      Hard resetting via RTS pin...
      2024-09-25 14:06:06,058 - DSMRlogger32_Flasher - INFO - DSMRlogger32 flashed successfully!
      2024-09-25 14:06:06,058 - DSMRlogger32_Flasher - INFO - DSMRlogger32 flashing process completed.
```
<space>      De DSMRlogger32 zal nu opnieuw opstarten met de nieuwe Firmware!
*  Upload de drie bewaarde `RING-bestanden`
*  Upload eventueel het `DSMRdevSettings.json` bestand
*  ReBoot de DSMRlogger32

In de GUI kun je zien dat de Fimware v5.2.9 aktief is.

![Screenshot 2024-09-25 at 15 28 02](https://github.com/user-attachments/assets/ee61d6a7-fb1d-4bea-9db8-522707e0b86c)

Om de (nieuwe) functionaliteit te testen ga je weer naar de FSmanager

![UpdateRemote_Button](https://github.com/user-attachments/assets/728f35e3-c8d9-41c7-a2c1-9ad37167232e)

Klik nu op de knop [Update Remote]<br>
In het scherm dat nu verschijnt selecteer je de gewenste versie van de firmware 

![UpdateRemote](https://github.com/user-attachments/assets/7d34b2e0-8e21-4627-a104-1c31d218b7fe)

(in dit geval **v5.3.0**) waarna je op [Update] klikt.

Wacht nu af tot de DSMRlogger32 geReboot is.

![Wait4UpdateComplete](https://github.com/user-attachments/assets/1796d7c6-e05b-4290-b7aa-663da387dbd5)

Kijk in de GUI of de juiste firmware nu gebruikt wordt.
