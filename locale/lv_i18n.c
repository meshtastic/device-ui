#include "./lv_i18n.h"


////////////////////////////////////////////////////////////////////////////////
// Define plural operands
// http://unicode.org/reports/tr35/tr35-numbers.html#Operands

// Integer version, simplified

#define UNUSED(x) (void)(x)

static inline uint32_t op_n(int32_t val) { return (uint32_t)(val < 0 ? -val : val); }
static inline uint32_t op_i(uint32_t val) { return val; }
// always zero, when decimal part not exists.
static inline uint32_t op_v(uint32_t val) { UNUSED(val); return 0;}
static inline uint32_t op_w(uint32_t val) { UNUSED(val); return 0; }
static inline uint32_t op_f(uint32_t val) { UNUSED(val); return 0; }
static inline uint32_t op_t(uint32_t val) { UNUSED(val); return 0; }

static uint8_t en_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t v = op_v(n); UNUSED(v);

    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t en_lang = {
    .locale_name = "en",


    .locale_plural_fn = en_plural_fn
};

static lv_i18n_phrase_t de_singulars[] = {
    {"User name: %s", "Benutzer: %s"},
    {"Device Role: %s", "Gerätemodus: %s"},
    {"no new messages", "keine Nachrichten"},
    {"1 of 1 nodes online", "1/1 Gerät online"},
    {"User name: ", "Benutzer: "},
    {"Modem Preset: LONG FAST", "Modem: LONG FAST"},
    {"Channel: LongFast", "Gruppen-Kanäle: LongFast"},
    {"Role: Client", "Gerätemodus: Client"},
    {"Screen Timeout: 60s", "Bild aus: 60s"},
    {"Screen Brightness: 60%", "Bildschirmhelligkeit: 60%"},
    {"Screen Calibration: default", "Kalibrierung: Standard"},
    {"Input Control: none/none", "Eingabe: ohne/ohne"},
    {"Message Alert Buzzer: on", "Akustischer Alarm: ein"},
    {"Language: English", "Sprache: Deutsch"},
    {"Configuration Reset", "Einstellungen zurücksetzen"},
    {"Reboot / Shutdown", "Neustart / Ausschalten"},
    {"Group Channels", "Gruppen Kanäle"},
    {"no messages", "keine Nachrichten"},
    {"Settings", "Einstellungen"},
    {"Settings (advanced)", "Erweiterte Einstellungen"},
    {"Locations Map", "Karte"},
    {"no chats", "Keine Gespräche"},
    {"Node Options", "Optionen"},
    {"Short Name", "Kurzname"},
    {"Long Name", "Langname"},
    {"<unset>", "<leer>"},
    {"FrequencySlot: 1 (902.0MHz)", "Frequenz: 1 (902.0MHz)"},
    {"Brightness: 60%", "Helligkeit: 60%"},
    {"Timeout: 60s", "Bild aus: 60s"},
    {"Mouse", "Maus"},
    {"Keyboard", "Tastatur"},
    {"Message Alert", "Alarm"},
    {"NodeDB Reset\nFactory Reset", "Datenbank Reset\nWerkseinstellungen"},
    {"Channel Name", "Kanalname"},
    {"Pre-shared Key", "Schlüssel"},
    {"New Message from\n", "Neue Nachricht von\n"},
    {"Resynch...", "Synchronisieren..."},
    {"Screen Calibration: %s", "Kalibrierung: %s"},
    {"done", "erledigt"},
    {"default", "Standard"},
    {"Modem Preset: %s", "Voreinstellung: %s"},
    {"Channel: %s", "Gruppen-Kanäle: %s"},
    {"Language: %s", "Sprache: %s"},
    {"Screen Lock: off", "Bildschirmsperre: aus"},
    {"Theme: Dark", "Farbschema: Dunkel"},
    {"Mesh Detector", "Mesh-Detektor"},
    {"Signal Scanner", "Signalstärke"},
    {"Trace Route", "Route verfolgen"},
    {"Neighbors", "Nachbarn"},
    {"Statistics", "Paketstatistik"},
    {"Packet Log", "Paket Historie"},
    {"Settings & Tools", "Einstellungen & Funktionen"},
    {"Node Search", "Gerätesuche"},
    {"Screen Lock", "Bildschirmsperre"},
    {"Lock PIN", "PIN"},
    {"Ringtone", "Klingelton"},
    {"Zone", "Gebiet"},
    {"City", "Stadt"},
    {"Unknown", "Unbekannt"},
    {"Offline", "Abwesend"},
    {"Public Key", "PKC Schlüssel"},
    {"Active Chat", "Aktiver Chat"},
    {"Telemetry", "Telemetrie"},
    {"Theme: %s", "Farbschema: %s"},
    {"Packet Statistics", "Paketstatistik"},
    {"none", "keine"},
    {"Default", "Standard"},
    {"Cancel", "Abbruch"},
    {"FrequencySlot: %d (%g MHz)", "Frequenz: %d (%g MHz)"},
    {"Brightness: %d%%", "Helligkeit: %d%%"},
    {"Timeout: off", "Timeout: aus"},
    {"Stop", "Stopp"},
    {"choose\nnode", "wähle\nNachbar"},
    {"choose target node", "wähle Ziel"},
    {"heard: !%08x", "Gehört: !%08x"},
    {"Packet Log: %d", "Paket Historie: %d"},
    {"Screen Timeout: off", "Display abschalten: nie"},
    {"Screen Timeout: %ds", "Display abschalten: %ds"},
    {"Screen Brightness: %d%%", "Helligkeit: %d%%"},
    {"Screen Lock: %s", "Bildschirmsperre: %s"},
    {"on", "ein"},
    {"off", "aus"},
    {"Message Alert: %s", "Klingelton: %s"},
    {"unknown", "unbekannt"},
    {"<no name>", "<kein Name>"},
    {"%d active chat(s)", "%d aktive Chat(s)"},
    {"New message from \n%s", "Neue Nachricht von \n%s"},
    {"Input Control: %s/%s", "Eingabegeräte: %s/%s"},
    {"%d of %d nodes online", "%d / %d Geräte online"},
    {"Filtering ...", "Filtern ..."},
    {"Filter: %d of %d nodes", "Filter: %d / %d Geräte"},
    {"now", "jetzt"},
    {"%d new message", "%d Nachricht"},
    {"%d new messages", "%d Nachrichten"},
    {"Tools", "Funktionen"},
    {"Highlight", "Hervorheben"},
    {"choose node", "wähle Nachbar"},
    {"Primary Channel", "Primärkanal"},
    {"Secondary Channels", "Sekundärkanal"},
    {"Dark\nLight", "Dunkel\nHell"},
    {"Resynch ...", "Synchronisierung ..."},
    {"Rebooting ...", "Neustart ..."},
    {"Shutting down ...", "Herunterfahren ..."},
    {"silent", "stumm"},
    {"WiFi: <not setup>", "WiFi: <kein Passwort>"},
    {"LoRa TX off!", "LoRa TX ausgeschaltet!"},
    {"WiFi pre-shared Key", "WiFi Passwort"},
    {"<not set>", "<nicht gesetzt>"},
    {"Banner & Sound", "Banner & Ton"},
    {"Banner only", "nur Banner"},
    {"Sound only", "nur Ton"},
    {NULL, NULL} // End mark
};



static uint8_t de_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t v = op_v(n); UNUSED(v);

    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t de_lang = {
    .locale_name = "de",
    .singulars = de_singulars,

    .locale_plural_fn = de_plural_fn
};

