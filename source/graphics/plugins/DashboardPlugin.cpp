#include "graphics/plugin/DashboardPlugin.h"
#include "Arduino.h"
#include "graphics/common/LoRaPresets.h"
#include "images.h"
#include "lv_i18n.h"
#include "lvgl.h"
#include <cstdio>
#include <ctime>

#ifndef DASHBOARD_PLUGIN_CUSTOM_WIDGET_NAMES
#include "screens.h"
#endif

static DashboardPlugin *p = nullptr;

DashboardPlugin::DashboardPlugin() : GfxPlugin("Dashboard") {}

DashboardPlugin::~DashboardPlugin() = default;

void DashboardPlugin::init(lv_obj_t *parent, WidgetResolver resolver, std::size_t widgetCount, lv_group_t *group,
                           lv_indev_t *indev, GfxPlugin::RegisterWidget registerWidget)
{
    p = this;
    GfxPlugin::init(parent, resolver, widgetCount, group, indev, registerWidget);
}

void DashboardPlugin::loadScreen(lv_screen_load_anim_t anim, uint32_t time)
{
    GfxPlugin::loadScreen(anim, time);
}

void DashboardPlugin::registerStandardWidgets(void)
{
#ifndef DASHBOARD_PLUGIN_CUSTOM_WIDGET_NAMES
    // register default widgets that were created by the generated UI
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::MailButton), objects.home_mail_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::MailLabel), objects.home_mail_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::NodesButton), objects.home_nodes_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::NodesLabel), objects.home_nodes_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::TimeButton), objects.home_time_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::TimeLabel), objects.home_time_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::LoRaButton), objects.home_lora_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::LoRaLabel), objects.home_lora_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::SignalButton), objects.home_signal_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::SignalLabel), objects.home_signal_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::SignalPctLabel), objects.home_signal_pct_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::BellButton), objects.home_bell_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::BellLabel), objects.home_bell_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::WlanButton), objects.home_wlan_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::WlanLabel), objects.home_wlan_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::MqttButton), objects.home_mqtt_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::MqttLabel), objects.home_mqtt_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::SdButton), objects.home_sd_card_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::SdLabel), objects.home_sd_card_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::MemoryButton), objects.home_memory_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::MemoryLabel), objects.home_memory_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::QrButton), objects.home_qr_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::QrLabel), objects.home_qr_label);
#endif
}

void DashboardPlugin::registerStandardWidgetActions(void)
{
    registerWidgetAction(static_cast<WidgetIndex>(Widget::MailButton), static_cast<GfxPlugin::Action>(Action::OpenMessages));
    registerWidgetAction(static_cast<WidgetIndex>(Widget::NodesButton), static_cast<GfxPlugin::Action>(Action::OpenNodes));
    registerWidgetAction(static_cast<WidgetIndex>(Widget::TimeButton), static_cast<GfxPlugin::Action>(Action::ToggleTime));
    registerWidgetAction(static_cast<WidgetIndex>(Widget::LoRaButton), static_cast<GfxPlugin::Action>(Action::ToggleLoRa));
    registerWidgetAction(static_cast<WidgetIndex>(Widget::BellButton),
                         static_cast<GfxPlugin::Action>(Action::ToggleNotifications));
    registerWidgetAction(static_cast<WidgetIndex>(Widget::LocationButton), static_cast<GfxPlugin::Action>(Action::ToggleGPS));
    registerWidgetAction(static_cast<WidgetIndex>(Widget::WlanButton), static_cast<GfxPlugin::Action>(Action::ToggleWLAN));
    registerWidgetAction(static_cast<WidgetIndex>(Widget::MqttButton), static_cast<GfxPlugin::Action>(Action::ToggleMQTT));
    registerWidgetAction(static_cast<WidgetIndex>(Widget::SdButton), static_cast<GfxPlugin::Action>(Action::RefreshSD));
    registerWidgetAction(static_cast<WidgetIndex>(Widget::MemoryButton), static_cast<GfxPlugin::Action>(Action::ToggleMem));
    registerWidgetAction(static_cast<WidgetIndex>(Widget::QrButton), static_cast<GfxPlugin::Action>(Action::ToggleQR));
}

