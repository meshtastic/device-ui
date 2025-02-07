#include "./lv_i18n.h"

////////////////////////////////////////////////////////////////////////////////
// Define plural operands
// http://unicode.org/reports/tr35/tr35-numbers.html#Operands

// Integer version, simplified

#define UNUSED(x) (void)(x)

static inline uint32_t op_n(int32_t val)
{
    return (uint32_t)(val < 0 ? -val : val);
}
static inline uint32_t op_i(uint32_t val)
{
    return val;
}
// always zero, when decimal part not exists.
static inline uint32_t op_v(uint32_t val)
{
    UNUSED(val);
    return 0;
}
static inline uint32_t op_w(uint32_t val)
{
    UNUSED(val);
    return 0;
}
static inline uint32_t op_f(uint32_t val)
{
    UNUSED(val);
    return 0;
}
static inline uint32_t op_t(uint32_t val)
{
    UNUSED(val);
    return 0;
}

static uint8_t en_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    UNUSED(n);
    uint32_t i = op_i(n);
    UNUSED(i);
    uint32_t v = op_v(n);
    UNUSED(v);

    if ((i == 1 && v == 0))
        return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t en_lang = {.locale_name = "en",

                                       .locale_plural_fn = en_plural_fn};

static lv_i18n_phrase_t de_singulars[] = {
    {"User name: %s", "Benutzer: %s"},
    {"Device Role: %s", "Gerätemodus: %s"},
    {"no new messages", "keine Nachrichten"},
    {"1 of 1 nodes online", "1 Gerät online"},
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
    {"Settings", "Konfiguration"},
    {"Settings (advanced)", "Erweiterte Konfiguration"},
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
    {"NodeDB Reset\nFactory Reset", "Datenbank leeren\nWerkseinstellungen"},
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
    {"Theme: Dark", "Farbschema: Dunkel"},
    {"Mesh Detector", "Mesh-Detektor"},
    {"Signal Scanner", "Signalstärke"},
    {"Trace Route", "Route verfolgen"},
    {"Neighbors", "Nachbarn"},
    {"Statistics", "Paketstatistik"},
    {"Packet Log", "Paket Historie"},
    {"Settings & Tools", "Konfiguration & Funktion"},
    {"Node Search", "Gerätesuche"},
    {"Screen Lock", "Bildschirm"},
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
    {"Timeout: off", "Display aus: aus"},
    {"Timeout: %ds", "Display aus: %ds"},
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
    {"New message from \n%s", "Neue Nachricht von \n%s"},
    {"Input Control: %s/%s", "Eingabegeräte: %s/%s"},
    {"Filtering ...", "Filtern ..."},
    {"Filter: %d of %d nodes", "Filter: %d / %d Geräte"},
    {"now", "jetzt"},
    {"%d new message", "%d Nachricht"},
    {"%d new messages", "%d Nachrichten"},
    {"Tools", "Funktionen"},
    {"Highlight", "Hervorheben"},
    {"choose node", "wähle Nachbar"},
    {"Primary Channel", "Primärkanal"},
    {"Secondary Channels", "Sekundärkanäle"},
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
    {"Lock: off/off", "Sperre aus/aus"},
    {"Settings Lock", "Einstellungen"},
    {"Lock: %s/%s", "Sperre: %s/%s"},
    {"Enter Text ...", "Nachricht ..."},
    {"!Enter Filter ...", "!Filter Name..."},
    {"Enter Filter ...", "Filter Name..."},
    {"region unset", "keine Region"},
    {"no signal", "kein Signal"},
    {"Restoring messages ...", "Lade Nachrichten..."},
    {NULL, NULL} // End mark
};

static lv_i18n_phrase_t de_plurals_one[] = {
    {"%d active chat(s)", "%d aktives Gespräch"}, {"%d of %d nodes online", "1 Gerät online"}, {NULL, NULL} // End mark
};

static lv_i18n_phrase_t de_plurals_other[] = {
    {"%d active chat(s)", "%d aktive Gespräche"}, {"%d of %d nodes online", "%d / %d Geräte online"}, {NULL, NULL} // End mark
};

static uint8_t de_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    UNUSED(n);
    uint32_t i = op_i(n);
    UNUSED(i);
    uint32_t v = op_v(n);
    UNUSED(v);

    if ((i == 1 && v == 0))
        return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t de_lang = {.locale_name = "de",
                                       .singulars = de_singulars,
                                       .plurals[LV_I18N_PLURAL_TYPE_ONE] = de_plurals_one,
                                       .plurals[LV_I18N_PLURAL_TYPE_OTHER] = de_plurals_other,
                                       .locale_plural_fn = de_plural_fn};

static lv_i18n_phrase_t es_singulars[] = {
    {"no new messages", "Sin mensajes nuevos"},
    {"1 of 1 nodes online", "1 de 1 nodos activos"},
    {"DEL", "BOR"},
    {"User name: ", "Nomb. Usua:"},
    {"Channel: LongFast", "Canal: LongFast"},
    {"Role: Client", "Rol: Cliente"},
    {"Screen Timeout: 60s", "Apagado pantalla: 60s"},
    {"Screen Brightness: 60%", "Brillo pantalla: 60%"},
    {"Screen Calibration: default", "Calibración Pantalla: predet."},
    {"Message Alert Buzzer: on", "Zumbador en mensaje: on"},
    {"Language: English", "Idioma: Español"},
    {"Configuration Reset", "Reiniciar Configuración"},
    {"Reboot / Shutdown", "Reiniciar / Apagar"},
    {"Group Channels", "Canales grupales"},
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
    {"Message Alert", "Alerta mensaje"},
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
    {"LONG FAST\nLONG SLOW\n-- deprecated --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO",
     "LONG FAST\nLONG SLOW\n-- obsoleto --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO"},
    {"Client\nClient Mute\nRouter\n-- deprecated --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker",
     "Client\nClient Mute\nRouter\n-- obsoleto --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker"},
    {"Dark\nLight", "Oscuro\nClaro"},
    {"Cancel", "Cancelar"},
    {"Brightness: %d%%", "Brillo : %d%%"},
    {"Stop", "Parar"},
    {"choose\nnode", "escoge\nnodo"},
    {"choose target node", "escoge nodo de destino"},
    {"heard: !%08x", "oído: !%08x"},
    {"Packet Log: %d", "Reg. Paquetes: %d"},
    {"Screen Timeout: off", "Apag. pantalla: off"},
    {"Screen Timeout: %ds", "Apag. pantalla: %ds"},
    {"Screen Brightness: %d%%", "Brillo pantalla: %d%%"},
    {"Screen Lock: %s", "Bloq. pantalla: %s"},
    {"on", "activo"},
    {"off", "inactivo"},
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
    {"Lock: off/off", "Bloqueo: no/no"},
    {"Settings Lock", "Bloqueo config"},
    {"Lock: %s/%s", "Bloqueo: %s/%s"},
    {"Enter Text ...", "Escribe un mensaje..."},
    {"!Enter Filter ...", "!Introduce filtro..."},
    {"Enter Filter ...", "Introduce filtro ..."},
    {"region unset", "región no configurada"},
    {"no signal", "sin señal"},
    {NULL, NULL} // End mark
};

static uint8_t es_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    UNUSED(n);

    if ((n == 1))
        return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t es_lang = {.locale_name = "es",
                                       .singulars = es_singulars,

                                       .locale_plural_fn = es_plural_fn};

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
    uint32_t n = op_n(num);
    UNUSED(n);
    uint32_t i = op_i(n);
    UNUSED(i);
    uint32_t v = op_v(n);
    UNUSED(v);

    if ((i == 1 && v == 0))
        return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t fi_lang = {.locale_name = "fi",
                                       .singulars = fi_singulars,

                                       .locale_plural_fn = fi_plural_fn};