static lv_i18n_phrase_t es_singulars[] = {
    {"no new messages", "sin mensajes nuevos"},
    {"1 of 1 nodes online", "1 de 1 nodos activos"},
    {"DEL", "BOR"},
    {"User name: ", "Nomb. Usua:"},
    {"Channel: LongFast", "Canal: LongFast"},
    {"Role: Client", "Rol: Cliente"},
    {"Screen Timeout: 60s", "Apag. Panta: 60s"},
    {"Screen Brightness: 60%", "Brillo Pantalla: 60%"},
    {"Screen Calibration: default", "Calibración Pantalla: predet."},
    {"Message Alert Buzzer: on", "Zumbador en mensaje: on"},
    {"Language: English", "Idioma: Español"},
    {"Configuration Reset", "Reiniciar Configuración"},
    {"Reboot / Shutdown", "Reiniciar / Apagar"},
    {"Group Channels", "Canales Grupales"},
    {"no messages", "sin mensajes"},
    {"Settings", "Configuración"},
    {"Settings (advanced)", "Configur. avanzada"},
    {"Locations Map", "Mapa de localizaciones"},
    {"no chats", "sin chats"},
    {"Node Options", "Opciones de Nodos"},
    {"Short Name", "Nombre Corto"},
    {"Long Name", "Nombre Largo"},
    {"<unset>", "<vacío>"},
    {"Brightness: 60%", "Brillo: 60%"},
    {"Mouse", "Ratón"},
    {"Keyboard", "Teclado"},
    {"Message Alert", "Alertar mensaje"},
    {"NodeDB Reset\nFactory Reset", "Borrar NodeDB\nReini. Fábrica"},
    {"Channel Name", "Nombre Canal"},
    {"Pre-shared Key", "Clave Precomp."},
    {"New Message from\\n", "Nuevo msg de\\n"},
    {"User name: %s", "Nombre: %s"},
    {"Device Role: %s", "Rol Dispo.: %s"},
    {"Screen Calibration: %s", "Calibración pant.: %s"},
    {"done", "hecho"},
    {"default", "defecto"},
    {"Region: %s", "Región: %s"},
    {"Channel: %s", "Canal: %s"},
    {"Language: %s", "Idioma: %s"},
    {"Screen Lock: off", "Bloq. Panta.: off"},
    {"Theme: Dark", "Tema: Oscuro"},
    {"Mesh Detector", "Detector Mesh"},
    {"Signal Scanner", "Escáner de señal"},
    {"Trace Route", "Trazar ruta"},
    {"Neighbors", "Vecinos"},
    {"Statistics", "Estadísticas"},
    {"Packet Log", "Registro de paquetes"},
    {"Settings & Tools", "Config y herramientas"},
    {"Node Search", "Buscar nodo"},
    {"Screen Lock", "Bloqueo Pantalla"},
    {"Lock PIN", "PIN bloq."},
    {"Ringtone", "Timbre"},
    {"Zone", "Zona"},
    {"City", "Ciudad"},
    {"Unknown", "Desconocido"},
    {"Offline", "Inactivo(s)"},
    {"Public Key", "Clave Pública"},
    {"Hops away", "Saltos"},
    {"Position", "Posición"},
    {"Name", "Nombre"},
    {"Active Chat", "Chat Activo"},
    {"Telemetry", "Telemetría"},
    {"Start", "Iniciar"},
    {"Theme: %s", "Tema: %s"},
    {"Packet Statistics", "Estadísticas"},
    {"LONG FAST\nLONG SLOW\n-- deprecated --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO", "LONG FAST\nLONG SLOW\n-- obsoleto --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO"},
    {"Client\nClient Mute\nRouter\n-- deprecated --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker", "Client\nClient Mute\nRouter\n-- obsoleto --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker"},
    {"Dark\nLight", "Oscuro\nClaro"},
    {"Cancel", "Cancelar"},
    {"Brightness: %d%%", "Brillo : %d%%"},
    {"Stop", "Parar"},
    {"choose\nnode", "escoge\nnodo"},
    {"choose target node", "escoge nodo de destino"},
    {"heard: !%08x", "oído: !%08x"},
    {"Packet Log: %d", "Reg. Paquetes: %d"},
    {"Screen Timeout: off", "Apag. Pantalla: off"},
    {"Screen Timeout: %ds", "Apag. Pantalla: %ds"},
    {"Screen Brightness: %d%%", "Brillo pantalla: %d%%"},
    {"Screen Lock: %s", "Bloq. Pantalla: %s"},
    {"Message Alert: %s", "Alertar mensaje: %s"},
    {"Util %0.1f%%  Air %0.1f%%", "Util %0.1f%%  Aire %0.1f%%"},
    {"hops: %d", "saltos: %d"},
    {"unknown", "desconocido"},
    {"<no name>", "<sin nombre>"},
    {"%d active chat(s)", "%d chat(s) activos"},
    {"New message from \n%s", "Nuevo mensaje de:\n%s"},
    {"%d of %d nodes online", "%d de %d nodos activos"},
    {"Filtering ...", "Filtrando ..."},
    {"Filter: %d of %d nodes", "Filtro: %d de %d nodos"},
    {"now", "ahora"},
    {"%d new message", "%d mensaje nuevo"},
    {"%d new messages", "%d mensajes nuevos"},
    {"uptime: %02d:%02d:%02d", "encendido: %02d:%02d:%02d"},
    {"Tools", "Herramientas"},
    {"Filter", "Filtro"},
    {"Highlight", "Resalte"},
    {"Primary Channel", "Canal Primario"},
    {"Secondary Channels", "Canal Secundario"},
    {"Resynch ...", "Resincronizando ..."},
    {"Rebooting ...", "Reiniciando ..."},
    {"Shutting down ...", "Apagando ..."},
    {"silent", "silenciado"},
    {"WiFi: <not setup>", "WiFi: <no configurada>"},
    {"LoRa TX off!", "LoRa TX apagado!"},
    {"WiFi SSID", "Nombre de la WiFi"},
    {"WiFi pre-shared Key", "Clave de la WiFi"},
    {"<not set>", "<vacío>"},
    {"Banner & Sound", "Visual y sonora"},
    {"Banner only", "Solo visual"},
    {"Sound only", "Solo Sonora"},
    {NULL, NULL} // End mark
};



