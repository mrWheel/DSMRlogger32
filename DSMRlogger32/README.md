# DSMRlogger32
Firmware for the DSMR-logger Rev. 5.0 using only API call's

[Here](https://willem.aandewiel.nl/index.php/2022/11/15/crisis-what-crisis/),
    [here](https://willem.aandewiel.nl/index.php/2019/04/09/dsmr-logger-v4-slimme-meter-uitlezer/) and
    [here](https://willem.aandewiel.nl/index.php/2020/02/28/restapis-zijn-hip-nieuwe-firmware-voor-de-dsmr-logger/)
          you can find information about this project.

Documentation can be found [here](https://mrwheel-docs.gitbook.io/dsmrlogger32/) (in progress)!

<table>
  <tr><th>Versie</th><th align="Left">Opmerking</th></tr>
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