static lv_i18n_phrase_t fr_singulars[] = {
    {"no new messages", "0 nouveau message"},
    {"1 of 1 nodes online", "1 sur 1 noeud actif"},
    {"uptime 00:00:00", "Activité: 00:00:00"},
    {"DEL", "DEL"},
    {"User name: ", "Nom:"},
    {"Modem Preset: LONG FAST", "Préréglages: LONG FAST"},
    {"Channel: LongFast", "Canal: LongFast"},
    {"Role: Client", "Rôle: Client"},
    {"Screen Timeout: 60s", "Extinction Écran: 60s"},
    {"Screen Lock: off", "Verr. écran: off"},
    {"Theme: Dark", "Thème: Sombre"},
    {"Screen Calibration: default", "Calibr. d'écran: défaut"},
    {"Message Alert Buzzer: on", "Sonnerie: on"},
    {"Language: English", "Langue: Français"},
    {"Configuration Reset", "Réinitialisation"},
    {"Reboot / Shutdown", "Redémarrer / Arrêter"},
    {"Mesh Detector", "Détecteur de Mesh"},
    {"Signal Scanner", "Qualité du Signal"},
    {"Trace Route", "Trace route"},
    {"Neighbors", "Voisins"},
    {"Statistics", "Statistiques"},
    {"Packet Log", "Journaux de paquets"},
    {"Group Channels", "Canaux de groupes"},
    {"no messages", "aucun message"},
    {"Settings & Tools", "Outils et Configuration"},
    {"Settings (advanced)", "Configur. avancée"},
    {"Locations Map", "Carte / Localisations"},
    {"no chats", "Aucune conversation"},
    {"Node Search", "Recherche noeud"},
    {"Packet Statistics", "Statistiques des paquets"},
    {"Node Options", "Options Noeud"},
    {"Short Name", "Nom court"},
    {"Long Name", "Nom complet"},
    {"<unset>", "<non défini>"},
    {"Brightness: 60%", "Luminosité: 60%"},
    {"Dark\nLight", "Sombre\nLumineux"},
    {"Screen Lock", "Verrou. PIN"},
    {"Lock PIN", "Code PIN"},
    {"Mouse", "Souris"},
    {"Keyboard", "Clavier"},
    {"Message Alert", "Notifications"},
    {"Ringtone", "Sonnerie"},
    {"Default", "défaut"},
    {"Zone", "Zone"},
    {"City", "Ville"},
    {"NodeDB Reset\nFactory Reset", "Reinit. NodeDB\nReinit. Usine"},
    {"Channel Name", "Nom du Canal"},
    {"Pre-shared Key", "Clé pré-partagée"},
    {"Unknown", "Inconnu"},
    {"Offline", "Hors ligne"},
    {"Public Key", "Clé Publique"},
    {"Hops away", "Sauts"},
    {"MQTT", "MQTT"},
    {"Position", "Position"},
    {"Name", "Nom"},
    {"Active Chat", "Chat Actif"},
    {"Telemetry", "Télémétrie"},
    {"Start", "Marche"},
    {"New Message from\n", "Nouveau message de\\n"},
    {"Cancel", "Annuler"},
    {"Brightness: %d%%", "Luminosité : %d%%"},
    {"Timeout: off", "Timeout: désactivé"},
    {"Screen Calibration: %s", "Calibration d'écran: %s"},
    {"done", "fait"},
    {"default", "défaut"},
    {"Stop", "Arrêt"},
    {"choose\nnode", "choix du\nnoeud"},
    {"choose target node", "Choix du noeud cible"},
    {"heard: !%08x", "Vu: !%08x"},
    {"Packet Log: %d", "Journaux des paquets: %d"},
    {"Language: %s", "Langue: %s"},
    {"Screen Timeout: off", "Extin. d'écran: off"},
    {"Screen Timeout: %ds", "Écran allumé: %ds"},
    {"Screen Brightness: %d%%", "Luminosité d'écran: %d%%"},
    {"Theme: %s", "Thème: %s"},
    {"User name: %s", "Nom: %s"},
    {"Device Role: %s", "Rôle: %s"},
    {"Region: %s", "Region: %s"},
    {"Modem Preset: %s", "Préréglages: %s"},
    {"Channel: %s", "Canal: %s"},
    {"Screen Lock: %s", "Verrou. PIN: %s"},
    {"on", "On"},
    {"off", "Off"},
    {"Message Alert: %s", "Notifications: %s"},
    {"hops: %d", "sauts: %d"},
    {"unknown", "inconnu"},
    {"<no name>", "<sans nom>"},
    {"%d active chat(s)", "%d chat(s) actif(s)"},
    {"New message from \n%s", "Nouveau message de:\n%s"},
    {"%d of %d nodes online", "%d sur %d noeuds actifs"},
    {"Filtering ...", "Filtrage ..."},
    {"Filter: %d of %d nodes", "Filtre: %d sur %d noeuds"},
    {"now", "maintenant"},
    {"%d new message", "%d nouveau message"},
    {"%d new messages", "%d nouveaux messages"},
    {"uptime: %02d:%02d:%02d", "Activité: %02d:%02d:%02d"},
    {"Settings", "Configuration"},
    {"Tools", "Outils"},
    {"Filter", "Filtrer"},
    {"Highlight", "Surbrillance"},
    {"Primary Channel", "Canal Primaire"},
    {"Secondary Channels", "Canaux Secondaires"},
    {"Rebooting ...", "Redémarrage ..."},
    {"Shutting down ...", "Arrêt..."},
    {"silent", "silencieux"},
    {"WiFi: <not setup>", "WiFi: <non configuré>"},
    {"Lock: off/off", "Verrouillage: non/non"},
    {"LoRa TX off!", "LoRa TX off!"},
    {"WiFi SSID", "SSID WiFi"},
    {"WiFi pre-shared Key", "Clé WiFi"},
    {"Settings Lock", "Verr. des param."},
    {"Lock: %s/%s", "Verrouillage: %s/%s"},
    {"Enter Text ...", "Écrire votre message..."},
    {"!Enter Filter ...", "!Filtrer..."},
    {"Enter Filter ...", "Filtrer..."},
    {"WiFi: %s", "WiFi: %s"},
    {"<not set>", "<non défini>"},
    {"region unset", "Région non configurée"},
    {"Banner & Sound", "Visuelle et sonore"},
    {"Banner only", "Alerte Visuelle"},
    {"Sound only", "Sonnerie Seule"},
    {"no signal", "aucun signal"},
    {NULL, NULL} // End mark
};

static uint8_t fr_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    UNUSED(n);
    uint32_t i = op_i(n);
    UNUSED(i);

    if ((((i == 0) || (i == 1))))
        return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t fr_lang = {.locale_name = "fr",
                                       .singulars = fr_singulars,

                                       .locale_plural_fn = fr_plural_fn};

static lv_i18n_phrase_t it_singulars[] = {
    {"no new messages", "nessun messaggio"},
    {"1 of 1 nodes online", "1 di 1 nodi online"},
    {"uptime 00:00:00", "tempo di attività 00:00:00"},
    {"Heap: 0\nLVGL: 0", "Heap: 0\nLVGL: 0"},
    {"DEL", "CANC"},
    {"User name: ", "Nome utente: "},
    {"Modem Preset: LONG FAST", "Modem: LONG FAST"},
    {"Channel: LongFast", "Canale: LongFast"},
    {"Role: Client", "Ruolo: Client"},
    {"Screen Timeout: 60s", "Timeout schermo: 60s"},
    {"Screen Lock: off", "Blocco schermo: disattivato"},
    {"Screen Brightness: 60%", "Luminosità schermo: 60%"},
    {"Theme: Dark", "Tema: Scuro"},
    {"Screen Calibration: default", "Calibrazione schermo: predefinita"},
    {"Input Control: none/none", "Controllo di input: nessuno/nessuno"},
    {"Message Alert Buzzer: on", "Avviso messaggio cicalino: attivo"},
    {"Language: English", "Lingua: Italiano"},
    {"Configuration Reset", "Ripristino configurazione"},
    {"Reboot / Shutdown", "Riavvio / Spegnimento"},
    {"Mesh Detector", "Rilevatore Mesh"},
    {"Signal Scanner", "Scanner segnale"},
    {"Trace Route", "Traccia percorso"},
    {"Neighbors", "Vicini"},
    {"Statistics", "Statistiche"},
    {"Packet Log", "Registro pacchetti"},
    {"Meshtastic", "Meshtastic"},
    {"Group Channels", "Canali di gruppo"},
    {"no messages", "nessun messaggio"},
    {"Settings & Tools", "Impostazioni & Strumenti"},
    {"Settings (advanced)", "Impostazioni (avanzate)"},
    {"Locations Map", "Mappa delle posizioni"},
    {"no chats", "nessuna chat"},
    {"Node Search", "Ricerca nodo"},
    {"Packet Statistics", "Statistiche pacchetti"},
    {"Node Options", "Opzioni nodo"},
    {"Short Name", "Nome breve"},
    {"Long Name", "Nome lungo"},
    {"<unset>", "<non impostato>"},
    {"Client\nClient Mute\nRouter\n-- deprecated --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker",
     "Client\nClient Muto\nRouter\n-- deprecato --\nRipetitore\nTracker\nSensore\nTAK\nClient Nascosto\nOggetti Smarriti\nTAK "
     "Tracker"},
    {"Brightness: 60%", "Luminosità: 60%"},
    {"Dark\nLight", "Scuro\nChiaro"},
    {"Screen Lock", "Blocco schermo"},
    {"Lock PIN", "Codice PIN"},
    {"none", "nessuno"},
    {"Keyboard", "Tastiera"},
    {"Message Alert", "Avviso di messaggio"},
    {"Ringtone", "Suoneria"},
    {"Default", "Predefinito"},
    {"Zone", "Zona"},
    {"City", "Città"},
    {"NodeDB Reset\nFactory Reset", "Ripristino NodeDB\nRipristino di fabbrica"},
    {"Channel Name", "Nome Canale"},
    {"Pre-shared Key", "Chiave Precondivisa"},
    {"Unknown", "Sconosciuto"},
    {"Public Key", "Chiave Pubblica"},
    {"Hops away", "Salti di distanza"},
    {"Position", "Posizione"},
    {"Name", "Nome"},
    {"Active Chat", "Chat Attiva"},
    {"Telemetry", "Telemetria"},
    {"New Message from\n", "Nuovo messaggio da\n"},
    {"Resynch...", "Risinc..."},
    {"Cancel", "Annulla"},
    {"FrequencySlot: %d (%g MHz)", "Slot di Frequenza: %d (%g MHz)"},
    {"Brightness: %d%%", "Luminosità: %d%%"},
    {"Timeout: off", "Timeout: disattivato"},
    {"Timeout: %ds", "Timeout: %ds"},
    {"Screen Calibration: %s", "Calibrazione Schermo: %s"},
    {"done", "fatto"},
    {"default", "predefinito"},
    {"choose\nnode", "scegli\nnodo"},
    {"choose target node", "scegli nodo di destinazione"},
    {"heard: !%08x", "sentito: !%08x"},
    {"Packet Log: %d", "Registro Pacchetti: %d"},
    {"Language: %s", "Lingua: %s"},
    {"Screen Timeout: off", "Timeout Schermo: disattivato"},
    {"Screen Timeout: %ds", "Timeout Schermo: %ds"},
    {"Screen Brightness: %d%%", "Luminosità Schermo: %d%%"},
    {"Theme: %s", "Tema: %s"},
    {"User name: %s", "Nome Utente: %s"},
    {"Device Role: %s", "Ruolo Dispositivo: %s"},
    {"Region: %s", "Regione: %s"},
    {"Modem Preset: %s", "Preset Modem: %s"},
    {"Channel: %s", "Canale: %s"},
    {"Screen Lock: %s", "Blocco Schermo: %s"},
    {"on", "attivo"},
    {"off", "disattivo"},
    {"Message Alert: %s", "Avviso di messaggio: %s"},
    {"Util %0.1f%%  Air %0.1f%%", "Utilizzo %0.1f%%  Aria %0.1f%%"},
    {"hops: %d", "salti: %d"},
    {"unknown", "sconosciuto"},
    {"<no name>", "<senza nome>"},
    {"New message from \n%s", "Nuovo messaggio da \n%s"},
    {"Input Control: %s/%s", "Controllo di Input: %s/%s"},
    {"Filtering ...", "Filtraggio ..."},
    {"Filter: %d of %d nodes", "Filtro: %d di %d nodi"},
    {"now", "ora"},
    {"%d new message", "%d nuovo messaggio"},
    {"%d new messages", "%d nuovi messaggi"},
    {"uptime: %02d:%02d:%02d", "tempo di attività:\n%02d:%02d:%02d"},
    {"Heap: %d (%d%%)\nLVGL: %d (%d%%)", "Heap: %d (%d%%)\nLVGL: %d (%d%%)"},
    {"Settings", "Impostazioni"},
    {"Tools", "Strumenti"},
    {"Filter", "Filtro"},
    {"Highlight", "Evidenzia"},
    {"Primary Channel", "Canale Primario"},
    {"Secondary Channels", "Canali Secondari"},
    {"Resynch ...", "Risinc ..."},
    {"Rebooting ...", "Riavvio ..."},
    {"Shutting down ...", "Spegnimento ..."},
    {"silent", "silenzioso"},
    {"WiFi: <not setup>", "WiFi: <non configurato>"},
    {"Lock: off/off", "Blocco: disattivato/disattivato"},
    {"LoRa TX off!", "LoRa TX disattivato!"},
    {"WiFi SSID", "SSID WiFi"},
    {"WiFi pre-shared Key", "Chiave Precondivisa WiFi"},
    {"Settings Lock", "Blocco Impostazioni"},
    {"Lock: %s/%s", "Blocco: %s/%s"},
    {"Enter Text ...", "Inserisci Testo ..."},
    {"!Enter Filter ...", "!Inserisci Filtro ..."},
    {"Enter Filter ...", "Inserisci Filtro ..."},
    {"WiFi: %s", "WiFi: %s"},
    {"<not set>", "<non impostato>"},
    {"region unset", "regione non impostata"},
    {"Banner & Sound", "Banner e Suono"},
    {"Banner only", "Solo Banner"},
    {"Sound only", "Solo Suono"},
    {"no signal", "nessun segnale"},
    {NULL, NULL} // End mark
};

