# Upgrade DSMRlogger32 naar v5.2.9

Het programma `Upgrade_v5.2.9.py` wordt gebruikt om de firmware en het bestandssysteem van de DSMRlogger32 te *upgraden* naar **v5.2.9**.

## Waarom is dit nodig?

Vanaf **v5.2.9** (dat in feite een *dummy* versie is ter voorbereiding op **v5.3.0**) is het mogelijk om firmware-updates rechtstreeks van een **Remote Firmware Server** te verkrijgen. 

Dit heeft als voordeel dat je zelf geen ontwikkelomgeving hoeft op te zetten. De maintainer van de DSMRlogger32 zal de firmware-updates op zijn remote server plaatsen, waarna je jouw DSMRlogger32 eenvoudig kunt updaten.

Helaas zijn de firmwareversies van vóór v5.2.9 niet geschikt voor deze functionaliteit. Het is alleen mogelijk om de upgrade ***bedraad*** uit te voeren.
Vervolgens moet je eenmalig de stappen in deze handleiding volgen.

## Wat heb je nodig ##

Voor de upgrade heb je het programma `Upgrade_v5.2.9.py` nodig, dat is geschreven in Python. Als Python nog niet op je computer is geïnstalleerd, kun je het downloaden [via deze link](https://www.python.org/downloads/).

Daarnaast heb je een **FTDI**- of een **USB naar seriële**-kabel nodig om de DSMRlogger32 op je computer aan te sluiten. Er zijn verschillende kabels beschikbaar en de prijzen kunnen sterk variëren. Het is daarom verstandig om even onderzoek te doen naar welke kabel voor jou het meest geschikt is.

**Let er wél op dat het een bordje of kabel is voor 3.3 volt (3v3) anders kun je de DSMRlogger32 beschadigen.**
<table>
  <tr>
    <td><img src="https://github.com/user-attachments/assets/d6805e39-ade9-483e-86ab-36b01dbc8c08" alt="Screenshot" width="300" /></td>
    <td>FTDI kabel<br>Let erop dat de aansluitingen overeenkomen met die van de DSMRlogger32</td>
  </tr>
  <tr>
    <td><img src="https://github.com/user-attachments/assets/e75c3f27-0207-47ec-b9bc-96043f2fdd75" alt="Kabel2" width="300" /> </td>
   <td>USB naar TTL-Seriële kabel<br>Vcc hoeft niet te worden aangesloten<br>Alleen GND, TX en RX aansluiten</td>
  </tr>
  <tr>
    <td><img src="https://github.com/user-attachments/assets/a1290a11-28c9-411c-b0a3-dfec98c60c2c" alt="Kabel3" width="300" /> </td>
    <td>FTDI bordje<br>Let erop dat de aansluitingen overeenkomen met die van de DSMRlogger32</td>
  </tr>
</table>

Een FTDI **3v3** kabel of bordje is aanbevolen. Hiermee wordt de DSMRlogger32 automatisch in de juiste flash mode gezet door het programma `Upgrade_v5.2.9.py`. Als je een *FTDI*- of *USB naar TTL*-kabel gebruikt die dit niet automatisch doet, zoals *USB naar TTL*-kabels met drie of vier aansluitingen, kun je de volgende foutmelding krijgen:
```
DSMRlogger32_Flasher - INFO - Starting DSMRlogger32 flashing process on port: <COM PORT>>
DSMRlogger32_Flasher - INFO - Working directory: /Users/<you>/DSMRlogger32/UpgradeTo_v5.2.9
DSMRlogger32_Flasher - INFO - Executing esptool with arguments: /Users/<you>/esptool.py v4.5.1
Serial port <COM PORT>
Connecting......................................
A fatal error occurred: Failed to connect to ESP32: No serial data received.
For troubleshooting steps visit: https://docs.espressif.com/projects/esptool/en/latest/troubleshooting.html
returned non-zero exit status 2.
DSMRlogger32_Flasher - ERROR - Error flashing DSMRlogger32
DSMRlogger32_Flasher - INFO - DSMRlogger32 flashing process completed.
```
 Als je deze foutmelding krijgt, moet je handmatig de flash mode activeren door de knoppen op de DSMRlogger32 als volgt te gebruiken, voordat je het programma start:

1.  Druk op de [Flash] knop en hou deze ingedrukt
2.  Druk nu op de [Reset] knop en laat deze weer los
3.  Laat nu de [Flash] knop los.

Na deze stappen zou het programma de DSMRlogger32 moeten herkennen en starten met de firmware-upgrade.

## Hoe voer je de upgrade uit?

1. **Download het pakket**:
   - Download een **`.zip`** bestand van deze repository naar je computer of gebruik het volgende commando om de repository te *clonen*:
     ```bash
     git clone https://github.com/mrWheel/DSMRlogger32.git
     ```
   - Let op: alleen de map `UpgradeTo_v5.2.9` is relevant voor de upgrade.

2. **Volg deze stappen één voor één en in de juiste volgorde**:
   - Start de FSmanager (het icoon van het *Filing Cabinet*).
   - Download de drie `RING-bestanden` naar je computer.
   - Download eventueel het `DSMRdevSettings.json` bestand naar je computer.
   - Sluit de DSMRlogger32 nog niet aan op je computer.
   - Start een ***Command Window***.
   - Navigeer naar de map `UpgradeTo_v5.2.9` met het commando `cd`.
   - Voer het volgende commando in:
     ```bash
     python3 Upgrade_v5.2.9.py <COM PORT>
     ```
     Vervang `<COM PORT>` door de poort waarop de DSMRlogger32 is (of wordt) aangesloten.

     De uitvoer zou ongeveer als volgt moeten zijn:
```plaintext
python3 Upgrade_v5.2.9.py <COM PORT>
DSMRlogger32_Flasher - INFO - Starting DSMRlogger32 flashing process on port: <COM PORT>
...
DSMRlogger32_Flasher - INFO - Installing required packages...
Collecting esptool
  Downloading esptool-4.8.1.tar.gz (409 kB)
    ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 409.5/409.5 kB 2.1 MB/s eta 0:00:00
  Installing build dependencies ... done
  Getting requirements to build wheel ... done
  Preparing metadata (pyproject.toml) ... done
Collecting pyserial
  Using cached pyserial-3.5-py2.py3-none-any.whl.metadata (1.6 kB)
...
Using cached six-1.16.0-py2.py3-none-any.whl (11 kB)
Using cached pycparser-2.22-py3-none-any.whl (117 kB)
Building wheels for collected packages: esptool
  Building wheel for esptool (pyproject.toml) ... done
  Created wheel for esptool: filename=esptool-4.8.1-py3-none-any.whl size=534237 sha256=74561f52b0d9d75887e4777d962133ed23538191d27c84625cfc40554360ffcd
  Stored in directory: /Users/<you>>/Library/Caches/pip/wheels/0e/da/61/96ab0615cc87eb88b681fd147d47ec0c9954e1349e3ce19753
Successfully built esptool
Installing collected packages: reedsolo, pyserial, intelhex, bitarray, six, PyYAML, pycparser, bitstring, argcomplete, ecdsa, cffi, cryptography, esptool
Successfully installed PyYAML-6.0.2 argcomplete-3.5.0 bitarray-2.9.2 bitstring-4.2.3 cffi-1.17.1 cryptography-43.0.1 ecdsa-0.19.0 esptool-4.8.1 intelhex-2.3.0 pycparser-2.22 pyserial-3.5 reedsolo-1.7.0 six-1.16.0
...
2024-09-25 14:12:20,155 - DSMRlogger32_Flasher - ERROR - Error flashing DSMRlogger32: Command '...' returned non-zero exit status 2.
2024-09-25 14:12:20,155 - DSMRlogger32_Flasher - INFO - DSMRlogger32 flashing process completed.
```
     Het proces stopt hier omdat er geen communicatie met de DSMRlogger32 is.

   - Sluit nu de DSMRlogger32 aan op de gebruikte seriële poort.
   - Voer opnieuw het volgende commando in:
     ```bash
     python3 Upgrade_v5.2.9.py <COM PORT>
     ```
     De uitvoer zou er nu als volgt uit moeten zien:
```plaintext
python3 Upgrade_v5.2.9.py <COM PORT>
DSMRlogger32_Flasher - INFO - Starting DSMRlogger32 flashing process on port: <COM PORT>
...
DSMRlogger32_Flasher - INFO - Executing esptool with arguments: /Users/<you>/DSMRlogger32/UpgradeTo_v5.2.9/venv/bin/python -m esptool --chip esp32 --port <COM PORT> --baud 230400 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x1000 /Users/<you>/DSMRlogger32/UpgradeTo_v5.2.9/bootloader.bin 0x8000 /Users/<you>/DSMRlogger32/UpgradeTo_v5.2.9/partitions.bin 0x10000 /Users/<you>/DSMRlogger32/UpgradeTo_v5.2.9/firmware_v5.2.9.bin 0x310000 /Users/<you>/DSMRlogger32/UpgradeTo_v5.2.9/spiffs_v5.2.9.bin
esptool.py v4.5.1
Serial port <COM PORT>
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
Wrote 18992 bytes (13110 compressed) at 0x00001000 in 0.9 seconds (effective 166.8 kbit/s)...
Hash of data verified.
Compressed 3072 bytes to 128...
Wrote 3072 bytes (128 compressed) at 0x00008000 in 0.1 seconds (effective 438.1 kbit/s)...
Hash of data verified.
Compressed 1436192 bytes to 884735...
Wrote 1436192 bytes (884735 compressed) at 0x00010000 in 43.4 seconds (effective 265.0 kbit/s)...
Hash of data verified.
Compressed 524288 bytes to 48967...
Wrote 524288 bytes (48967 compressed) at 0x00310000 in 4.1 seconds (effective 1014.0 kbit/s)...
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
DSMRlogger32_Flasher - INFO - DSMRlogger32 flashed successfully!
DSMRlogger32_Flasher - INFO - DSMRlogger32 flashing process completed.
```
     De DSMRlogger32 zal nu opnieuw opstarten met de nieuwe firmware.

   - Upload de drie bewaarde `RING-bestanden`.
   - Upload eventueel het `DSMRdevSettings.json` bestand.
   - Reboot de DSMRlogger32.

## Controleer de nieuwe firmware

In de GUI kun je nu zien dat firmware versie v5.2.9 actief is.

![Screenshot 2024-09-25 at 15 28 02](https://github.com/user-attachments/assets/ee61d6a7-fb1d-4bea-9db8-522707e0b86c)

Om de nieuwe functionaliteit te testen, ga je terug naar de FSmanager.

![UpdateRemote_Button](https://github.com/user-attachments/assets/728f35e3-c8d9-41c7-a2c1-9ad37167232e)

Klik nu op de knop [Update Remote].<br>
In het scherm dat verschijnt, selecteer je de gewenste versie van de firmware.

![UpdateRemote](https://github.com/user-attachments/assets/7d34b2e0-8e21-4627-a104-1c31d218b7fe)

(In dit geval **v5.3.0**), waarna je op [Update] klikt.

Wacht tot de DSMRlogger32 opnieuw is opgestart.

![Wait4UpdateComplete](https://github.com/user-attachments/assets/1796d7c6-e05b-4290-b7aa-663da387dbd5)

Controleer in de GUI of de juiste firmware nu actief is.