static uint8_t es_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);


    if ((n == 1)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t es_lang = {
    .locale_name = "es",
    .singulars = es_singulars,

    .locale_plural_fn = es_plural_fn
};

static lv_i18n_phrase_t fi_singulars[] = {
    {"no new messages", "ei uusia viestejä"},
    {"1 of 1 nodes online", "1/1 nodea verkossa"},
    {"uptime 00:00:00", "käyttöaika 00:00:00"},
    {"User name: ", "Käyttäjätunnus"},
    {"Modem Preset: LONG FAST", "modeemin esiasetus: LONG FAST"},
    {"Channel: LongFast", "Kanava: LongFast"},
    {"Role: Client", "Rooli: Client"},
    {"Screen Timeout: 60s", "Näytön aikakatkaisu: 60s"},
    {"Screen Lock: off", "Näytön lukitus: pois"},
    {"Screen Brightness: 60%", "Näytön kirklaus: 60%"},
    {"Theme: Dark", "Teema: Tumma"},
    {"Screen Calibration: default", "Näytön kalibrointi: oletus"},
    {"Input Control: none/none", "Tulon ohjaus: ei mitään/ei mitään"},
    {"Message Alert Buzzer: on", "Viestihälytyssummeri: päällä"},
    {"Language: English", "Kieli: Englanti"},
    {"Configuration Reset", "Konfiguraation palautus"},
    {"Reboot / Shutdown", "Käynn. uudest. / Sammuta"},
    {"Mesh Detector", "Mesh verkko tunnistin"},
    {"Signal Scanner", "Signaalin skanneri"},
    {"Trace Route", "Reitin selvitys"},
    {"Neighbors", "Naapurit"},
    {"Statistics", "Tilastot"},
    {"Packet Log", "Pakettiloki"},
    {"Group Channels", "Ryhmä keskustelut"},
    {"no messages", "Ei viestejä"},
    {"Settings & Tools", "Asetukset ja Työkalut"},
    {"Settings (advanced)", "lisäasetukset"},
    {"Locations Map", "Sijaintikartta"},
    {"no chats", "ei keskusteluja"},
    {"Node Search", "Node haku"},
    {"Packet Statistics", "Pakettitilastot"},
    {"Node Options", "noden asetukset"},
    {"Short Name", "Lyhyt Nimi"},
    {"Long Name", "Pitkä Nimi"},
    {"<unset>", "<ei asetettu>"},
    {"Brightness: 60%", "Kirkkaus: 60%"},
    {"Dark\nLight", "Tumma\nVaalea"},
    {"Timeout: 60s", "Aikakatkaisu: 60s"},
    {"Screen Lock", "Näytön lukitus"},
    {"Lock PIN", "Pinkoodi"},
    {"Mouse", "Hiiri"},
    {"none", "ei mikään"},
    {"Keyboard", "Näppäimistö"},
    {"Message Alert", "Viestihälytys"},
    {"Ringtone", "Soittoääni"},
    {"Default", "Oletus"},
    {"Zone", "vyöhyke"},
    {"City", "Kaupunki"},
    {"NodeDB Reset\nFactory Reset", "NodeDB:n nollaus\nTehdasasetusten palautus"},
    {"Channel Name", "Kanavan nimi"},
    {"Pre-shared Key", "Esijaettu avain"},
    {"Unknown", "Tuntematon"},
    {"Public Key", "Julkinen avain"},
    {"Hops away", "hyppyjen päässä"},
    {"Position", "Sijainti"},
    {"Name", "Nimi"},
    {"Active Chat", "Aktiivinen keskustelu"},
    {"Telemetry", "Telemetria"},
    {"Start", "Aloita"},
    {"New Message from\n", "Uusi viesti lähettäjälyä\n"},
    {"Cancel", "Peruuta"},
    {"FrequencySlot: %d (%g MHz)", "Taajuuspaikka: %d (%g MHz)"},
    {"Brightness: %d%%", "Kirkkaus: %d%%"},
    {"Timeout: off", "Aikakatkaisu: pois"},
    {"Timeout: %ds", "Aikakatkaisu: %ds"},
    {"Screen Calibration: %s", "Näytön kalibrointi: %s"},
    {"done", "valmis"},
    {"default", "oletus"},
    {"Stop", "Lopeta"},
    {"choose\nnode", "valitse\nnode"},
    {"choose target node", "valitse kohde node"},
    {"Packet Log: %d", "Pakettiloki: %d"},
    {"Language: %s", "Kieli: %s"},
    {"Screen Timeout: off", "Näytön aikakatkaisu: pois päältä"},
    {"Screen Timeout: %ds", "Näytön aikakatkaisu: %ds"},
    {"Screen Brightness: %d%%", "Näytön kirkkaus: %d%%"},
    {"Theme: %s", "Teema: %s"},
    {"User name: %s", "Käyttäjätunnus: %s"},
    {"Device Role: %s", "Laitteen rooli: %s"},
    {"Region: %s", "Alue: %s"},
    {"Channel: %s", "Kanava: %s"},
    {"Screen Lock: %s", "Näytön lukitus: %s"},
    {"on", "päällä"},
    {"off", "pois"},
    {"Message Alert: %s", "Viestihälytys: %s"},
    {"hops: %d", "hypyt: %d"},
    {"unknown", "tuntematon"},
    {"<no name>", "<ei nimeä>"},
    {"%d active chat(s)", "aktiivisia keskusteluja: %d"},
    {"New message from \n%s", "Uusi viesti lähettäjältä\n%s"},
    {"Input Control: %s/%s", "Tulon ohjaus: %s/%s"},
    {"%d of %d nodes online", "%d/%d nodea verkossa"},
    {"Filtering ...", "Suodatus ..."},
    {"Filter: %d of %d nodes", "Suodatetaan %d/%d nodea"},
    {"now", "nyt"},
    {"%d new message", "%d uusi viesti"},
    {"%d new messages", "%d uutta viestä"},
    {"uptime: %02d:%02d:%02d", "käyttöaika: %02d:%02d:%02d"},
    {"Settings", "Asetukset"},
    {"Tools", "Työkalut"},
    {"Filter", "Suodatin"},
    {"Highlight", "Korosta"},
    {"Primary Channel", "Ensisijainen kanava"},
    {"Secondary Channels", "Toissijaiset kanavat"},
    {"Resynch ...", "Synkronoi uudelleen ..."},
    {"Rebooting ...", "Käynnistetään uudelleen ..."},
    {"Shutting down ...", "Sammutetetaan ..."},
    {NULL, NULL} // End mark
};



static uint8_t fi_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t v = op_v(n); UNUSED(v);

    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t fi_lang = {
    .locale_name = "fi",
    .singulars = fi_singulars,

    .locale_plural_fn = fi_plural_fn
};

static uint8_t fr_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);

    if ((((i == 0) || (i == 1)))) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t fr_lang = {
    .locale_name = "fr",


    .locale_plural_fn = fr_plural_fn
};

static uint8_t it_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t v = op_v(n); UNUSED(v);

    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t it_lang = {
    .locale_name = "it",


    .locale_plural_fn = it_plural_fn
};