static lv_i18n_phrase_t it_plurals_one[] = {
    {"%d active chat(s)", "%d chat attiva"}, {"%d of %d nodes online", "1 nodo online"}, {NULL, NULL} // End mark
};

static lv_i18n_phrase_t it_plurals_other[] = {
    {"%d active chat(s)", "%d chat attive"}, {"%d of %d nodes online", "%d di %d nodi online"}, {NULL, NULL} // End mark
};

static uint8_t it_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    UNUSED(n);
    uint32_t i = op_i(n);
    UNUSED(i);
    uint32_t v = op_v(n);
    UNUSED(v);

    if ((i == 1 && v == 0))
        return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t it_lang = {.locale_name = "it",
                                       .singulars = it_singulars,
                                       .plurals[LV_I18N_PLURAL_TYPE_ONE] = it_plurals_one,
                                       .plurals[LV_I18N_PLURAL_TYPE_OTHER] = it_plurals_other,
                                       .locale_plural_fn = it_plural_fn};

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
    {"LONG FAST\nLONG SLOW\n-- deprecated --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO",
     "LONG FAST\nLONG SLOW\n-- deprecated --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO"},
    {"Client\nClient Mute\nRouter\n-- deprecated --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker",
     "Client\nClient Mute\nRouter\n-- deprecated --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker"},
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
    {"silent", "stil"},
    {"WiFi: <not setup>", "WiFi: <niet ingesteld>"},
    {"Lock: off/off", "Slot: uit/uit"},
    {"LoRa TX off!", "Lora TX uit!"},
    {"Settings Lock", "Instellingen Slot"},
    {"Lock: %s/%s", "Slot: %s/%s"},
    {"Enter Text ...", "Voer Tekst In ..."},
    {"!Enter Filter ...", "!Naam ..."},
    {"Enter Filter ...", "Naam ..."},
    {"<not set>", "<niet ingesteld>"},
    {"region unset", "regio niet ingesteld"},
    {"Banner & Sound", "Strook & Geluid"},
    {"Banner only", "Alleen strook"},
    {"Sound only", "Alleen geluid"},
    {"no signal", "geen signaal"},
    {NULL, NULL} // End mark
};

static uint8_t nl_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    UNUSED(n);
    uint32_t i = op_i(n);
    UNUSED(i);
    uint32_t v = op_v(n);
    UNUSED(v);

    if ((i == 1 && v == 0))
        return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t nl_lang = {.locale_name = "nl",
                                       .singulars = nl_singulars,

                                       .locale_plural_fn = nl_plural_fn};

static lv_i18n_phrase_t no_singulars[] = {
    {"no new messages", "Ingen nye meldinger"},
    {"1 of 1 nodes online", "1 av 1 noder online"},
    {"uptime 00:00:00", "oppetid 00:00:00"},
    {"User name: ", "Brukernavn: "},
    {"Modem Preset: LONG FAST", "Modem innstilling: LONG FAST"},
    {"Channel: LongFast", "Kanal: LongFast"},
    {"Role: Client", "Rolle: Klient"},
    {"Screen Timeout: 60s", "Skjermtidsavbrudd: 60s"},
    {"Screen Lock: off", "Skjermlås: av"},
    {"Screen Brightness: 60%", "Lysstyrke skjerm: 60%"},
    {"Theme: Dark", "Tema: Mørk"},
    {"Screen Calibration: default", "Skjermkalibering: standard"},
    {"Input Control: none/none", "Input kontroll: ingen/ingen"},
    {"Message Alert Buzzer: on", "Meldingsvarslingslyd: på"},
    {"Language: English", "Språk: Norsk Bokmål"},
    {"Configuration Reset", "Farbrikkinstillinger"},
    {"Reboot / Shutdown", "Start på nytt / Skru av"},
    {"Mesh Detector", "Meshdetektor"},
    {"Signal Scanner", "Signalskanner"},
    {"Trace Route", "Spor rute"},
    {"Neighbors", "Naboer"},
    {"Statistics", "Statistikk"},
    {"Packet Log", "Pakke-logg"},
    {"Group Channels", "Kanaler"},
    {"no messages", "ingen meldinger"},
    {"Settings & Tools", "Innstillinger & Verktøy"},
    {"Settings (advanced)", "Innstillinger (Avansert)"},
    {"Locations Map", "Kart"},
    {"no chats", "ingen chats"},
    {"Node Search", "Søk node"},
    {"Packet Statistics", "Statistikk på pakker"},
    {"Node Options", "Node alternativ"},
    {"Short Name", "Kort Navn"},
    {"Long Name", "Langt Navn"},
    {"<unset>", "<ikke satt>"},
    {"LONG FAST\nLONG SLOW\n-- deprecated --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO",
     "LONG FAST\nLONG SLOW\n-- utfaset --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO"},
    {"Client\nClient Mute\nRouter\n-- deprecated --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker",
     "Klient\nKlient Stum\nRuter\n-- utfaset --\nRepeater\nSporer\nSensor\nTAK\nKlient Gjemt\nMistet & Funnet\nTAK Sporing"},
    {"Brightness: 60%", "Lysstyrke: 60%"},
    {"Dark\nLight", "Mørk\nLyst"},
    {"Timeout: 60s", "Tidsavbrudd: 60s"},
    {"Screen Lock", "Skjermlås"},
    {"Lock PIN", "PIN"},
    {"Mouse", "Mus"},
    {"none", "ingen"},
    {"Keyboard", "Tastatur"},
    {"Message Alert", "Meldingsvarsel"},
    {"Ringtone", "Ringesignal"},
    {"Default", "Standard"},
    {"Zone", "Sone"},
    {"City", "By"},
    {"NodeDB Reset\nFactory Reset", "Nullstill NodeDB\nFabrikkinstillinger"},
    {"Channel Name", "Kanal Navn"},
    {"Pre-shared Key", "Delt nøkkel"},
    {"Unknown", "Ukjent"},
    {"Offline", "Frakoblet"},
    {"Public Key", "Offentlig nøkkel"},
    {"Hops away", "Hopp bort"},
    {"Position", "Posisjon"},
    {"Name", "Navn"},
    {"Active Chat", "Aktiv Chat"},
    {"Telemetry", "Telemetri"},
    {"Start", "Kjør"},
    {"New Message from\n", "Ny melding fra\n"},
    {"Resynch...", "Synkroniser om..."},
    {"Cancel", "Avbryt"},
    {"FrequencySlot: %d (%g MHz)", "Frekvensplass: %d (%g MHz)"},
    {"Brightness: %d%%", "Lysstyrke: %d%%"},
    {"Timeout: off", "Tidsavbrudd: av"},
    {"Timeout: %ds", "Tidsavbrudd: %ds"},
    {"Screen Calibration: %s", "Bildskärmens Kalibrering: %s"},
    {"done", "ferdig"},
    {"default", "standard"},
    {"Stop", "Stopp"},
    {"choose\nnode", "velg\nnode"},
    {"choose target node", "velg mål-node"},
    {"heard: !%08x", "hørt: !%08x"},
    {"Packet Log: %d", "Pakke-loggbok: %d"},
    {"Language: %s", "Språk: %s"},
    {"Screen Timeout: off", "Skjermtidsavbrudd: av"},
    {"Screen Timeout: %ds", "Skjermtidsavbrudd: %ds"},
    {"Screen Brightness: %d%%", "Lysstyrke skjerm: %d%%"},
    {"Theme: %s", "Tema: %s"},
    {"User name: %s", "Brukernavn: %s"},
    {"Device Role: %s", "Rolle: %s"},
    {"Region: %s", "Region: %s"},
    {"Modem Preset: %s", "Modem innstilling: %s"},
    {"Channel: %s", "Kanal: %s"},
    {"Screen Lock: %s", "Skjermlås: %s"},
    {"on", "på"},
    {"off", "av"},
    {"Message Alert: %s", "Meldingsvarsling: %s"},
    {"hops: %d", "hopp: %d"},
    {"unknown", "ukjent"},
    {"<no name>", "<ingen navn>"},
    {"New message from \n%s", "Ny melding fra \n%s"},
    {"Input Control: %s/%s", "Input kontroll: %s/%s"},
    {"Filtering ...", "Filtrerer ..."},
    {"Filter: %d of %d nodes", "Filter: %d av %d noder"},
    {"now", "nå"},
    {"%d new message", "%d ny melding"},
    {"%d new messages", "%d nye meldinger"},
    {"uptime: %02d:%02d:%02d", "oppetid: %02d:%02d:%02d"},
    {"Tools", "Verktøy"},
    {"Highlight", "Marker"},
    {"Settings", "Innstillinger"},
    {"Primary Channel", "Hovedkanal"},
    {"Secondary Channels", "Sekunder kanal"},
    {"Resynch ...", "Synkroniserer ..."},
    {"Rebooting ...", "Starter på nytt ..."},
    {"Shutting down ...", "Skrur av ..."},
    {"silent", "stille"},
    {"WiFi: <not setup>", "WiFi: <ingen innstilling>"},
    {"Lock: off/off", "Lås: av/av"},
    {"LoRa TX off!", "LoRa TX avskrudd!"},
    {"WiFi pre-shared Key", "WiFi passord"},
    {"Settings Lock", "Innstillingslås"},
    {"Lock: %s/%s", "Lås: %s/%s"},
    {"Enter Text ...", "Tekst ..."},
    {"!Enter Filter ...", "!Filter ..."},
    {"Enter Filter ...", "Filter ..."},
    {"<not set>", "<ikke satt>"},
    {"region unset", "ingen region"},
    {"Banner & Sound", "Banner & Lyd"},
    {"Banner only", "Bare banner"},
    {"Sound only", "Bare lyd"},
    {"no signal", "ingen signal"},
    {NULL, NULL} // End mark
};