void DashboardPlugin::registerStandardEventCallbacks(void)
{
    lv_obj_t *mail_button = p->getWidget(static_cast<WidgetIndex>(Widget::MailButton));
    if (mail_button)
        lv_obj_add_event_cb(mail_button, this->ui_event_button, LV_EVENT_ALL, (void *)&onOpenMessages);

    lv_obj_t *nodes_button = p->getWidget(static_cast<WidgetIndex>(Widget::NodesButton));
    if (nodes_button)
        lv_obj_add_event_cb(nodes_button, this->ui_event_button, LV_EVENT_ALL, (void *)&onOpenNodes);

    lv_obj_t *time_button = p->getWidget(static_cast<WidgetIndex>(Widget::TimeButton));
    if (time_button)
        lv_obj_add_event_cb(time_button, this->ui_event_button, LV_EVENT_ALL, (void *)&onToggleTime);

    lv_obj_t *lora_button = p->getWidget(static_cast<WidgetIndex>(Widget::LoRaButton));
    if (lora_button)
        lv_obj_add_event_cb(lora_button, this->ui_event_button, LV_EVENT_ALL, (void *)&onToggleLoRa);

    lv_obj_t *bell_button = p->getWidget(static_cast<WidgetIndex>(Widget::BellButton));
    if (bell_button)
        lv_obj_add_event_cb(bell_button, this->ui_event_button, LV_EVENT_ALL, (void *)&onToggleSound);

    lv_obj_t *location_button = p->getWidget(static_cast<WidgetIndex>(Widget::LocationButton));
    if (location_button)
        lv_obj_add_event_cb(location_button, this->ui_event_button, LV_EVENT_ALL, (void *)&onToggleGPS);

    lv_obj_t *wlan_button = p->getWidget(static_cast<WidgetIndex>(Widget::WlanButton));
    if (wlan_button)
        lv_obj_add_event_cb(wlan_button, this->ui_event_button, LV_EVENT_ALL, (void *)&onToggleWLAN);

    lv_obj_t *mqtt_button = p->getWidget(static_cast<WidgetIndex>(Widget::MqttButton));
    if (mqtt_button)
        lv_obj_add_event_cb(mqtt_button, this->ui_event_button, LV_EVENT_ALL, (void *)&onToggleMQTT);

    lv_obj_t *sd_button = p->getWidget(static_cast<WidgetIndex>(Widget::SdButton));
    if (sd_button)
        lv_obj_add_event_cb(sd_button, this->ui_event_button, LV_EVENT_ALL, (void *)&onRefreshSDCard);

    lv_obj_t *memory_button = p->getWidget(static_cast<WidgetIndex>(Widget::MemoryButton));
    if (memory_button)
        lv_obj_add_event_cb(memory_button, this->ui_event_button, LV_EVENT_ALL, (void *)&onRefreshSDCard);

    lv_obj_t *qr_button = p->getWidget(static_cast<WidgetIndex>(Widget::QrButton));
    if (qr_button)
        lv_obj_add_event_cb(qr_button, this->ui_event_button, LV_EVENT_ALL, (void *)&onRefreshSDCard);
}

void DashboardPlugin::ui_event_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == EVENT_CLICKED) {
        DashboardPlugin::Callback *onPress = (DashboardPlugin::Callback *)(lv_event_get_user_data(e));
        if (onPress && *onPress)
            (*onPress)(e);
    }
}