static lv_i18n_phrase_t nl_singulars[] = {
    {"no new messages", "0 nieuwe berichten"},
    {"1 of 1 nodes online", "1 van 1 nodes online"},
    {"uptime 00:00:00", "uptime 00:00:00"},
    {"Heap: 0\\nLVGL: 0", "Heap: 0\\nLVGL: 0"},
    {"DEL", "DEL"},
    {"User name: ", "Gebruikersnaam: "},
    {"Modem Preset: LONG FAST", "Modem Preset: LONG FAST"},
    {"Channel: LongFast", "Kanaal: LongFast"},
    {"Role: Client", "Rol: Client"},
    {"Screen Timeout: 60s", "Schermvergrendeling: 60s"},
    {"Screen Lock: off", "Schermvergrendeling: uit"},
    {"Screen Brightness: 60%", "Helderheid: 60%"},
    {"Theme: Dark", "Weergave: Donker"},
    {"Screen Calibration: default", "Schermcalibratie: standaard"},
    {"Input Control: none/none", "Input Control: geen/geen"},
    {"Message Alert Buzzer: on", "Notificatiegeluid: aan"},
    {"Language: English", "Taal: Nederlands"},
    {"Configuration Reset", "Configuratie herstellen"},
    {"Reboot / Shutdown", "Herstarten / Uitschakelen"},
    {"Mesh Detector", "Mesh Detector"},
    {"Signal Scanner", "Signaal Scanner"},
    {"Trace Route", "Trace Route"},
    {"Neighbors", "Buren"},
    {"Statistics", "Statistieken"},
    {"Packet Log", "Packet Log"},
    {"Meshtastic", "Meshtastic"},
    {"Group Channels", "Groepskanalen"},
    {"no messages", "geen berichten"},
    {"Settings & Tools", "Instellingen en Tools"},
    {"Settings (advanced)", "Instellingen (geavanceerd)"},
    {"Locations Map", "Lokaties Kaart"},
    {"no chats", "geen chats"},
    {"Node Search", "Node zoeken"},
    {"Packet Statistics", "Packet Statistieken"},
    {"Node Options", "Node Opties"},
    {"Short Name", "Korte Naam"},
    {"Long Name", "Lange Naam"},
    {"<unset>", "<niet ingesteld>"},
    {"LONG FAST\nLONG SLOW\n-- deprecated --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO", "LONG FAST\nLONG SLOW\n-- deprecated --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO"},
    {"Client\nClient Mute\nRouter\n-- deprecated --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker", "Client\nClient Mute\nRouter\n-- deprecated --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker"},
    {"Brightness: 60%", "Helderheid: 60%"},
    {"Dark\nLight", "Donker\nLicht"},
    {"Timeout: 60s", "Timeout: 60s"},
    {"Screen Lock", "Vergrendeling"},
    {"Lock PIN", "PIN"},
    {"Mouse", "Muis"},
    {"none", "geen"},
    {"Keyboard", "Toetsenbord"},
    {"Message Alert", "Berichtnotificatie"},
    {"Ringtone", "Ringtone"},
    {"Default", "Standaard"},
    {"Zone", "Zone"},
    {"City", "Stad"},
    {"NodeDB Reset\nFactory Reset", "NodeDB herstellen\nFabrieksinstellingen herstellen"},
    {"Channel Name", "Kanaal Naam"},
    {"Pre-shared Key", "Pre-shared Key"},
    {"Unknown", "Onbekend"},
    {"Offline", "Offline"},
    {"Public Key", "Public Key"},
    {"Hops away", "Hops verwijderd"},
    {"MQTT", "MQTT"},
    {"Position", "Positie"},
    {"Name", "Naam"},
    {"Active Chat", "Actieve Chat"},
    {"Telemetry", "Telemetrie"},
    {"IAQ", "IAQ"},
    {"Start", "Start"},
    {"New Message from\n", "Nieuw bericht van\n"},
    {"Resynch...", "Hersync..."},
    {"OK", "OK"},
    {"Cancel", "Annuleer"},
    {"FrequencySlot: %d (%g MHz)", "FrequencySlot: %d (%g MHz)"},
    {"Brightness: %d%%", "Helderheid: %d%%"},
    {"Timeout: off", "Timeout: uit"},
    {"Timeout: %ds", "Timeout: %ds"},
    {"Screen Calibration: %s", "Schermcalibratie: %s"},
    {"done", "gereed"},
    {"default", "standaard"},
    {"Stop", "Stop"},
    {"choose\nnode", "kies\nnode"},
    {"choose target node", "kies doelnode"},
    {"heard: !%08x", "gehoord: !%08x"},
    {"Packet Log: %d", "Packet Log: %d"},
    {"Language: %s", "Taal: %s"},
    {"Screen Timeout: off", "Schermvergrendeling: uit"},
    {"Screen Timeout: %ds", "Schermvergrendeling: %ds"},
    {"Screen Brightness: %d%%", "Helderheid: %d%%"},
    {"Theme: %s", "Weergave: %s"},
    {"User name: %s", "Gebruikersnaam: %s"},
    {"Device Role: %s", "Apparaat Rol: %s"},
    {"Region: %s", "Regio: %s"},
    {"Modem Preset: %s", "Modem Preset: %s"},
    {"Channel: %s", "Kanaal: %s"},
    {"Screen Lock: %s", "Schermvergrendeling: %s"},
    {"on", "aan"},
    {"off", "uit"},
    {"Message Alert: %s", "Bericht Notificatie: %s"},
    {"Util %0.1f%%  Air %0.1f%%", "Util %0.1f%%  Air %0.1f%%"},
    {"hops: %d", "hops: %d"},
    {"unknown", "onbekend"},
    {"<no name>", "<geen naam>"},
    {"%d active chat(s)", "%d actieve chat(s)"},
    {"New message from \n%s", "Nieuw bericht van \n%s"},
    {"Input Control: %s/%s", "Input Control: %s/%s"},
    {"%d of %d nodes online", "%d van %d nodes online"},
    {"Filtering ...", "Filteren ..."},
    {"Filter: %d of %d nodes", "Filter: %d van %d nodes"},
    {"now", "nu"},
    {"%d new message", "%d nieuw bericht"},
    {"%d new messages", "%d nieuwe berichten"},
    {"uptime: %02d:%02d:%02d", "uptime: %02d:%02d:%02d"},
    {"Heap: %d (%d%%)\\nLVGL: %d (%d%%)", "Heap: %d (%d%%)\\nLVGL: %d (%d%%)"},
    {"Settings", "Instellingen"},
    {"Tools", "Tools"},
    {"Filter", "Filter"},
    {"Highlight", "Highlight"},
    {"Primary Channel", "Primair Kanaal"},
    {"Secondary Channels", "Secundaire Kanalen"},
    {"Resynch ...", "Hersync ..."},
    {"Rebooting ...", "Herstarten ..."},
    {"Shutting down ...", "Uitschakelen ..."},
    {NULL, NULL} // End mark
};



static uint8_t nl_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t v = op_v(n); UNUSED(v);

    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t nl_lang = {
    .locale_name = "nl",
    .singulars = nl_singulars,

    .locale_plural_fn = nl_plural_fn
};

