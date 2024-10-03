# DSMRlogger32
Firmware for the DSMR-logger Rev. 5.0 using only API call's

[Here](https://willem.aandewiel.nl/index.php/2022/11/15/crisis-what-crisis/),
    [here](https://willem.aandewiel.nl/index.php/2019/04/09/dsmr-logger-v4-slimme-meter-uitlezer/) and
    [here](https://willem.aandewiel.nl/index.php/2020/02/28/restapis-zijn-hip-nieuwe-firmware-voor-de-dsmr-logger/)
          you can find information about this project.

Documentation can be found [here](https://mrwheel-docs.gitbook.io/dsmrlogger32/) (in progress)!

### Ported to PlatformIO
As of now the DSMRlogger32 firmware is ported to `VSCode` with the `PlatformIO` extension.
<p>

<table>
  <tr><th>Versie</th><th align="Left">Opmerking</th></tr>
  <tr>
    <td valign="top">5.4.0</td>
    <td>Nieuwe functionaliteit
        <br>2024-10-03: Met deze update is het niet alleen mogelijk om firmware updates
        van de Remote Server te halen en te flashen maar ook het SPIFFS bestandsysteem
        kan nu via [Update Remote] worden geflashed.<br>
        Er is een nieuw soort presentatie met horizontale 'bars'. Dit is een voortborduring
        op de Gauges van HJM. Deze voldoen helaas niet bij installaties met zonnepanelen
        vandaar deze nieuwe functionaliteit. Je kunt deze zien door op het icoontje
        <big>&#9776</big> te klikken.<br>
        Om deze nieuwe update te kunnen gebruiken moet je éérst de firmware updaten en
        daarná het SPIFFS bestand systeem. Vergeet niet om je RING en `.json` bestanden
        te downloaden en na de update weer terug te zetten.
    </td>
  </tr>
  <tr>
    <td valign="top">5.3.0</td>
    <td>Nieuwe functionaliteit
        <br>2024-09-20: Firmware updates kunnen nu ook vanaf een remote server worden
        opgehaald. Als de functie wordt opgestart ([FSmanager]->[Update Remote])
        verschijnt er een scherm met een drop-down lijst met beschikbare updates.<br>
        Selecteer de gewenste update en klik op [Update] om de update uit te voeren
        of op [Return] om terug te keren naar de FSmanager.<br>
        Om deze nieuwe functionaliteit te kunnen gebruiken moet éénmalig de nieuwe
        firmware (versie 5.3.0) naar je lokale computer worden gedownload en daarna
        met [Update Local] worden geflashed.<br>
        Om deze functie te kunnen gebruiken moet, behalve de firmware update, ook
        het bestand `FSmanager.html` (te vinden op `github`) vanuit de data folder 
        worden gedownload naar je lokale computer en daarna met de FSmanager worden
         ge-[Upload].<br>
        Het uploaden van SPIFFS is helaas nog niet mogelijk.
    </td>
  </tr>
  <tr>
    <td valign="top">5.2.1</td>
    <td>Nieuwe functionaliteit
        <br>2024-09-16: Op een GPIO-pin kan (bijvoorbeeld) een relays worden aangesloten.
        Via de [Instellingen] -> [Systeem] kun je nu opgeven op welke GPIO-pin
        dit relays is aangesloten en bij welke waarden dit relays "Aan" of "Uit" moet 
        gaan. Om niet te snel na het "Uit" schakelen van het relays deze weer "Aan" 
        te zetten kan er een Hysterese Tijd worden ingesteld.<br>
        Ook kan de logica van de aansturing van het Relays ge-inverteerd
        worden (om te voorkomen dat het relays in rust, de "Uit stand", juist
        bekrachtigd wordt). Sommige relays modules maken gebruik van deze 
        inversed logic.
    </td>
  </tr>
  <tr>
    <td valign="top">5.0.5</td>
    <td>Nieuwe functionaliteit
        <br>24-04-2023: Uitbreiding "Actual Tabel" naar 500 punten.
        Hiervoor is ook DSMRgraphics.js aangepast
        <br>Het is nu mogelijk de DSMR-logger32 in zgn. "Access Point"
        mode op te starten. De DSMR-logger32 wordt dan zelf een Access
        Point waar je je computer of tablet aan kunt koppelen.
        <br>De GUI en telnet zijn daarna bereikbaar op IP Adres "192.168.4.1" of
        URL "DMSR-ESP32.local".
    </td>
  </tr>
  <tr>
    <td valign="top">5.0.4</td>
    <td>First Production Release
        <br>De API is aangepast en uitvoer ervan zal verschillen van die in de 
        DSMRloggerAPI firmware (Rev. 3).
    </td>
  </tr>
  <tr>
    <td valign="top">5.0.3</td>
    <td>Release Candidate
        <br>Mindergas functionaliteit is weggehaald, want hoort niet thuis in deze firmware.
        Je kunt eenvoudig met een los esp8266 bordje de gegevens uit deze DSMR-logger halen en
        naar 'mindergas' sturen.
        <br>De API is aangepast en uitvoer ervan zal verschillen van die in de 
        DSMRloggerAPI firmware (Rev. 3).
    </td>
  </tr>
  <tr>
    <td valign="top">5.0.0</td>
    <td>Not for production!
        <br>Deze firmware is bij-lange-na nog niet compleet of zelf "werkend"
        <br>Mindergas functionaliteit is weggehaald, want hoort niet thuis in deze firmware.
        Je kunt eenvoudig met een los esp8266 bordje de gegevens uit deze DSMR-logger halen en
        naar 'mindergas' sturen.
        <br>Waarschijnlijk wordt de API aangepast en zal dus verschillen van die in de 
        DSMRloggerAPI firmware (Rev. 3).
    </td>
  </tr>
</table>