void DashboardPlugin::updateTime(uint32_t unixTime, bool use_12h_clock)
{
    lv_obj_t *timeLbl = getWidget(static_cast<WidgetIndex>(Widget::TimeLabel));
    if (timeLbl) {
        time_t t = (time_t)unixTime;
        tm *curr_tm = localtime(&t);
        char buf[80];
        if (t > 1000000) {
            if (use_12h_clock) {
                strftime(buf, sizeof(buf), "%I:%M:%S %p\n%a %d-%b-%g", curr_tm);
            } else {
                strftime(buf, sizeof(buf), "%T %Z%z\n%a %d-%b-%g", curr_tm);
            }
        } else {
            // show uptime if no valid time
            uint32_t uptime_s = millis() / 1000;
            int hours = uptime_s / 3600;
            uptime_s -= hours * 3600;
            int minutes = uptime_s / 60;
            int seconds = uptime_s - minutes * 60;
            snprintf(buf, sizeof(buf), "uptime: %02d:%02d:%02d", hours, minutes, seconds);
        }
        lv_label_set_text(timeLbl, buf);
    }
}

void DashboardPlugin::updateLoRaConfig(const meshtastic_Config_LoRaConfig &cfg)
{
    // LoRa label
    lv_obj_t *loraLbl = getWidget(static_cast<WidgetIndex>(Widget::LoRaLabel));
    if (loraLbl) {
        char loraFreq[96];
        // The shared preset table predates newer firmware enum values. Preserve
        // their config while avoiding an out-of-range table lookup in the UI.
        if (cfg.region == meshtastic_Config_LoRaConfig_RegionCode_UNSET) {
            snprintf(loraFreq, sizeof(loraFreq), "%s", _("Radio: Region unset"));
        } else if (!cfg.use_preset || cfg.region > 22 || cfg.modem_preset > 8) {
            snprintf(loraFreq, sizeof(loraFreq), "%s\n%s", cfg.tx_enabled ? _("Radio: On") : _("Radio: Off"),
                     _("Current radio configuration"));
        } else {
            const float frequency =
                LoRaPresets::getRadioFreq(cfg.region, cfg.modem_preset, cfg.channel_num) + cfg.frequency_offset;
            snprintf(loraFreq, sizeof(loraFreq), "%s\nLoRa %g MHz [%s kHz]", cfg.tx_enabled ? _("Radio: On") : _("Radio: Off"),
                     frequency, LoRaPresets::getBandwidthString(cfg.modem_preset));
        }
        lv_label_set_text(loraLbl, loraFreq);
        // Themes::recolorButton(objects.home_lora_button, cfg.tx_enabled);
        // Themes::recolorText(loraLbl, cfg.tx_enabled);
#if 0 // TODO
        if (!cfg.tx_enabled) {
            lv_obj_clear_flag(objects.top_lora_tx_panel, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(objects.top_lora_tx_panel, LV_OBJ_FLAG_HIDDEN);
        }
#endif
    }
}

void DashboardPlugin::updateSignalStrength(int32_t rssi, float snr)
{
    lv_obj_t *sigLbl = getWidget(static_cast<WidgetIndex>(Widget::SignalLabel));
    lv_obj_t *sigPct = getWidget(static_cast<WidgetIndex>(Widget::SignalPctLabel));
    lv_obj_t *sigBtn = getWidget(static_cast<WidgetIndex>(Widget::SignalButton));
    if (sigLbl && sigPct && sigBtn) {
        if (rssi != 0 || snr != 0.0f) {
            char buf[64];
            snprintf(buf, sizeof(buf), "SNR: %.1f\nRSSI: %" PRId32, snr, rssi);
            lv_label_set_text(sigLbl, buf);
            int pct = signalStrength2Percent(rssi, snr);
            snprintf(buf, sizeof(buf), "(%d%%)", pct);
            lv_label_set_text(sigPct, buf);
#if 0
            if (pct > 80)
                lv_obj_set_style_bg_image_src(sigBtn, &img_home_signal_icon, LV_PART_MAIN | LV_STATE_DEFAULT);
            else if (pct > 60)
                lv_obj_set_style_bg_image_src(sigBtn, &img_home_signal_strong_icon, LV_PART_MAIN | LV_STATE_DEFAULT);
            else if (pct > 40)
                lv_obj_set_style_bg_image_src(sigBtn, &img_home_signal_good_icon, LV_PART_MAIN | LV_STATE_DEFAULT);
            else if (pct > 20)
                lv_obj_set_style_bg_image_src(sigBtn, &img_home_signal_fair_icon, LV_PART_MAIN | LV_STATE_DEFAULT);
            else if (pct > 1)
                lv_obj_set_style_bg_image_src(sigBtn, &img_home_signal_weak_icon, LV_PART_MAIN | LV_STATE_DEFAULT);
            else
                lv_obj_set_style_bg_image_src(sigBtn, &img_home_no_signal_icon, LV_PART_MAIN | LV_STATE_DEFAULT);
#endif
        }
    }
}

void DashboardPlugin::updatePosition(int32_t lat, int32_t lon, int32_t alt, uint32_t sats, uint32_t precision, bool metric)
{
    lv_obj_t *locationLbl = getWidget(static_cast<WidgetIndex>(Widget::LocationLabel));
    if (locationLbl) {
        int32_t altU = abs(alt) < 10000 ? alt : 0;
        char units[3] = {};
        if (metric) {
            units[0] = 'm';
        } else {
            units[0] = 'f';
            units[1] = 't';
            altU = int32_t(float(altU) * 3.28084);
        }

        char buf[64];
        int latSeconds = (int)round(lat * 1e-7 * 3600);
        int latDegrees = latSeconds / 3600;
        latSeconds = abs(latSeconds % 3600);
        int latMinutes = latSeconds / 60;
        latSeconds %= 60;
        char latLetter = (lat > 0) ? 'N' : 'S';

        int lonSeconds = (int)round(lon * 1e-7 * 3600);
        int lonDegrees = lonSeconds / 3600;
        lonSeconds = abs(lonSeconds % 3600);
        int lonMinutes = lonSeconds / 60;
        lonSeconds %= 60;
        char lonLetter = (lon > 0) ? 'E' : 'W';

        if (sats)
            sprintf(buf, "%c%02i° %2i'%02i\"   %u sats\n%c%02i° %2i'%02i\"   %d%s", latLetter, abs(latDegrees), latMinutes,
                    latSeconds, sats, lonLetter, abs(lonDegrees), lonMinutes, lonSeconds, altU, units);
        else
            sprintf(buf, "%c%02i° %2i'%02i\"\n%c%02i° %2i'%02i\"   %d%s", latLetter, abs(latDegrees), latMinutes, latSeconds,
                    lonLetter, abs(lonDegrees), lonMinutes, lonSeconds, altU, units);

        lv_label_set_text(locationLbl, buf);
    }
}

void DashboardPlugin::updateSDCard(bool cardDetected, const char *info)
{
    // SD label
    lv_obj_t *sdLbl = getWidget(static_cast<WidgetIndex>(Widget::SdLabel));
    if (sdLbl) {
        lv_label_set_text(sdLbl, info);
    }
}

void DashboardPlugin::updateNetworkConfig(const meshtastic_Config_NetworkConfig &cfg)
{
    networkEnabled = cfg.wifi_enabled || cfg.eth_enabled;
    renderConnectionStatus();
}

void DashboardPlugin::updateMQTTConfig(const meshtastic_ModuleConfig_MQTTConfig &cfg)
{
    mqttEnabled = cfg.enabled;
    renderConnectionStatus();
}

void DashboardPlugin::updateConnectionStatus(const meshtastic_DeviceConnectionStatus &status)
{
    connectionStatus = status;
    renderConnectionStatus();
}

void DashboardPlugin::renderConnectionStatus()
{
    const bool connected =
        connectionStatus.has_wifi && connectionStatus.wifi.has_status && connectionStatus.wifi.status.is_connected;
    auto *label = getWidget(static_cast<WidgetIndex>(Widget::WlanLabel));
    if (label) {
        if (networkEnabled && connected) {
            const uint32_t ip = connectionStatus.wifi.status.ip_address;
            lv_label_set_text_fmt(label, "%s\n%u.%u.%u.%u", _("Wi-Fi: Connected"), ip & 0xff, (ip >> 8) & 0xff, (ip >> 16) & 0xff,
                                  (ip >> 24) & 0xff);
        } else {
            lv_label_set_text(label, networkEnabled ? _("Wi-Fi: On, disconnected") : _("Wi-Fi: Off"));
        }
    }
    auto *button = getWidget(static_cast<WidgetIndex>(Widget::WlanButton));
    if (button)
        lv_obj_set_style_bg_image_src(button, connected ? &img_home_wlan_icon : &img_home_wlan_off_icon, 0);
    label = getWidget(static_cast<WidgetIndex>(Widget::MqttLabel));
    if (label) {
        const bool mqttConnected = connected && connectionStatus.wifi.status.is_mqtt_connected;
        lv_label_set_text(label, !mqttEnabled    ? _("MQTT: Off")
                                 : mqttConnected ? _("MQTT: Connected")
                                                 : _("MQTT: On, disconnected"));
    }
}

void DashboardPlugin::updateNotifications(bool enabled)
{
    notificationsEnabled = enabled;
    renderNotifications();
}

void DashboardPlugin::updateSound(bool enabled)
{
    soundEnabled = enabled;
    soundKnown = true;
    renderNotifications();
}

void DashboardPlugin::renderNotifications()
{
    auto *label = getWidget(static_cast<WidgetIndex>(Widget::BellLabel));
    if (!label)
        return;
    const char *popups = notificationsEnabled ? _("Message popups: On") : _("Message popups: Off");
    if (soundKnown)
        lv_label_set_text_fmt(label, "%s\n%s", popups, soundEnabled ? _("Sound: On") : _("Sound: Off"));
    else
        lv_label_set_text(label, popups);
}


void DashboardPlugin::updateFreeMem(uint32_t freeHeapBytes, uint32_t lvglFreeBytes)
{
    // memory
    lv_obj_t *memLbl = getWidget(static_cast<WidgetIndex>(Widget::MemoryLabel));
    if (memLbl) {
        char buf[64];
        lv_mem_monitor_t mon;
        lv_mem_monitor(&mon);
        snprintf(buf, sizeof(buf), "Heap: %u\nLVGL: %u", (unsigned)freeHeapBytes, (unsigned)lvglFreeBytes);
        lv_label_set_text(memLbl, buf);
    }
}

void DashboardPlugin::updateNodesStatus(uint32_t online, uint32_t total)
{
    lv_obj_t *nodesLbl = getWidget(static_cast<WidgetIndex>(Widget::NodesLabel));
    if (nodesLbl)
        lv_label_set_text_fmt(nodesLbl, _p("%u of %u nodes online", total), online, total);

    // if (nodesFiltered)
    //     lv_snprintf(buf, sizeof(buf), _("Filter: %d of %d nodes"), nodeCount - nodesFiltered, nodeCount);
    // lv_label_set_text(objects.top_nodes_online_label, buf);
}

int32_t DashboardPlugin::signalStrength2Percent(int32_t rx_rssi, float rx_snr)
{
#if defined(USE_SX127x)
    int p_snr = ((std::max<int32_t>(rx_snr, -19.0f) + 19.0f) / 33.0f) * 100.0f; // range -19..14
    int p_rssi = ((std::max<int32_t>(rx_rssi, -145L) + 145) * 100) / 90;        // range -145..-55
#else
    int p_snr = ((std::max<int32_t>(rx_snr, -18.0f) + 18.0f) / 26.0f) * 100.0f; // range -18..8
    int p_rssi = ((std::max<int32_t>(rx_rssi, -125) + 125) * 100) / 100;        // range -125..-25
#endif
    return std::min<int32_t>((p_snr + p_rssi * 2) / 3, 100);
}

void DashboardPlugin::handleAction(Action actionId, WidgetIndex /*idx*/, int /*event_code*/)
{
    switch (actionId) {
#if 0 // not used
    case Action::OpenMessages:
        if (onOpenMessages) onOpenMessages();
        break;
    case Action::OpenNodes:
        if (onOpenNodes) onOpenNodes();
        break;
    case Action::ToggleTime:
        if (onToggleTime) onToggleTime();
        break;
#endif
    default:
        invokeActionCallback(static_cast<GfxPlugin::Action>(actionId), nullptr);
        break;
    }
}