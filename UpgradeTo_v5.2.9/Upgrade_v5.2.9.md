# Upgrade DSMRlogger32 naar v5.2.9

Het programma `Upgrade_v5.2.9.py` wordt gebruikt om de firmware en het bestandssysteem van de DSMRlogger32 te *upgraden* naar **v5.2.9**.

## Waarom is dit nodig?

Vanaf **v5.2.9** (dat in feite een *dummy* versie is ter voorbereiding op **v5.3.0**) is het mogelijk om firmware-updates rechtstreeks van een **Remote Firmware Server** te verkrijgen. 

Dit heeft als voordeel dat je zelf geen ontwikkelomgeving hoeft op te zetten. De maintainer van de DSMRlogger32 zal de firmware-updates op zijn remote server plaatsen, waarna je jouw DSMRlogger32 eenvoudig kunt updaten.

Helaas zijn de firmwareversies van vóór v5.2.9 niet geschikt voor deze functionaliteit. Daarom moet je eenmalig de stappen in deze handleiding volgen.

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
     python3 Upgrade_v5.2.9.py <COM poort>
     ```
     Vervang `<COM poort>` door de poort waarop de DSMRlogger32 is (of wordt) aangesloten.

     De uitvoer zou ongeveer als volgt moeten zijn:
     ```plaintext
     python3 Upgrade_v5.2.9.py /dev/cu.usbserial-3224142
     2024-09-25 14:12:10,915 - DSMRlogger32_Flasher - INFO - Starting DSMRlogger32 flashing process on port: /dev/cu.usbserial-3224142
     ...
     2024-09-25 14:12:20,155 - DSMRlogger32_Flasher - ERROR - Error flashing DSMRlogger32: Command '...' returned non-zero exit status 2.
     2024-09-25 14:12:20,155 - DSMRlogger32_Flasher - INFO - DSMRlogger32 flashing process completed.
     ```
     Het proces stopt hier omdat er geen communicatie met de DSMRlogger32 is.

   - Sluit nu de DSMRlogger32 aan op de gebruikte seriële poort.
   - Voer opnieuw het volgende commando in:
     ```bash
     python3 Upgrade_v5.2.9.py <COM poort>
     ```
     De uitvoer zou er nu als volgt uit moeten zien:
     ```plaintext
     python3 Upgrade_v5.2.9.py /dev/cu.usbserial-3224142
     2024-09-25 14:05:13,228 - DSMRlogger32_Flasher - INFO - Starting DSMRlogger32 flashing process on port: /dev/cu.usbserial-3224142
     ...
     2024-09-25 14:06:06,058 - DSMRlogger32_Flasher - INFO - DSMRlogger32 flashed successfully!
     2024-09-25 14:06:06,058 - DSMRlogger32_Flasher - INFO - DSMRlogger32 flashing process completed.
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