static lv_i18n_phrase_t no_plurals_one[] = {
    {"%d active chat(s)", "1 aktiv chat"}, {"%d of %d nodes online", "1 node online"}, {NULL, NULL} // End mark
};

static lv_i18n_phrase_t no_plurals_other[] = {
    {"%d active chat(s)", "%d aktive chat"}, {"%d of %d nodes online", "%d av %d noder online"}, {NULL, NULL} // End mark
};

static uint8_t no_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    UNUSED(n);

    if ((n == 1))
        return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t no_lang = {.locale_name = "no",
                                       .singulars = no_singulars,
                                       .plurals[LV_I18N_PLURAL_TYPE_ONE] = no_plurals_one,
                                       .plurals[LV_I18N_PLURAL_TYPE_OTHER] = no_plurals_other,
                                       .locale_plural_fn = no_plural_fn};

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
    uint32_t n = op_n(num);
    UNUSED(n);
    uint32_t i = op_i(n);
    UNUSED(i);
    uint32_t v = op_v(n);
    UNUSED(v);
    uint32_t i10 = i % 10;
    uint32_t i100 = i % 100;
    if ((i == 1 && v == 0))
        return LV_I18N_PLURAL_TYPE_ONE;
    if ((v == 0 && (2 <= i10 && i10 <= 4) && (!(12 <= i100 && i100 <= 14))))
        return LV_I18N_PLURAL_TYPE_FEW;
    if ((v == 0 && i != 1 && (0 <= i10 && i10 <= 1)) || (v == 0 && (5 <= i10 && i10 <= 9)) ||
        (v == 0 && (12 <= i100 && i100 <= 14)))
        return LV_I18N_PLURAL_TYPE_MANY;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t pl_lang = {.locale_name = "pl",
                                       .singulars = pl_singulars,

                                       .locale_plural_fn = pl_plural_fn};

