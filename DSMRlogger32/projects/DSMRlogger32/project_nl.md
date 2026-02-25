# DSMRlogger32

Dit is een verbeterde en modernere versie van mijn eerdere DSMR-loggers (energiemeteruitlezers voor slimme meters).
Door de energiecrisis is het belangrijker geworden om het energieverbruik goed bij te houden. Daarom heb ik na bijna vier jaar een nieuwe versie ontwikkeld.

De DSMRlogger32 biedt dezelfde basisfunctionaliteit als de vorige versie, maar werkt betrouwbaarder en beschikt over beter geheugenbeheer dankzij de ESP32 met PSRAM-module. Sommige functies uit eerdere versies zijn verwijderd (zoals integratie met mindergas.nl en een ADC-interface), omdat deze als niet-essentieel werden beschouwd.

Nieuwe kenmerken zijn onder andere de mogelijkheid om een shield met extra hardware aan te sluiten (bijvoorbeeld Modbus of relais), en een secundaire P1-poort om een extra uitlezer aan te sluiten. Daarnaast is een hardware-watchdog toegevoegd om vastlopen te voorkomen.

Het project omvat zowel hardwareontwerp als firmware, met verbeterde ondersteuning voor JSON-data en een stabielere loggingervaring. Er is de mogelijkheid om PCB’s of complete sets te verkrijgen.

Let op!
Je kunt hier de firmware voor de ESP32 flashen, maar als je het volledige project zelf bouwt, houd er dan rekening mee dat je de ATtiny85 (watchdog) ook zelf moet flashen.