static lv_i18n_phrase_t pl_singulars[] = {
    {"no new messages", "Brak wiadomości"},
    {"1 of 1 nodes online", "1 z 1 węzłów online"},
    {"User name: ", "Nazwa"},
    {"Modem Preset: LONG FAST", ""},
    {"Channel: LongFast", "Kanał: LongFast"},
    {"Role: Client", "Tryb: Client"},
    {"Screen Timeout: 60s", ""},
    {"Screen Brightness: 60%", "Jasność ekranu: 60%"},
    {"Screen Calibration: default", "Kalibracja ekranu: domyślna"},
    {"Input Control: none/none", ""},
    {"Language: English", "Język: Angielski"},
    {"Configuration  Reset", "Zresetuj konfigurację"},
    {"Reboot / Shutdown", "Zrestartuj / Wyłącz"},
    {"Group Channels", "Kanały grupowe"},
    {"no messages", "brak wiadomości"},
    {"Settings", "Ustawienia"},
    {"Settings (advanced)", "Ustawienia (zaawansowane)"},
    {"Locations Map", "Mapa miejsc"},
    {"no chats", "brak czatów"},
    {"Node Options", "Ustawienia węzła"},
    {"Short Name", "Krótka nazwa"},
    {"Long Name", "Długa nazwa"},
    {"<unset>", "Nieustawione"},
    {"FrequencySlot: 1 (902.0MHz)", "Kanał częstotliwości: 1 (902.0MHz)"},
    {"Brightness: 60%", "Jasność: 60%"},
    {"Mouse", "Kursor"},
    {"Keyboard", "Klawiatura"},
    {"Message Alert", "Alerty dźwiękowe"},
    {"NodeDB Reset\nFactory Reset", "Reset NodeDB\nReset do ustawień domyślnych"},
    {"Channel Name", "Nazwa kanału"},
    {"New Message from\n", "Nowa wiadomość od\n"},
    {"Resynch ...", "Resynchronizacja..."},
    {"User name: %s", "Nazwa: %s"},
    {"Device Role: %s", "Typ urządzenia: %s"},
    {"Screen Calibration: %s", "Kalibracja ekranu: %s"},
    {"done", "wykonano"},
    {"default", "domyślny"},
    {"Channel: %s", "Kanał: %s"},
    {"Language: %s", "Język: %s"},
    {"Screen Lock: off", "Blokada ekranu: wyłączona"},
    {"Theme: Dark", "Skórka: Ciemny"},
    {"Mesh Detector", "Wykrywanie siatki"},
    {"Signal Scanner", "Skaner sygnału"},
    {"Trace Route", "Śledzenie trasy"},
    {"Neighbors", "Sąsiedzi"},
    {"Statistics", "Statystyki"},
    {"Packet Log", "Dziennik pakietów"},
    {"Settings & Tools", "Ustawienia & Narzędzia"},
    {"Node Search", "Szukanie Węzła"},
    {"Light\\nDark", "Jasny\\nCiemny"},
    {"Screen Lock", "Blokada Ekranu"},
    {"Lock PIN", "Blokada PIN"},
    {"Ringtone", "Dzwonek"},
    {"Zone", "Strefa"},
    {"City", "Miasto"},
    {"Unknown", "Nieznany"},
    {"Public Key", "Klucz publiczny"},
    {"Position", "Pozycja"},
    {"Name", "Nazwa"},
    {"Active Chat", "Aktywny czat"},
    {"Telemetry", "Telemetria"},
    {"Theme: %s", "Skórka: %s"},
    {"uptime 00:00:00", "Aktywny: 00:00:00"},
    {"Packet Statistics", "Statystyki pakietu"},
    {"Dark\nLight", "Ciemny\nJasny"},
    {"none", "brak"},
    {"Default", "Domyślny"},
    {"Cancel", "Anuluj"},
    {"FrequencySlot: %d (%g MHz)", "Kanał: %d (%g MHz)"},
    {"Brightness: %d%%", "Jasność: %d%%"},
    {"choose\nnode", "wybierz\nwęzeł"},
    {"choose target node", "Wybierz adresata"},
    {"heard: !%08x", "Widziano !%08x"},
    {"Packet Log: %d", "Dziennik pakietów: %d"},
    {"Screen Brightness: %d%%", "Jasność ekranu: %d%%"},
    {"Screen Lock: %s", "Blokada ekranu: %s"},
    {"on", "włączony"},
    {"off", "wyłączony"},
    {"Message Alert: %s", "Dzwonek alertu: %s"},
    {"hops: %d", "Skoki: %d"},
    {"unknown", "nieznany"},
    {"<no name>", "<brak nazwy>"},
    {"%d active chat(s)", "%d brak aktywnych czatów"},
    {"New message from \n%s", "Nowa wiadomość od \n%s"},
    {"Input Control: %s/%s", "Kontrola wprowadzania: %s/%s"},
    {"%d of %d nodes online", "%d z %d węzłów online"},
    {"Filtering ...", "Filtrowanie..."},
    {"Filter: %d of %d nodes", "Wyfiltrowano: %d z %d węzłów"},
    {"now", "teraz"},
    {"%d new message", "%d nowa wiadomość"},
    {"%d new messages", "%d nowe wiadomości"},
    {"uptime: %02d:%02d:%02d", "aktywny: %02d:%02d:%02d"},
    {"Configuration Reset", "Reset konfiguracji"},
    {"Tools", "Narzędzia"},
    {"Filter", "Filtruj"},
    {"Highlight", "Podświetl"},
    {"Primary Channel", "Kanał główny"},
    {"Secondary Channels", "Kanały dodatkowe"},
    {"Rebooting ...", "Uruchamiam ponownie ..."},
    {"Shutting down ...", "Wyłączam ..."},
    {NULL, NULL} // End mark
};



static uint8_t pl_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t v = op_v(n); UNUSED(v);
    uint32_t i10 = i % 10;
    uint32_t i100 = i % 100;
    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    if ((v == 0 && (2 <= i10 && i10 <= 4) && (!(12 <= i100 && i100 <= 14)))) return LV_I18N_PLURAL_TYPE_FEW;
    if ((v == 0 && i != 1 && (0 <= i10 && i10 <= 1)) || (v == 0 && (5 <= i10 && i10 <= 9)) || (v == 0 && (12 <= i100 && i100 <= 14))) return LV_I18N_PLURAL_TYPE_MANY;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t pl_lang = {
    .locale_name = "pl",
    .singulars = pl_singulars,

    .locale_plural_fn = pl_plural_fn
};

static lv_i18n_phrase_t pt_singulars[] = {
    {"no new messages", "Nenhuma mensagem nova"},
    {"1 of 1 nodes online", "1 de 1 dispositivos online"},
    {"uptime 00:00:00", "Tempo ligado 00:00:00"},
    {"DEL", "Apagar"},
    {"User name: ", "Nome de Usuário"},
    {"Modem Preset: LONG FAST", "Configuração do modem: LONG FAST"},
    {"Channel: LongFast", "Canal: LongFast"},
    {"Role: Client", "Função: Cliente"},
    {"Screen Timeout: 60s", "Tempo de tela ligada: 60s"},
    {"Screen Lock: off", "Bloqueio de tela: desligado"},
    {"Screen Brightness: 60%", "Brilho de tela: 60%"},
    {"Theme: Dark", "Tema: Escuro"},
    {"Screen Calibration: default", "Calibração de tela: padrão"},
    {"Input Control: none/none", "Controle de entrada: nenhum"},
    {"Message Alert Buzzer: on", "Alerta de mensagem sonoro: ligado"},
    {"Language: English", "Idioma: Inglês"},
    {"Configuration Reset", "Restaurar configuração padrão"},
    {"Reboot / Shutdown", "Reiniciar / Desligar"},
    {"Mesh Detector", "Detector de Mesh"},
    {"Signal Scanner", "Buscador de Sinais"},
    {"Trace Route", "Traçar Rota"},
    {"Neighbors", "Vizinhos"},
    {"Statistics", "Estatísticas"},
    {"Packet Log", "Histórico de Pacotes"},
    {"Meshtastic", "Meshtastic"},
    {"Group Channels", "Grupo de Canais"},
    {"no messages", "Sem Mensagens"},
    {"Settings & Tools", "Configurações e Ferramentas"},
    {"Settings (advanced)", "Configurações Avançadas"},
    {"Locations Map", "Mapa de Localização"},
    {"no chats", "Nenhuma Conversa"},
    {"Node Search", "Buscar Dispositivos"},
    {"Packet Statistics", "Estatísticas de Pacotes"},
    {"Node Options", "Opções do Dispositivo"},
    {"Short Name", "Nome Curto"},
    {"Long Name", "Nome Longo"},
    {"<unset>", "<Não Configurado>"},
    {"LONG FAST\nLONG SLOW\n-- deprecated --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO", "LONG FAST\nLONG SLOW\n-- desativado --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO"},
    {"Client\nClient Mute\nRouter\n-- deprecated --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker", "Cliente\nCliente Mudo\nRoteador\n-- desativado --\nRepetidor\nRastreador\nSensor\nTAK\nCliente Oculto\nAchados e Perdidos\nRastreador TAK"},
    {"Brightness: 60%", "Brilho de Tela: 60%"},
    {"Dark\nLight", "Escuro\nClaro"},
    {"Timeout: 60s", "Tempo Máximo: 60s"},
    {"Screen Lock", "Bloqueio de Tela"},
    {"Lock PIN", "PIN de bloqueio"},
    {"Mouse", "Mouse"},
    {"none", "nenhum"},
    {"Keyboard", "Teclado"},
    {"Message Alert", "Notificação de Mensagem"},
    {"Ringtone", "Alerta Sonoro"},
    {"Default", "Padrão"},
    {"Zone", "Zona"},
    {"City", "Cidade"},
    {"NodeDB Reset\nFactory Reset", "Resetar Base de Dados\nConfigurações de Fábrica"},
    {"Channel Name", "Nome do Canal"},
    {"Pre-shared Key", "Chave pré-compartilhada"},
    {"Unknown", "Desconhecido"},
    {"Offline", "Offline"},
    {"Public Key", "Chave pública"},
    {"Hops away", "Saltos"},
    {"MQTT", "MQTT"},
    {"Position", "Localização"},
    {"Name", "Nome"},
    {"Active Chat", "Conversas Ativas"},
    {"Telemetry", "Telemetria"},
    {"IAQ", "Qualidade do Ar"},
    {"Start", "Início"},
    {"New Message from\n", "Nova Mensagem de\n"},
    {"Resynch...", "Resincronizando..."},
    {"OK", "OK"},
    {"Cancel", "Cancelar"},
    {"FrequencySlot: %d (%g MHz)", "Banco de Frequência %d (%g MHz)"},
    {"Brightness: %d%%", "Brilho de Tela: %d%%"},
    {"Timeout: off", "Temporizador de tela: desligado"},
    {"Timeout: %ds", "Temporizador de tela: %ds"},
    {"Screen Calibration: %s", "Calibração de Tela: %s"},
    {"done", "Concluído"},
    {"default", "Padrão"},
    {"Stop", "Finalizar"},
    {"choose\nnode", "Escolher\ndispositivo"},
    {"choose target node", "Selecionar dispositivo"},
    {"heard: !%08x", "recebido !%08x"},
    {"Packet Log: %d", "Histórico de Pacotes %d"},
    {"Language: %s", "Idioma: %s"},
    {"Screen Timeout: off", "Temporizador de tela: desligado"},
    {"Screen Timeout: %ds", "Temporizador de tela: %ds"},
    {"Screen Brightness: %d%%", "Brilho de Tela: %d%%"},
    {"Theme: %s", "Aparência: %s"},
    {"User name: %s", "Nome de Usuário: %s"},
    {"Device Role: %s", "Tipo de Dispositivo: %s"},
    {"Region: %s", "Região: %s"},
    {"Modem Preset: %s", "Configuração de Modem: %s"},
    {"Channel: %s", "Canal: %s"},
    {"Screen Lock: %s", "Bloqueio de Tela: %s"},
    {"on", "Ligar"},
    {"off", "Desligar"},
    {"Message Alert: %s", "Notificações de Mensagem: %s"},
    {"Util %0.1f%%  Air %0.1f%%", "Util %0.1f%%  Ar %0.1f%%"},
    {"hops: %d", "saltos: %d"},
    {"unknown", "desconhecido"},
    {"<no name>", "<sem nome>"},
    {"%d active chat(s)", "%d conversas ativas"},
    {"New message from \n%s", "Nova mensagem de \n%s"},
    {"Input Control: %s/%s", "Controle de entradas: %s/%s"},
    {"%d of %d nodes online", "%d de %d dispositivos online"},
    {"Filtering ...", "Filtrando..."},
    {"Filter: %d of %d nodes", "Filtro: %d de %d dispositivos"},
    {"now", "agora"},
    {"%d new message", "%d Nova Mensagem"},
    {"%d new messages", "%d Novas Mensagens"},
    {"uptime: %02d:%02d:%02d", "tempo ligado: %02d:%02d:%02d "},
    {"Settings", "Configurações"},
    {"Tools", "Ferramentas"},
    {"Filter", "Filtros"},
    {"Highlight", "Destaque"},
    {"Primary Channel", "Canal Primário"},
    {"Secondary Channels", "Canais Secundários"},
    {"Resynch ...", "Resincronizando..."},
    {"Rebooting ...", "Reiniciando..."},
    {"Shutting down ...", "Desligando..."},
    {NULL, NULL} // End mark
};