static lv_i18n_phrase_t pt_singulars[] = {
    {"no new messages", "Nenhuma mensagem"},
    {"1 of 1 nodes online", "1 dispositivo online"},
    {"uptime 00:00:00", "Tempo ligado\n00:00:00"},
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
    {"LONG FAST\nLONG SLOW\n-- deprecated --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO",
     "LONG FAST\nLONG SLOW\n-- desativado --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO"},
    {"Client\nClient Mute\nRouter\n-- deprecated --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker",
     "Cliente\nCliente Mudo\nRoteador\n-- desativado --\nRepetidor\nRastreador\nSensor\nTAK\nCliente Oculto\nAchados e "
     "Perdidos\nRastreador TAK"},
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
    {"Filtering ...", "Filtrando..."},
    {"Filter: %d of %d nodes", "Filtro: %d/%d dispositivos"},
    {"now", "agora"},
    {"%d new message", "%d Nova Mensagem"},
    {"%d new messages", "%d Novas Mensagens"},
    {"uptime: %02d:%02d:%02d", "tempo ligado:\n%02d:%02d:%02d"},
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

static lv_i18n_phrase_t pt_plurals_one[] = {
    {"%d of %d nodes online", "1 dispositivo online"}, {NULL, NULL} // End mark
};

static lv_i18n_phrase_t pt_plurals_other[] = {
    {"%d of %d nodes online", "%d/%d dispositivos online"}, {NULL, NULL} // End mark
};

static uint8_t pt_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    UNUSED(n);
    uint32_t i = op_i(n);
    UNUSED(i);

    if (((0 <= i && i <= 1)))
        return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t pt_lang = {.locale_name = "pt",
                                       .singulars = pt_singulars,
                                       .plurals[LV_I18N_PLURAL_TYPE_ONE] = pt_plurals_one,
                                       .plurals[LV_I18N_PLURAL_TYPE_OTHER] = pt_plurals_other,
                                       .locale_plural_fn = pt_plural_fn};

static uint8_t ro_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    UNUSED(n);
    uint32_t i = op_i(n);
    UNUSED(i);
    uint32_t v = op_v(n);
    UNUSED(v);
    uint32_t n100 = n % 100;
    if ((i == 1 && v == 0))
        return LV_I18N_PLURAL_TYPE_ONE;
    if ((v != 0) || (n == 0) || (n != 1 && (1 <= n100 && n100 <= 19)))
        return LV_I18N_PLURAL_TYPE_FEW;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t ro_lang = {.locale_name = "ro",

                                       .locale_plural_fn = ro_plural_fn};

static lv_i18n_phrase_t ru_singulars[] = {
    {"no new messages", "нет новых сообщений"},
    {"1 of 1 nodes online", "1 из 1 узлов онлайн"},
    {"DEL", "DEL"},
    {"User name: ", "Имя пользователя:"},
    {"Channel: LongFast", "Канал: LongFast"},
    {"Role: Client", "Роль: Клиент"},
    {"Screen Timeout: 60s", "Таймаут экрана: 60с"},
    {"Screen Brightness: 60%", "Яркость экрана: 60%"},
    {"Screen Calibration: default", "Калибровка экрана: по умолчанию"},
    {"Message Alert Buzzer: on", "Оповещение: включено"},
    {"Language: English", "Язык: Русский"},
    {"Configuration Reset", "Сброс настроек"},
    {"Reboot / Shutdown", "Перезагрузка / Выкл."},
    {"Group Channels", "Групповые каналы"},
    {"no messages", "нет сообщений"},
    {"Settings", "Настройки"},
    {"Settings (advanced)", "Расширенные настройки"},
    {"Locations Map", "Карта локаций"},
    {"no chats", "нет чатов"},
    {"Node Options", "Опции узлов"},
    {"Short Name", "Краткое имя"},
    {"Long Name", "Полное имя"},
    {"<unset>", "<пусто>"},
    {"Brightness: 60%", "Яркость: 60%"},
    {"Mouse", "Мышка"},
    {"Keyboard", "Клавиатура"},
    {"Message Alert", "Оповещение"},
    {"NodeDB Reset\nFactory Reset", "Сброс NodeDB\nСброс настроек"},
    {"Channel Name", "Имя канала"},
    {"New Message from\\n", "Новое сообщение от\\n"},
    {"User name: %s", "Имя: %s"},
    {"Device Role: %s", "Роль устройства: %s"},
    {"Screen Calibration: %s", "Калибровка экрана: %s"},
    {"done", "готово"},
    {"default", "по умолчанию"},
    {"Region: %s", "Регион: %s"},
    {"Channel: %s", "Канал: %s"},
    {"Language: %s", "Язык: %s"},
    {"Screen Lock: off", "Блокировка экрана: off"},
    {"Theme: Dark", "Тема: Тёмная"},
    {"Mesh Detector", "Детектор Mesh"},
    {"Signal Scanner", "Сканер сигнала"},
    {"Trace Route", "Маршрут"},
    {"Neighbors", "Соседи"},
    {"Statistics", "Статистика"},
    {"Packet Log", "Лог пакетов"},
    {"Settings & Tools", "Настройки и инструменты"},
    {"Node Search", "Поиск узла"},
    {"Screen Lock", "Блокировка экрана"},
    {"Lock PIN", "PIN-код"},
    {"Ringtone", "Рингтон"},
    {"Zone", "Зона"},
    {"City", "Город"},
    {"Unknown", "Неизвестно"},
    {"Offline", "Не в сети"},
    {"Public Key", "Открытый ключ"},
    {"Hops away", "Переходы"},
    {"Position", "Позиция"},
    {"Name", "Имя"},
    {"Active Chat", "Активный чат"},
    {"Telemetry", "Телеметрия"},
    {"Start", "Начать"},
    {"Theme: %s", "Тема: %s"},
    {"Packet Statistics", "Статистика пакетов"},
    {"LONG FAST\nLONG SLOW\n-- deprecated --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO",
     "LONG FAST\nLONG SLOW\n-- устарело --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO"},
    {"Client\nClient Mute\nRouter\n-- deprecated --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker",
     "Client\nClient Mute\nRouter\n-- устарело --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker"},
    {"Dark\nLight", "Тёмная\nСветлая"},
    {"Cancel", "Отмена"},
    {"Brightness: %d%%", "Яркость: %d%%"},
    {"Stop", "Стоп"},
    {"choose\nnode", "выбрать\nузел"},
    {"choose target node", "выбрать узел назначения"},
    {"heard: !%08x", "услышано: !%08x"},
    {"Packet Log: %d", "Журнал пакетов: %d"},
    {"Screen Timeout: off", "Таймаут экрана: выкл"},
    {"Screen Timeout: %ds", "Таймаут экрана: %ds"},
    {"Screen Brightness: %d%%", "Яркость экрана: %d%%"},
    {"Screen Lock: %s", "Блокировка экрана: %s"},
    {"on", "вкл"},
    {"off", "выкл"},
    {"Message Alert: %s", "Оповещение: %s"},
    {"hops: %d", "переходов: %d"},
    {"unknown", "неизвестно"},
    {"<no name>", "<без имени>"},
    {"%d active chat(s)", "%d активных чатов"},
    {"New message from \n%s", "Новое сообщение от:\n%s"},
    {"%d of %d nodes online", "%d из %d узлов онлайн"},
    {"Filtering ...", "Фильтрация ..."},
    {"Filter: %d of %d nodes", "Фильтр: %d из %d узлов"},
    {"now", "сейчас"},
    {"%d new message", "%d новое сообщение"},
    {"%d new messages", "%d новых сообщений"},
    {"uptime: %02d:%02d:%02d", "время работы:\n%02d:%02d:%02d"},
    {"Tools", "Инструменты"},
    {"Filter", "Фильтр"},
    {"Highlight", "Выделить"},
    {"Primary Channel", "Основной канал"},
    {"Secondary Channels", "Вторичные каналы"},
    {"Resynch ...", "Ресинхронизация ..."},
    {"Rebooting ...", "Перезагрузка ..."},
    {"Shutting down ...", "Выключение ..."},
    {"silent", "тихий режим"},
    {"WiFi: <not setup>", "WiFi: <не настроен>"},
    {"Lock: off/off", "Блокировка: выкл/выкл"},
    {"LoRa TX off!", "LoRa TX выкл.!"},
    {"WiFi SSID", "Имя сети WiFi"},
    {"Settings Lock", "Блокировка настроек"},
    {"Lock: %s/%s", "Блокировка: %s/%s"},
    {"Enter Text ...", "Введите сообщение ..."},
    {"!Enter Filter ...", "!Введите фильтр ..."},
    {"Enter Filter ...", "Введите фильтр ..."},
    {"WiFi: %s", "WiFi: %s"},
    {"<not set>", "<не настроено>"},
    {"region unset", "регион не установлен"},
    {"Banner & Sound", "Баннер и звук"},
    {"Banner only", "Только баннер"},
    {"Sound only", "Только звук"},
    {NULL, NULL} // End mark
};

static uint8_t ru_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    UNUSED(n);
    uint32_t v = op_v(n);
    UNUSED(v);
    uint32_t i = op_i(n);
    UNUSED(i);
    uint32_t i10 = i % 10;
    uint32_t i100 = i % 100;
    if ((v == 0 && i10 == 1 && i100 != 11))
        return LV_I18N_PLURAL_TYPE_ONE;
    if ((v == 0 && (2 <= i10 && i10 <= 4) && (!(12 <= i100 && i100 <= 14))))
        return LV_I18N_PLURAL_TYPE_FEW;
    if ((v == 0 && i10 == 0) || (v == 0 && (5 <= i10 && i10 <= 9)) || (v == 0 && (11 <= i100 && i100 <= 14)))
        return LV_I18N_PLURAL_TYPE_MANY;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t ru_lang = {.locale_name = "ru",
                                       .singulars = ru_singulars,

                                       .locale_plural_fn = ru_plural_fn};

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
    {"Long Name", "Långt Namn"},
    {"<unset>", "<ej inställt>"},
    {"LONG FAST\nLONG SLOW\n-- deprecated --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO",
     "Lång räckvidd / Snabb\nLång räckvidd / Långsam\n-- utfasad --\nMedium Räckvidd / Långsam\nMedium räckvidd / Snabb\nKort "
     "räckvidd / Långsam\nKort räckvidd / Snabb\nLång räckvidd / Måttlig\nKort räckvidd / Turbo"},
    {"Client\nClient Mute\nRouter\n-- deprecated --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker",
     "Klient\nKlient Stum\nRouter\n-- utfasad --\nRelästation\nSpårare\nSensor\nTAK\nKlient Gömd\nFörlorad & hittad\nTAK "
     "Spårare"},
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
    {"New message from \n%s", "Nytt meddelande från \n%s"},
    {"Input Control: %s/%s", "Inmatningsmetod: %s/%s"},
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
    {"silent", "tyst"},
    {"WiFi: <not setup>", "WiFi: <ej inställt>"},
    {"Lock: off/off", "Lås: av/av"},
    {"LoRa TX off!", "LoRa TX avstängt!"},
    {"WiFi pre-shared Key", "WiFi Lösenord"},
    {"Settings Lock", "Inställningslås"},
    {"Lock: %s/%s", "Lås: %s/%s"},
    {"Enter Text ...", "Text ..."},
    {"!Enter Filter ...", "!Filter ..."},
    {"Enter Filter ...", "Filter ..."},
    {"<not set>", "<ej inställd>"},
    {"region unset", "ingen region"},
    {"Banner & Sound", "Banner & Ljud"},
    {"Banner only", "Endast Banner"},
    {"Sound only", "Endast ljud"},
    {NULL, NULL} // End mark
};

static lv_i18n_phrase_t se_plurals_one[] = {
    {"%d active chat(s)", "1 aktiv chatt"}, {"%d of %d nodes online", "1 nod online"}, {NULL, NULL} // End mark
};

static lv_i18n_phrase_t se_plurals_other[] = {
    {"%d active chat(s)", "%d aktiva chattar"}, {"%d of %d nodes online", "%d av %d noder online"}, {NULL, NULL} // End mark
};

static uint8_t se_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    UNUSED(n);

    if ((n == 1))
        return LV_I18N_PLURAL_TYPE_ONE;
    if ((n == 2))
        return LV_I18N_PLURAL_TYPE_TWO;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t se_lang = {.locale_name = "se",
                                       .singulars = se_singulars,
                                       .plurals[LV_I18N_PLURAL_TYPE_ONE] = se_plurals_one,
                                       .plurals[LV_I18N_PLURAL_TYPE_OTHER] = se_plurals_other,
                                       .locale_plural_fn = se_plural_fn};