static uint8_t pt_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);

    if (((0 <= i && i <= 1))) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t pt_lang = {
    .locale_name = "pt",
    .singulars = pt_singulars,

    .locale_plural_fn = pt_plural_fn
};

static uint8_t ro_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t v = op_v(n); UNUSED(v);
    uint32_t n100 = n % 100;
    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    if ((v != 0) || (n == 0) || (n != 1 && (1 <= n100 && n100 <= 19))) return LV_I18N_PLURAL_TYPE_FEW;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t ro_lang = {
    .locale_name = "ro",


    .locale_plural_fn = ro_plural_fn
};

static lv_i18n_phrase_t se_singulars[] = {
    {"no new messages", "inga nya meddelanden"},
    {"1 of 1 nodes online", "1 av 1 noder online"},
    {"uptime 00:00:00", "upptid 00:00:00"},
    {"User name: ", "Användarnamn: "},
    {"Modem Preset: LONG FAST", "Förinställning: Lång räckvidd / Snabb"},
    {"Channel: LongFast", "Kanal: LongFast"},
    {"Role: Client", "Roll: Klient"},
    {"Screen Timeout: 60s", "Bildskärms Timeout: 60s"},
    {"Screen Lock: off", "Bildskärms Lås: av"},
    {"Screen Brightness: 60%", "Bildskärmens Ljusstyrka: 60%"},
    {"Theme: Dark", "Tema: Mörkt"},
    {"Screen Calibration: default", "Bildskärmens Kalibrering: standard"},
    {"Input Control: none/none", "Inmatningsmetod: ingen/ingen"},
    {"Message Alert Buzzer: on", "Meddelande varningsklocka: på"},
    {"Language: English", "Språk: Svenska"},
    {"Configuration Reset", "Återställ konfiguration"},
    {"Reboot / Shutdown", "Starta om / Stäng av"},
    {"Mesh Detector", "Meshdetektor"},
    {"Signal Scanner", "Signalskanner"},
    {"Trace Route", "Traceroute (spåra rutt)"},
    {"Neighbors", "Grannar"},
    {"Statistics", "Statistik"},
    {"Packet Log", "Paket Loggbok"},
    {"Group Channels", "Kanaler"},
    {"no messages", "inga meddelanden"},
    {"Settings & Tools", "Inställningar & Verktyg"},
    {"Settings (advanced)", "Inställningar (Avancerat)"},
    {"Locations Map", "Karta"},
    {"no chats", "inga chattar"},
    {"Node Search", "Sök nod"},
    {"Packet Statistics", "Paket Statistik"},
    {"Node Options", "Nod alternativ"},
    {"Short Name", "Kort Namn"},
    {"Long Name", "Lånt Namn"},
    {"<unset>", "<ej inställt>"},
    {"LONG FAST\nLONG SLOW\n-- deprecated --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO", "Lång räckvidd / Snabb\nLång räckvidd / Långsam\n-- utfasad --\nMedium Räckvidd / Långsam\nMedium räckvidd / Snabb\nKort räckvidd / Långsam\nKort räckvidd / Snabb\nLång räckvidd / Måttlig\nKort räckvidd / Turbo"},
    {"Client\nClient Mute\nRouter\n-- deprecated --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker", "Klient\nKlient Stum\nRouter\n-- utfasad --\nRelästation\nSpårare\nSensor\nTAK\nKlient Gömd\nFörlorad & hittad\nTAK Spårare"},
    {"Brightness: 60%", "Ljusstyrka: 60%"},
    {"Dark\nLight", "Mörkt\nLjust"},
    {"Screen Lock", "Bildskärms Lås"},
    {"Lock PIN", "Lås PIN"},
    {"Mouse", "Mus"},
    {"none", "ingen"},
    {"Keyboard", "Tangentbord"},
    {"Message Alert", "Meddelande Varning"},
    {"Ringtone", "Ringsignal"},
    {"Default", "Standard"},
    {"Zone", "Zon"},
    {"City", "Stad"},
    {"NodeDB Reset\nFactory Reset", "Nollställ NodeDB\nFabriksåterställning"},
    {"Channel Name", "Kanal Namn"},
    {"Pre-shared Key", "Tidigare delad Nyckel"},
    {"Unknown", "Okänd"},
    {"Offline", "Frånkopplad"},
    {"Public Key", "Publik Nyckel"},
    {"Hops away", "Hopp bort"},
    {"Position", "Position"},
    {"Name", "Namn"},
    {"Active Chat", "Aktiv Chatt"},
    {"Telemetry", "Telemetri"},
    {"Start", "Kör"},
    {"New Message from\n", "Nytt meddelande från\n"},
    {"Resynch...", "Synkronisera om..."},
    {"Cancel", "Avbryt"},
    {"FrequencySlot: %d (%g MHz)", "Frekvensplats: %d (%g MHz)"},
    {"Brightness: %d%%", "Ljusstyrka: %d%%"},
    {"Timeout: off", "Timeout: av"},
    {"Screen Calibration: %s", "Bildskärmens Kalibrering: %s"},
    {"done", "klar"},
    {"default", "standard"},
    {"Stop", "Stopp"},
    {"choose\nnode", "välj\nnod"},
    {"choose target node", "välj mål nod"},
    {"heard: !%08x", "hört: !%08x"},
    {"Packet Log: %d", "Paket Loggbok: %d"},
    {"Language: %s", "Språk: %s"},
    {"Screen Timeout: off", "Bildskärms Timeout: av"},
    {"Screen Timeout: %ds", "Bildskärms Timeout: %ds"},
    {"Screen Brightness: %d%%", "Bildskärmens Ljusstyrka: %d%%"},
    {"Theme: %s", "Tema: %s"},
    {"User name: %s", "Användarnamn: %s"},
    {"Device Role: %s", "Roll: %s"},
    {"Region: %s", "Region: %s"},
    {"Modem Preset: %s", "Förinställning: %s"},
    {"Channel: %s", "Kanal: %s"},
    {"Screen Lock: %s", "Bildskärms Lås: %s"},
    {"on", "på"},
    {"off", "av"},
    {"Message Alert: %s", "Meddelande Varning: %s"},
    {"hops: %d", "hopp: %d"},
    {"unknown", "okänd"},
    {"<no name>", "<inget namn>"},
    {"%d active chat(s)", "%d aktiva chatt(ar)"},
    {"New message from \n%s", "Nytt meddelande från \n%s"},
    {"Input Control: %s/%s", "Inmatningsmetod: %s/%s"},
    {"%d of %d nodes online", "%d av %d noder online"},
    {"Filtering ...", "Filtrerar ..."},
    {"Filter: %d of %d nodes", "Filter: %d av %d noder"},
    {"now", "nu"},
    {"%d new message", "%d nytt meddelande"},
    {"%d new messages", "%d nya meddelanden"},
    {"uptime: %02d:%02d:%02d", "upptid: %02d:%02d:%02d"},
    {"Tools", "Verktyg"},
    {"Highlight", "Markera"},
    {"Settings", "Inställningar"},
    {"Primary Channel", "Primär Kanal"},
    {"Secondary Channels", "Sekundär Kanal"},
    {"Resynch ...", "Synkroniserar om ..."},
    {"Rebooting ...", "Startar om ..."},
    {"Shutting down ...", "Stänger av ..."},
    {NULL, NULL} // End mark
};