static lv_i18n_phrase_t sl_singulars[] = {
    {"User name: %s", "Uporabniško ime: %s"},
    {"Device Role: %s", "Vloga: %s"},
    {"no new messages", "ni novih sporočil"},
    {"1 of 1 nodes online", "1 vozlišč povezano"},
    {"DEL", "Zbriši"},
    {"User name: ", "Uporabniško ime: "},
    {"Modem Preset: LONG FAST", "Modem: LONG FAST"},
    {"Channel: LongFast", "Kanal: LongFast"},
    {"Role: Client", "Vloga: Odjemalec"},
    {"Screen Timeout: 60s", "Izklop zaslona po: 60s"},
    {"Screen Lock: off", "Zaklep zaslona: izključeno"},
    {"Screen Brightness: 60%", "Svetlost zaslona: 60%"},
    {"Theme: Dark", "Tema: Temna"},
    {"Screen Calibration: default", "Kalibracija zaslona: privzeto"},
    {"Input Control: none/none", "Nadzor vnosa: nič/nič"},
    {"Message Alert Buzzer: on", "Opozorila za sporočila: vključeno"},
    {"Language: English", "Jezik: English"},
    {"Configuration Reset", "Ponastavi nastavitve"},
    {"Reboot / Shutdown", "Ponovni zagon / Izklop"},
    {"Mesh Detector", "Detektor mreže"},
    {"Signal Scanner", "Skener signala"},
    {"Trace Route", "Iskanje poti"},
    {"Neighbors", "Sosedi"},
    {"Statistics", "Statistika"},
    {"Packet Log", "Dnevnik paketov"},
    {"Group Channels", "Skupinski kanali"},
    {"no messages", "ni sporočil"},
    {"Settings & Tools", "Nastavitve in orodja"},
    {"Settings (advanced)", "Nastavitve (napredno)"},
    {"Locations Map", "Zemljevid lokacij"},
    {"no chats", "ni pogovorov"},
    {"Node Search", "Iskanje vozlišč"},
    {"Packet Statistics", "Statistika paketov"},
    {"Node Options", "Možnosti vozlišč"},
    {"Short Name", "Kratko ime"},
    {"Long Name", "Dolgo ime"},
    {"<unset>", "<prazno>"},
    {"LONG FAST\nLONG SLOW\n-- zastarelo --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO",
     "LONG FAST\nLONG SLOW\n-- zastarelo --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO"},
    {"Client\nClient Mute\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker",
     "Odjemalec\nNemi odjemalec\nSledilnik\nSenzor\nTAK\nSkriti odjemalec\nIzgubljeno - najdeno\nTAK sledilnik"},
    {"Brightness: 60%", "Svetlost: 60%"},
    {"Dark\nLight", "Temna\nSvetla"},
    {"Timeout: 60s", "Izklop zaslona po: 60s"},
    {"Screen Lock", "Zaklepanje zaslona"},
    {"Lock PIN", "PIN kodo"},
    {"Mouse", "Miš"},
    {"none", "nič"},
    {"Keyboard", "Tipkovnica"},
    {"Message Alert", "Opozorila za sporočila"},
    {"Ringtone", "Zvonenje"},
    {"Default", "Privzeto"},
    {"Zone", "Območje"},
    {"City", "Mesto"},
    {"NodeDB Reset\nFactory Reset\nClear Chat History",
     "Ponastavitev NodeDB\nTovarniška ponastavitev\nPočisti zgodovino klepeta"},
    {"Channel Name", "Ime kanala"},
    {"Pre-shared Key", "PSK ključ"},
    {"Unknown", "Neznano"},
    {"Offline", "Ni povezave"},
    {"Public Key", "Javni ključ"},
    {"Hops away", "Skoki"},
    {"MQTT", "MQTT"},
    {"Position", "Pozicija"},
    {"Name", "Ime"},
    {"Active Chat", "Aktiven pogovor"},
    {"Telemetry", "Telemetrija"},
    {"Start", "Start"},
    {"uptime 00:00:00", "prižgan 00:00:00"},
    {"Heap: 0\nLVGL: 0", "Kopica: 0\nLVGL: 0"},
    {"New Message from \n%s", "Novo sporočilo od\n%s"},
    {"Resynch...", "Sinhronizacija..."},
    {"OK", "OK"},
    {"Cancel", "Prekliči"},
    {"FrequencySlot: %d (%g MHz)", "Frekvenčno okno: %d (%g MHz)"},
    {"Brightness: %d%%", "Svetlost: %d%%"},
    {"Timeout: off", "Izklop zaslona: -"},
    {"Timeout: %ds", "Izklop zaslona: %ds"},
    {"Screen Calibration: %s", "Kalibracija zaslona %s"},
    {"done", "končano"},
    {"default", "privzeto"},
    {"Stop", "Stop"},
    {"choose\nnode", "izberi\nvozlišče"},
    {"choose target node", "izberi ciljno vozlišče"},
    {"heard: !%08x", "slišano: !%08x"},
    {"Packet Log: %d", "Dnevnik paketa: %d"},
    {"Language: %s", "Jezik: %s"},
    {"Screen Timeout: off", "Izklop zaslona po: -"},
    {"Screen Timeout: %ds", "Izklop zaslona po: %ds"},
    {"Screen Brightness: %d%%", "Svetlost: %d%%"},
    {"Theme: %s", "Tema: %s"},
    {"Region: %s", "Regija: %s"},
    {"Modem Preset: %s", "Modem: %s"},
    {"Channel: %s", "Ime kanala: %s"},
    {"Screen Lock: %s", "Zaklepanje zaslona: %s"},
    {"on", "vključeno"},
    {"off", "izključeno"},
    {"Message Alert: %s", "Opozorila za sporočila: %s"},
    {"hops: %d", "skoki: %d"},
    {"unknown", "neznano"},
    {"<no name>", "<brez imena>"},
    {"%d active chat(s)", "%d aktivnih pogovorov"},
    {"New message from \\n%s", "Novo sporočilo od \n%s"},
    {"Input Control: %s/%s", "Nadzor vnosa: %s/%s"},
    {"Filtering ...", "Filtriranje..."},
    {"Filter: %d of %d nodes", "Filter: %d od %d vozlišč"},
    {"now", "zdaj"},
    {"%d new message", "%d novih sporočil"},
    {"%d new messages", "%d novih sporočil"},
    {"uptime: %02d:%02d:%02d", "prižgan %02d:%02d:%02d"},
    {"Heap: %d (%d%%)\nLVGL: %d (%d%%)", "Kopica: %d (%d%%)\nLVGL: %d (%d%%)"},
    {"Settings", "Nastavitve"},
    {"Tools", "Orodja"},
    {"Filter", "Filter"},
    {"Highlight", "Označeno"},
    {"Primary Channel", "Primarni kanal"},
    {"Secondary Channels", "Sekundarni kanal"},
    {"Resynch ...", "Sinhronizacija ..."},
    {"Rebooting ...", "Ponovni zagon ..."},
    {"Shutting down ...", "Ugašanje ..."},
    {"silent", "tihi način"},
    {"WiFi: <not setup>", "WiFi: <ni nastavljen>"},
    {"LoRa TX off!", "LoRa TX ugasnjen!"},
    {"WiFi pre-shared Key", "WiFi ključ"},
    {"<not set>", "<ni nastavljen>"},
    {"Banner & Sound", "Pasica in zvok"},
    {"Banner only", "Samo pasica"},
    {"Sound only", "Samo zvok"},
    {"Lock: off/off", "Zaklep: -/-"},
    {"Settings Lock", "Zaklep nastavitev"},
    {"Lock: %s/%s", "Zaklep: %s/%s"},
    {"Enter Text ...", "Vnesi besedilo ..."},
    {"!Enter Filter ...", "!Vnesi filter ..."},
    {"Enter Filter ...", "Vnesi filter ..."},
    {"region unset", "regija ni nastavljena"},
    {"no signal", "ni signala"},
    {NULL, NULL} // End mark
};

static lv_i18n_phrase_t sl_plurals_one[] = {
    {"%d of %d nodes online", "1 vozlišč povezano"}, {NULL, NULL} // End mark
};

static lv_i18n_phrase_t sl_plurals_other[] = {
    {"%d of %d nodes online", "%d / %d vozl. povezanih"}, {NULL, NULL} // End mark
};

static uint8_t sl_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    UNUSED(n);
    uint32_t v = op_v(n);
    UNUSED(v);
    uint32_t i = op_i(n);
    UNUSED(i);
    uint32_t i100 = i % 100;
    if ((v == 0 && i100 == 1))
        return LV_I18N_PLURAL_TYPE_ONE;
    if ((v == 0 && i100 == 2))
        return LV_I18N_PLURAL_TYPE_TWO;
    if ((v == 0 && (3 <= i100 && i100 <= 4)) || (v != 0))
        return LV_I18N_PLURAL_TYPE_FEW;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t sl_lang = {.locale_name = "sl",
                                       .singulars = sl_singulars,
                                       .plurals[LV_I18N_PLURAL_TYPE_ONE] = sl_plurals_one,
                                       .plurals[LV_I18N_PLURAL_TYPE_OTHER] = sl_plurals_other,
                                       .locale_plural_fn = sl_plural_fn};

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
    uint32_t n = op_n(num);
    UNUSED(n);
    uint32_t v = op_v(n);
    UNUSED(v);
    uint32_t i = op_i(n);
    UNUSED(i);
    uint32_t f = op_f(n);
    UNUSED(f);
    uint32_t i10 = i % 10;
    uint32_t i100 = i % 100;
    uint32_t f10 = f % 10;
    uint32_t f100 = f % 100;
    if ((v == 0 && i10 == 1 && i100 != 11) || (f10 == 1 && f100 != 11))
        return LV_I18N_PLURAL_TYPE_ONE;
    if ((v == 0 && (2 <= i10 && i10 <= 4) && (!(12 <= i100 && i100 <= 14))) ||
        ((2 <= f10 && f10 <= 4) && (!(12 <= f100 && f100 <= 14))))
        return LV_I18N_PLURAL_TYPE_FEW;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t sr_lang = {.locale_name = "sr",
                                       .singulars = sr_singulars,

                                       .locale_plural_fn = sr_plural_fn};

static lv_i18n_phrase_t tr_singulars[] = {
    {"no new messages", "Yeni mesaj yok"},
    {"1 of 1 nodes online", "1/1 düğüm aktif"},
    {"uptime 00:00:00", "Çalışma süresi %s"},
    {"DEL", "Kapat"},
    {"User name: ", "Kullanıcı adı"},
    {"Modem Preset: LONG FAST", "Modem ön ayarı: LONG FAST"},
    {"Channel: LongFast", "Kanal: LongFast"},
    {"Role: Client", "Rol: Müşteri"},
    {"Screen Timeout: 60s", "Ekran süresi: 60 saniye"},
    {"Screen Lock: off", "Ekran kilidi: kapalı"},
    {"Screen Brightness: 60%", "Ekran parlaklığı: 60%"},
    {"Theme: Dark", "Tema: Karanlık"},
    {"Screen Calibration: default", "Ekran kalibrasyonu: varsayılan"},
    {"Input Control: none/none", "Giriş kontrolü: yok/yok"},
    {"Message Alert Buzzer: on", "Mesaj Uyarı Sesi: açık"},
    {"Language: English", "Dil: İngizce"},
    {"Configuration Reset", "Yapılandırma sıfırlaması"},
    {"Reboot / Shutdown", "Yeniden başlat / Kapat"},
    {"Mesh Detector", "Mesh dedektörü"},
    {"Signal Scanner", "Sinyal tarayıcısı"},
    {"Neighbors", "Komşular"},
    {"Statistics", "İstatistikler"},
    {"Packet Log", "Paket kaydı"},
    {"Group Channels", "Grup kanalları"},
    {"no messages", "Mesaj yok"},
    {"Settings & Tools", "Ayarlar & Araçlar"},
    {"Settings (advanced)", "Gelişmiş ayarlar"},
    {"Locations Map", "Harita lokasyonları"},
    {"no chats", "Sohbet yok"},
    {"Node Search", "Düğüm araması"},
    {"Packet Statistics", "Paket İstatistiği"},
    {"Node Options", "Düğüm seçenekleri"},
    {"Short Name", "Cihaz kısa ad"},
    {"Long Name", "Cihaz uzun ad"},
    {"<unset>", "<ayarlanmamış>"},
    {"Brightness: 60%", "Parlaklık: 60%"},
    {"Dark\nLight", "Karanlık\nAçık"},
    {"Timeout: 60s", "Zaman aşımı: 60s"},
    {"Screen Lock", "Ekran kilidi"},
    {"Lock PIN", "PIN kodunu \\n kitle"},
    {"Mouse", "Fare"},
    {"none", "Yok"},
    {"Keyboard", "Klavye"},
    {"Message Alert", "Mesaj uyarısı"},
    {"Ringtone", "Zil ses"},
    {"Default", "Varsayılan"},
    {"Zone", "Alan"},
    {"City", "Şehir"},
    {"NodeDB Reset\nFactory Reset", "NodeDB Sıfırlaması\nFabrika sıfırlaması"},
    {"Channel Name", "Kanal adı"},
    {"Pre-shared Key", "Önceden paylaşılan anahtar"},
    {"Unknown", "Bilinmeyen"},
    {"Offline", "Çevrimdışı"},
    {"Public Key", "Açık Anahtar"},
    {"Position", "Pozisyon"},
    {"Name", "Ad"},
    {"Active Chat", "Aktif sohbet"},
    {"Telemetry", "Telemetri"},
    {"Start", "Başla"},
    {"New Message from\n", "Yeni mesaj"},
    {"Resynch...", "Yeniden senkronize et"},
    {"OK", "Tamam"},
    {"Cancel", "İptal"},
    {"FrequencySlot: %d (%g MHz)", "Frekans Slotu: %d (%g MHz)"},
    {"Brightness: %d%%", "Parlaklık: %d%%"},
    {"Timeout: off", "Zaman aşımı: kapalı"},
    {"Timeout: %ds", "Zaman aşımı: %ds"},
    {"Screen Calibration: %s", "Ekran kalibrasyonu: %s"},
    {"done", "Bitti"},
    {"default", "Varsayılan"},
    {"Stop", "Dur"},
    {"choose\nnode", "Düğüm\nseç"},
    {"choose target node", "Hedef düğüm seç"},
    {"Packet Log: %d", "Paket kaydı: %d"},
    {"Language: %s", "Dil: %s"},
    {"Screen Timeout: off", "Ekran Zaman Aşımı: kapalı"},
    {"Screen Timeout: %ds", "Ekran Zaman Aşımı: %ds"},
    {"Screen Brightness: %d%%", "Ekran parlaklığı: %d%%"},
    {"Theme: %s", "Tema: %s"},
    {"User name: %s", "Kullanıcı adı: %s"},
    {"Device Role: %s", "Cihaz rolü: %s"},
    {"Region: %s", "Bölge: %s"},
    {"Modem Preset: %s", "Modem Ön Ayarı: %s"},
    {"Channel: %s", "Kanal: %s"},
    {"Screen Lock: %s", "Ekran kilidi: %s"},
    {"on", "Açık"},
    {"off", "Kapalı"},
    {"Message Alert: %s", "Mesaj Uyarısı: %s"},
    {"unknown", "Bilinmeyen"},
    {"<no name>", "<ad yok>"},
    {"%d active chat(s)", "%d aktif sohbet(ler)"},
    {"New message from \n%s", "Yeni mesaj: \n%s'den"},
    {"Input Control: %s/%s", "Giriş kontrolü: %s/%s"},
    {"Filtering ...", "Filtreleniyor ..."},
    {"Filter: %d of %d nodes", "Filtre: %d/%d"},
    {"now", "Şimdi"},
    {"%d new message", "Yeni %d mesaj"},
    {"%d new messages", "Yeni %d mesajlar"},
    {"uptime: %02d:%02d:%02d", "Çalışma süresi: %02d:%02d:%02d"},
    {"Settings", "Ayarlar"},
    {"Tools", "Araçlar"},
    {"Filter", "Filtre"},
    {"Highlight", "Vurgu"},
    {"Primary Channel", "Birinci kanallar"},
    {"Secondary Channels", "İkinci kanallar"},
    {"Resynch ...", "Yeniden senkronize et"},
    {"Rebooting ...", "Yeniden başlatılıyor ..."},
    {"Shutting down ...", "Kapatılıyor ..."},
    {"silent", "Sessiz"},
    {"WiFi: <not setup>", "WiFi: <kurulmamış>"},
    {"Lock: off/off", "Lock: kapalı/kapalı"},
    {"LoRa TX off!", "LoRa TX kapalı!"},
    {"WiFi SSID", "WiFi SSID"},
    {"WiFi pre-shared Key", "WiFi önceden paylaşılmış anahtar"},
    {"Settings Lock", "Ayarlar kilidi"},
    {"Lock: %s/%s", "Kilit: %s/%s"},
    {"Enter Text ...", "Metin gir..."},
    {"!Enter Filter ...", "!Filtre girin ..."},
    {"Enter Filter ...", "Filtre gir ..."},
    {"<not set>", "<ayarlanmamış>"},
    {"region unset", "Bölge ayarlanmamış"},
    {"Banner & Sound", "Afiş & ses"},
    {"Banner only", "Sadece afiş"},
    {"Sound only", "Sadece ses"},
    {"no signal", "Sinyal yok"},
    {"Restoring messages ...", "Mesajlar kurtarılıyor..."},
    {NULL, NULL} // End mark
};

static lv_i18n_phrase_t tr_plurals_one[] = {
    {"%d of %d nodes online", "1 düğüm çevrimiçi"}, {NULL, NULL} // End mark
};

static lv_i18n_phrase_t tr_plurals_other[] = {
    {"%d of %d nodes online", "%d / %d düğümler çevrimiçi"}, {NULL, NULL} // End mark
};

static uint8_t tr_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    UNUSED(n);

    if ((n == 1))
        return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t tr_lang = {.locale_name = "tr",
                                       .singulars = tr_singulars,
                                       .plurals[LV_I18N_PLURAL_TYPE_ONE] = tr_plurals_one,
                                       .plurals[LV_I18N_PLURAL_TYPE_OTHER] = tr_plurals_other,
                                       .locale_plural_fn = tr_plural_fn};