static uint8_t se_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);


    if ((n == 1)) return LV_I18N_PLURAL_TYPE_ONE;
    if ((n == 2)) return LV_I18N_PLURAL_TYPE_TWO;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t se_lang = {
    .locale_name = "se",
    .singulars = se_singulars,

    .locale_plural_fn = se_plural_fn
};

static lv_i18n_phrase_t sr_singulars[] = {
    {"no new messages", "nema novih poruka"},
    {"1 of 1 nodes online", "1 od 1 nodova online"},
    {"uptime 00:00:00", "uključen pre 00:00:00"},
    {"Heap: 0\nLVGL: 0", "Hip: 0\nLVGL: 0"},
    {"DEL", "Obriši"},
    {"User name: ", "Korisnikovo ime: "},
    {"Modem Preset: LONG FAST", "Modem: DALEKO BRZO"},
    {"Channel: LongFast", "Kanal: LongFast"},
    {"Role: Client", "Uloga: Client"},
    {"Screen Timeout: 60s", "Ekran se gasi posle: 60s"},
    {"Screen Lock: off", "Zaključivanje ekrana: ugašen"},
    {"Screen Brightness: 60%", "Pozadinsko osvetljenje: 60%"},
    {"Theme: Dark", "Tema: Dark"},
    {"Screen Calibration: default", "Kalibracija ekrana: podrazumevano"},
    {"Input Control: none/none", "Kontrola ulaza: ništa/ništa"},
    {"Message Alert Buzzer: on", "Zvonce za poruku: uključeno"},
    {"Language: English", "Jezik: "},
    {"Configuration Reset", "Resetovanje konfiguracije"},
    {"Reboot / Shutdown", "Reset / Gašenje"},
    {"Mesh Detector", "Detektor mreže"},
    {"Signal Scanner", "Skener signala"},
    {"Trace Route", "Traženje rute"},
    {"Neighbors", "Komšije"},
    {"Statistics", "Statistika"},
    {"Packet Log", "Log paketa"},
    {"Group Channels", "Grupni kanali"},
    {"no messages", "nema poruka"},
    {"Settings & Tools", "Podešavanja i alat"},
    {"Settings (advanced)", "Podešavanja (napredna)"},
    {"Locations Map", "Mapa lokacija"},
    {"no chats", "nema četova"},
    {"Node Search", "Pretraga Nodova"},
    {"Packet Statistics", "Statistika paketa"},
    {"Node Options", "Opcije nodova"},
    {"Short Name", "Kratko ime"},
    {"Long Name", "Dugo ime"},
    {"<unset>", "<nesetovano>"},
    {"Brightness: 60%", "Pozadinsko osvetljenje: 60%"},
    {"Dark\nLight", "Tamno\\nSvetlo"},
    {"Timeout: 60s", "Tajmaut: 60s"},
    {"Screen Lock", "Zaključivanje ekrana"},
    {"Lock PIN", "PIN zaključivanje"},
    {"Mouse", "Miš"},
    {"none", "ništa"},
    {"Keyboard", "Tastatura"},
    {"Message Alert", "Poruka upozorenja"},
    {"Ringtone", "Zvono"},
    {"Default", "Podrazumevano"},
    {"Zone", "Zona"},
    {"City", "Grad"},
    {"NodeDB Reset\nFactory Reset", "Resetovanje NodeDB\nFabrička podešavanja"},
    {"Channel Name", "Ime kanala"},
    {"Pre-shared Key", "Podeljen ključ"},
    {"Unknown", "Nepoznato"},
    {"Offline", "Oflajn"},
    {"Public Key", "Javni ključ"},
    {"Hops away", "Skokovi"},
    {"MQTT", "MQTT"},
    {"Position", "Pozicija"},
    {"Name", "Ime"},
    {"Active Chat", "Aktivan čet"},
    {"Telemetry", "Telemetrija"},
    {"Start", "Start"},
    {"New Message from\\n", "Nova poruka od\n:"},
    {"Resynch...", "Sinhronizacija..."},
    {"OK", "OK"},
    {"Cancel", "Odustajanje"},
    {"FrequencySlot: %d (%g MHz)", "Frekventni slot: %d (%g MHz)"},
    {"Brightness: %d%%", "Pozadinsko: %d%%"},
    {"Timeout: off", "Isticanje: ugašeno"},
    {"Timeout: %ds", "Isticanje: %ds"},
    {"Screen Calibration: %s", "Kalibracija ekrana %s"},
    {"done", "urađeno"},
    {"default", "podrazumevano"},
    {"Stop", "Stop"},
    {"choose\nnode", "odaberi\nništa"},
    {"choose target node", "odaberi ciljani nod"},
    {"Packet Log: %d", "Log paketa: %d"},
    {"Language: %s", "Jezik: %s"},
    {"Screen Timeout: off", "Vreme osvetljenja: ugašeno"},
    {"Screen Timeout: %ds", "Vreme osvetljenja: %ds"},
    {"Screen Brightness: %d%%", "Pozadinsko osvetljenje: %d%%"},
    {"Theme: %s", "Tema: %s"},
    {"User name: %s", "Korisnikovo ime: %s"},
    {"Device Role: %s", "Uloga uređaja"},
    {"Region: %s", "Region: %s"},
    {"Modem Preset: %s", "Modem: %s"},
    {"Channel: %s", "Ime kanala: %s"},
    {"Screen Lock: %s", "Zaključivanje ekrana: %s"},
    {"on", "uključen"},
    {"off", "isključen"},
    {"Message Alert: %s", "Poruka upozorenja: %s"},
    {"hops: %d", "skokovi: %d"},
    {"unknown", "nepoznato"},
    {"<no name>", "<bez imena>"},
    {"%d active chat(s)", "%d aktivnih četova"},
    {"New message from \\n%s", "Nova poruka od \n%s"},
    {"Input Control: %s/%s", "Kontrola ulaza: %s/%s"},
    {"%d of %d nodes online", "%d od %d nodova online"},
    {"Filtering ...", "Filtriranje..."},
    {"Filter: %d of %d nodes", "Filtrirano %d od %d nodova"},
    {"now", "sada"},
    {"%d new message", "%d novih poruka"},
    {"%d new messages", "%d novih poruka"},
    {"uptime: %02d:%02d:%02d", "uključen %02d:%02d:%02d"},
    {"Heap: %d (%d%%)\nLVGL: %d (%d%%)", "Hip: %d (%d%%)\nLVGL: %d (%d%%)"},
    {"Settings", "Podešavanja"},
    {"Tools", "Alat"},
    {"Filter", "Filter"},
    {"Highlight", "Označeno"},
    {"Primary Channel", "Primarni kanal"},
    {"Secondary Channels", "Sekundarni kanal"},
    {"Resynch ...", "Sinhronizacija ..."},
    {"Rebooting ...", "Resetovanje ..."},
    {"Shutting down ...", "Gašenje ..."},
    {"silent", "bez zvuka"},
    {"WiFi: <not setup>", "WiFi: <nije podešen>"},
    {"LoRa TX off!", "LoRa TX ugašen!"},
    {"WiFi pre-shared Key", "WiFi šifra"},
    {"<not set>", "<nije podešen>"},
    {"Banner & Sound", "Baner i zvuk"},
    {"Banner only", "Samo baner"},
    {"Sound only", "Samo zvuk"},
    {NULL, NULL} // End mark
};