static lv_i18n_phrase_t zh_cn_singulars[] = {
    {"no new messages", "暂无新消息"},
    {"1 of 1 nodes online", "1/1 个节点在线"},
    {"DEL", "删除"},
    {"User name: ", "用户名："},
    {"Modem Preset: LONG FAST", "调制解调器预设：远距快速"},
    {"Channel: LongFast", "信道：远距快速"},
    {"Role: Client", "角色：客户端"},
    {"Screen Timeout: 60s", "屏幕超时：60秒"},
    {"Screen Brightness: 60%", "屏幕亮度：60%"},
    {"Screen Calibration: default", "屏幕校准：默认"},
    {"Input Control: none/none", "输入控制：无/无"},
    {"Message Alert Buzzer: on", "消息提醒蜂鸣：开"},
    {"Language: English", "语言：简体中文"},
    {"Configuration Reset", "重置配置"},
    {"Reboot / Shutdown", "重启/关机"},
    {"Group Channels", "群组信道"},
    {"no messages", "暂无消息"},
    {"Settings", "设置"},
    {"Settings (advanced)", "高级设置"},
    {"Locations Map", "位置地图"},
    {"no chats", "暂无聊天"},
    {"Node Options", "节点选项"},
    {"Short Name", "简称"},
    {"Long Name", "全称"},
    {"<unset>", "<未设置>"},
    {"FrequencySlot: 1 (902.0MHz)", "频段：1 (902.0MHz)"},
    {"Brightness: 60%", "亮度：60%"},
    {"Timeout: 60s", "超时：60秒"},
    {"Mouse", "鼠标"},
    {"Keyboard", "键盘"},
    {"Message Alert", "消息提醒"},
    {"NodeDB Reset\nFactory Reset", "节点数据库重置\n恢复出厂设置"},
    {"Channel Name", "信道名称"},
    {"Pre-shared Key", "预共享密钥"},
    {"Unknown", "未知"},
    {"Offline", "离线"},
    {"Public Key", "公钥"},
    {"Hops away", "跳数"},
    {"MQTT", "MQTT"},
    {"Position", "位置"},
    {"Name", "名称"},
    {"Active Chat", "活跃聊天"},
    {"Telemetry", "遥测"},
    {"IAQ", "空气质量"},
    {"Start", "开始"},
    {"Theme: Dark", "主题：深色"},
    {"Mesh Detector", "Mesh检测器"},
    {"Signal Scanner", "信号扫描"},
    {"Trace Route", "路由追踪"},
    {"Neighbors", "邻近节点"},
    {"Statistics", "统计信息"},
    {"Packet Log", "数据包日志"},
    {"Settings & Tools", "设置与工具"},
    {"Node Search", "节点搜索"},
    {"Screen Lock", "屏幕锁定"},
    {"Lock PIN", "锁定PIN码"},
    {"Ringtone", "铃声"},
    {"Zone", "区域"},
    {"City", "城市"},
    {"Theme: %s", "主题：%s"},
    {"uptime 00:00:00", "运行时间：00:00:00"},
    {"Heap: 0\nLVGL: 0", "堆内存：0\nLVGL：0"},
    {"Meshtastic", "Meshtastic"},
    {"Packet Statistics", "数据包统计"},
    {"LONG FAST\nLONG SLOW\n-- deprecated --\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE\nSHORT TURBO",
     "远距快速\n远距慢速\n-- 已弃用 --\n中距慢速\n中距快速\n近距慢速\n近距快速\n远距适中\n近距极速"},
    {"Client\nClient Mute\nRouter\n-- deprecated --\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker",
     "客户端\n静音客户端\n路由器\n-- 已弃用 --\n中继器\n追踪器\n传感器\nTAK\n隐藏客户端\n失物招领\nTAK追踪器"},
    {"Dark\nLight", "深色\n浅色"},
    {"none", "无"},
    {"Default", "默认"},
    {"OK", "确定"},
    {"Cancel", "取消"},
    {"FrequencySlot: %d (%g MHz)", "频段：%d (%g MHz)"},
    {"Brightness: %d%%", "亮度：%d%%"},
    {"Timeout: off", "超时：关闭"},
    {"Timeout: %ds", "超时：%d秒"},
    {"Stop", "停止"},
    {"choose\nnode", "选择\n节点"},
    {"choose target node", "选择目标节点"},
    {"heard: !%08x", "已收到：!%08x"},
    {"Packet Log: %d", "数据包日志：%d"},
    {"Screen Timeout: off", "屏幕超时：关闭"},
    {"Screen Timeout: %ds", "屏幕超时：%d秒"},
    {"Screen Brightness: %d%%", "屏幕亮度：%d%%"},
    {"Screen Lock: %s", "屏幕锁定：%s"},
    {"on", "开启"},
    {"off", "关闭"},
    {"Message Alert: %s", "消息提醒：%s"},
    {"Util %0.1f%%  Air %0.1f%%", "使用率 %0.1f%%  空中占用 %0.1f%%"},
    {"hops: %d", "跳数：%d"},
    {"unknown", "未知"},
    {"<no name>", "<无名称>"},
    {"%d active chat(s)", "%d 个活跃聊天"},
    {"New message from \n%s", "新消息来自：\n%s"},
    {"Input Control: %s/%s", "输入控制：%s/%s"},
    {"%d of %d nodes online", "%d/%d 个节点在线"},
    {"Filtering ...", "正在过滤..."},
    {"Filter: %d of %d nodes", "过滤：%d/%d 个节点"},
    {"now", "现在"},
    {"%d new message", "%d 条新消息"},
    {"%d new messages", "%d 条新消息"},
    {"uptime: %02d:%02d:%02d", "运行时间：%02d:%02d:%02d"},
    {"Heap: %d (%d%%)\nLVGL: %d (%d%%)", "堆内存：%d (%d%%)\nLVGL：%d (%d%%)"},
    {"Tools", "工具"},
    {"Filter", "过滤"},
    {"Highlight", "高亮"},
    {"choose node", "选择节点"},
    {"Primary Channel", "主信道"},
    {"Secondary Channels", "辅助信道"},
    {"Resynch ...", "正在重新同步..."},
    {"Rebooting ...", "正在重启..."},
    {"Shutting down ...", "正在关机..."},
    {"LoRa 0.0 MHz", "LoRa 0.0 MHz"},
    {"silent", "静音"},
    {"WiFi: <not setup>", "WiFi：<未设置>"},
    {"LoRa TX off!", "LoRa发送已关闭！"},
    {"WiFi SSID", "WiFi名称"},
    {"WiFi pre-shared Key", "WiFi预共享密钥"},
    {"WiFi: %s", "WiFi：%s"},
    {"<not set>", "<未设置>"},
    {"Banner & Sound", "横幅和声音"},
    {"Banner only", "仅横幅"},
    {"Sound only", "仅声音"},
    {"Lock: off/off", "锁定：关/关"},
    {"Settings Lock", "设置锁定"},
    {"Lock: %s/%s", "锁定：%s/%s"},
    {"Enter Text ...", "输入文字..."},
    {"!Enter Filter ...", "!输入过滤条件..."},
    {"Enter Filter ...", "输入过滤条件..."},
    {"region unset", "区域未设置"},
    {"no signal", "无信号"},
    {NULL, NULL} // End mark
};

static uint8_t zh_cn_plural_fn(int32_t num)
{

    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t zh_cn_lang = {.locale_name = "zh-CN",
                                          .singulars = zh_cn_singulars,

                                          .locale_plural_fn = zh_cn_plural_fn};

const lv_i18n_language_pack_t lv_i18n_language_pack[] = {
    &en_lang, &de_lang, &es_lang, &fi_lang, &fr_lang, &it_lang, &nl_lang, &no_lang,    &pl_lang,
    &pt_lang, &ro_lang, &ru_lang, &se_lang, &sl_lang, &sr_lang, &tr_lang, &zh_cn_lang,
    NULL // End mark
};

////////////////////////////////////////////////////////////////////////////////

// Internal state
static const lv_i18n_language_pack_t *current_lang_pack;
static const lv_i18n_lang_t *current_lang;

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
int lv_i18n_init(const lv_i18n_language_pack_t *langs)
{
    if (langs == NULL)
        return -1;
    if (langs[0] == NULL)
        return -1;

    current_lang_pack = langs;
    current_lang = langs[0]; /*Automatically select the first language*/
    return 0;
}

/**
 * Change the localization (language)
 * @param l_name name of the translation locale to use. E.g. "en-GB"
 */
int lv_i18n_set_locale(const char *l_name)
{
    if (current_lang_pack == NULL)
        return -1;

    uint16_t i;

    for (i = 0; current_lang_pack[i] != NULL; i++) {
        // Found -> finish
        if (strcmp(current_lang_pack[i]->locale_name, l_name) == 0) {
            current_lang = current_lang_pack[i];
            return 0;
        }
    }

    return -1;
}

static const char *__lv_i18n_get_text_core(lv_i18n_phrase_t *trans, const char *msg_id)
{
    uint16_t i;
    for (i = 0; trans[i].msg_id != NULL; i++) {
        if (strcmp(trans[i].msg_id, msg_id) == 0) {
            /*The msg_id has found. Check the translation*/
            if (trans[i].translation)
                return trans[i].translation;
        }
    }

    return NULL;
}

/**
 * Get the translation from a message ID
 * @param msg_id message ID
 * @return the translation of `msg_id` on the set local
 */
const char *lv_i18n_get_text(const char *msg_id)
{
    if (current_lang == NULL)
        return msg_id;

    const lv_i18n_lang_t *lang = current_lang;
    const void *txt;

    // Search in current locale
    if (lang->singulars != NULL) {
        txt = __lv_i18n_get_text_core(lang->singulars, msg_id);
        if (txt != NULL)
            return txt;
    }

    // Try to fallback
    if (lang == current_lang_pack[0])
        return msg_id;
    lang = current_lang_pack[0];

    // Repeat search for default locale
    if (lang->singulars != NULL) {
        txt = __lv_i18n_get_text_core(lang->singulars, msg_id);
        if (txt != NULL)
            return txt;
    }

    return msg_id;
}

/**
 * Get the translation from a message ID and apply the language's plural rule to get correct form
 * @param msg_id message ID
 * @param num an integer to select the correct plural form
 * @return the translation of `msg_id` on the set local
 */
const char *lv_i18n_get_text_plural(const char *msg_id, int32_t num)
{
    if (current_lang == NULL)
        return msg_id;

    const lv_i18n_lang_t *lang = current_lang;
    const void *txt;
    lv_i18n_plural_type_t ptype;

    // Search in current locale
    if (lang->locale_plural_fn != NULL) {
        ptype = lang->locale_plural_fn(num);

        if (lang->plurals[ptype] != NULL) {
            txt = __lv_i18n_get_text_core(lang->plurals[ptype], msg_id);
            if (txt != NULL)
                return txt;
        }
    }

    // Try to fallback
    if (lang == current_lang_pack[0])
        return msg_id;
    lang = current_lang_pack[0];

    // Repeat search for default locale
    if (lang->locale_plural_fn != NULL) {
        ptype = lang->locale_plural_fn(num);

        if (lang->plurals[ptype] != NULL) {
            txt = __lv_i18n_get_text_core(lang->plurals[ptype], msg_id);
            if (txt != NULL)
                return txt;
        }
    }

    return msg_id;
}

/**
 * Get the name of the currently used locale.
 * @return name of the currently used locale. E.g. "en-GB"
 */
const char *lv_i18n_get_current_locale(void)
{
    if (!current_lang)
        return NULL;
    return current_lang->locale_name;
}