static uint8_t sr_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t v = op_v(n); UNUSED(v);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t f = op_f(n); UNUSED(f);
    uint32_t i10 = i % 10;
    uint32_t i100 = i % 100;
    uint32_t f10 = f % 10;
    uint32_t f100 = f % 100;
    if ((v == 0 && i10 == 1 && i100 != 11) || (f10 == 1 && f100 != 11)) return LV_I18N_PLURAL_TYPE_ONE;
    if ((v == 0 && (2 <= i10 && i10 <= 4) && (!(12 <= i100 && i100 <= 14))) || ((2 <= f10 && f10 <= 4) && (!(12 <= f100 && f100 <= 14)))) return LV_I18N_PLURAL_TYPE_FEW;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t sr_lang = {
    .locale_name = "sr",
    .singulars = sr_singulars,

    .locale_plural_fn = sr_plural_fn
};

static uint8_t tr_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);


    if ((n == 1)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t tr_lang = {
    .locale_name = "tr",


    .locale_plural_fn = tr_plural_fn
};

const lv_i18n_language_pack_t lv_i18n_language_pack[] = {
    &en_lang,
    &de_lang,
    &es_lang,
    &fi_lang,
    &fr_lang,
    &it_lang,
    &nl_lang,
    &pl_lang,
    &pt_lang,
    &ro_lang,
    &se_lang,
    &sr_lang,
    &tr_lang,
    NULL // End mark
};

////////////////////////////////////////////////////////////////////////////////


// Internal state
static const lv_i18n_language_pack_t * current_lang_pack;
static const lv_i18n_lang_t * current_lang;


/**
 * Reset internal state. For testing.
 */
void __lv_i18n_reset(void)
{
    current_lang_pack = NULL;
    current_lang = NULL;
}

/**
 * Set the languages for internationalization
 * @param langs pointer to the array of languages. (Last element has to be `NULL`)
 */
int lv_i18n_init(const lv_i18n_language_pack_t * langs)
{
    if(langs == NULL) return -1;
    if(langs[0] == NULL) return -1;

    current_lang_pack = langs;
    current_lang = langs[0];     /*Automatically select the first language*/
    return 0;
}

/**
 * Change the localization (language)
 * @param l_name name of the translation locale to use. E.g. "en-GB"
 */
int lv_i18n_set_locale(const char * l_name)
{
    if(current_lang_pack == NULL) return -1;

    uint16_t i;

    for(i = 0; current_lang_pack[i] != NULL; i++) {
        // Found -> finish
        if(strcmp(current_lang_pack[i]->locale_name, l_name) == 0) {
            current_lang = current_lang_pack[i];
            return 0;
        }
    }

    return -1;
}


static const char * __lv_i18n_get_text_core(lv_i18n_phrase_t * trans, const char * msg_id)
{
    uint16_t i;
    for(i = 0; trans[i].msg_id != NULL; i++) {
        if(strcmp(trans[i].msg_id, msg_id) == 0) {
            /*The msg_id has found. Check the translation*/
            if(trans[i].translation) return trans[i].translation;
        }
    }

    return NULL;
}


/**
 * Get the translation from a message ID
 * @param msg_id message ID
 * @return the translation of `msg_id` on the set local
 */
const char * lv_i18n_get_text(const char * msg_id)
{
    if(current_lang == NULL) return msg_id;

    const lv_i18n_lang_t * lang = current_lang;
    const void * txt;

    // Search in current locale
    if(lang->singulars != NULL) {
        txt = __lv_i18n_get_text_core(lang->singulars, msg_id);
        if (txt != NULL) return txt;
    }

    // Try to fallback
    if(lang == current_lang_pack[0]) return msg_id;
    lang = current_lang_pack[0];

    // Repeat search for default locale
    if(lang->singulars != NULL) {
        txt = __lv_i18n_get_text_core(lang->singulars, msg_id);
        if (txt != NULL) return txt;
    }

    return msg_id;
}

/**
 * Get the translation from a message ID and apply the language's plural rule to get correct form
 * @param msg_id message ID
 * @param num an integer to select the correct plural form
 * @return the translation of `msg_id` on the set local
 */
const char * lv_i18n_get_text_plural(const char * msg_id, int32_t num)
{
    if(current_lang == NULL) return msg_id;

    const lv_i18n_lang_t * lang = current_lang;
    const void * txt;
    lv_i18n_plural_type_t ptype;

    // Search in current locale
    if(lang->locale_plural_fn != NULL) {
        ptype = lang->locale_plural_fn(num);

        if(lang->plurals[ptype] != NULL) {
            txt = __lv_i18n_get_text_core(lang->plurals[ptype], msg_id);
            if (txt != NULL) return txt;
        }
    }

    // Try to fallback
    if(lang == current_lang_pack[0]) return msg_id;
    lang = current_lang_pack[0];

    // Repeat search for default locale
    if(lang->locale_plural_fn != NULL) {
        ptype = lang->locale_plural_fn(num);

        if(lang->plurals[ptype] != NULL) {
            txt = __lv_i18n_get_text_core(lang->plurals[ptype], msg_id);
            if (txt != NULL) return txt;
        }
    }

    return msg_id;
}

/**
 * Get the name of the currently used locale.
 * @return name of the currently used locale. E.g. "en-GB"
 */
const char * lv_i18n_get_current_locale(void)
{
    if(!current_lang) return NULL;
    return current_lang->locale_name;
}
