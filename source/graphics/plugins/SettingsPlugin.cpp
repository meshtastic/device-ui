#include "graphics/plugin/Plugins.h"

#ifdef MUI_SETTINGS_PLUGIN

#include "graphics/common/LoRaPresets.h"
#include "graphics/common/ViewController.h"
#include "graphics/plugin/ListRowStyle.h"
#include "graphics/plugin/SettingsPlugin.h"
#include "input/InputDriver.h"
#include "lv_i18n.h"
#include "util/ISpiLock.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#if defined(ARCH_PORTDUINO)
#include "PortduinoFS.h"
#else
#include "LittleFS.h"
#endif
extern fs::FS &fileSystem;

namespace
{
constexpr uintptr_t Confirm = 1;
constexpr uintptr_t Cancel = 2;
constexpr uintptr_t SoundSettings = 3;
constexpr uintptr_t FirstEditor = 10;

void removeFromGroup(lv_obj_t *object)
{
    lv_group_remove_obj(object);
    for (uint32_t i = 0; i < lv_obj_get_child_count(object); ++i)
        removeFromGroup(lv_obj_get_child(object, i));
}

bool checked(lv_obj_t *obj)
{
    return lv_obj_has_state(obj, LV_STATE_CHECKED);
}
} // namespace

SettingsPlugin::SettingsPlugin(ViewController &controller, InputDriver &input, meshtastic_DeviceUIConfig &ui,
                               meshtastic_LocalConfig &config, meshtastic_LocalModuleConfig &modules)
    : GfxPlugin("Settings"), controller(controller), input(input), ui(ui), config(config), modules(modules)
{
}

SettingsPlugin::~SettingsPlugin()
{
    clearDialog(false);
}

void SettingsPlugin::configure(lv_obj_t *screen, lv_obj_t *panel, lv_group_t *settingsGroup, lv_indev_t *primaryInput)
{
    GfxPlugin::init(screen, nullptr, 0, settingsGroup, primaryInput, RegisterWidget::Nothing);
    // Keep the generated screen/header, replacing unfinished settings contents.
    for (uint32_t i = 0; i < lv_obj_get_child_count(panel); ++i) {
        auto *child = lv_obj_get_child(panel, i);
        removeFromGroup(child);
        lv_obj_add_flag(child, LV_OBJ_FLAG_HIDDEN);
    }
    lv_obj_remove_flag(panel, LV_OBJ_FLAG_HIDDEN);
    ListRowStyle::container(panel);
    const char *labels[] = {_("Wi-Fi"),          _("GPS"),   _("Radio"),   _("MQTT"),
                            _("Message popups"), _("Sound"), _("Display"), _("Input")};
    auto *previousGroup = lv_group_get_default();
    lv_group_set_default(settingsGroup);
    for (uint8_t i = 0; i < sizeof(labels) / sizeof(labels[0]); ++i) {
        auto *row = lv_button_create(panel);
        ListRowStyle::row(row);
        lv_obj_set_user_data(row, reinterpret_cast<void *>(FirstEditor + i));
        lv_obj_add_event_cb(row, event, LV_EVENT_ALL, this);
        auto *label = lv_label_create(row);
        lv_label_set_text(label, labels[i]);
        lv_obj_align(label, LV_ALIGN_LEFT_MID, 8, 0);
        ListRowStyle::text(label);
    }
    lv_group_set_default(previousGroup);
    ISpiLock::Guard bus;
    File file = fileSystem.open("/prefs/double-space-period", FILE_READ);
    doubleSpacePeriod = file && file.size() == 1 && file.read() == '1';
    file.close();
}

void SettingsPlugin::captureOrigin()
{
    lv_indev_t *devices[] = {indev, input.getKeyboard(), input.getEncoder(), input.getButton()};
    for (size_t i = 0; i < origin.size(); ++i) {
        origin[i].device = devices[i];
        origin[i].group = devices[i] ? lv_indev_get_group(devices[i]) : nullptr;
        origin[i].focus = origin[i].group ? lv_group_get_focused(origin[i].group) : nullptr;
    }
}

void SettingsPlugin::loadScreen(lv_screen_load_anim_t anim, uint32_t time)
{
    if (dialog)
        return;
    if (ui.settings_lock) {
        captureOrigin();
        requestPIN(true);
        return;
    }
    GfxPlugin::loadScreen(anim, time);
    if (input.getKeyboard())
        lv_indev_set_group(input.getKeyboard(), group);
}

void SettingsPlugin::open(Editor requested)
{
    if (dialog)
        return;
    editor = requested;
    captureOrigin();
    if (ui.settings_lock)
        requestPIN(false);
    else
        createEditor();
}

void SettingsPlugin::unlockScreen(const std::function<void()> &onSuccess)
{
    if (screenUnlock)
        return;
    clearDialog(true);
    captureOrigin();
    screenUnlock = true;
    unlocked = onSuccess;
    requestPIN(false);
}

void SettingsPlugin::cancel()
{
    if (screenUnlock) {
        lv_textarea_set_text(fields[0], "");
        lv_group_focus_obj(fields[0]);
        return;
    }
    clearDialog(true);
}

void SettingsPlugin::attachInput(lv_obj_t *obj)
{
    lv_group_add_obj(dialogGroup, obj);
    ListRowStyle::focus(obj);
    lv_obj_add_event_cb(obj, event, LV_EVENT_ALL, this);
}

void SettingsPlugin::createDialog(const char *title)
{
    dialogGroup = lv_group_create();
    auto *previousGroup = lv_group_get_default();
    lv_group_set_default(nullptr);
    dialog = lv_obj_create(lv_layer_top());
    lv_obj_set_size(dialog, LV_PCT(100), LV_PCT(100));
    lv_obj_center(dialog);
    lv_obj_set_style_pad_all(dialog, 6, 0);
    lv_obj_set_style_pad_row(dialog, 4, 0);
    lv_obj_set_flex_flow(dialog, LV_FLEX_FLOW_COLUMN);
    ListRowStyle::panel(dialog);
    lv_obj_remove_flag(dialog, LV_OBJ_FLAG_SCROLLABLE);
    auto *heading = lv_label_create(dialog);
    lv_label_set_text(heading, title);
    body = lv_obj_create(dialog);
    lv_obj_set_width(body, LV_PCT(100));
    lv_obj_set_flex_grow(body, 1);
    lv_obj_set_style_pad_all(body, 4, 0);
    lv_obj_set_style_pad_row(body, 5, 0);
    lv_obj_set_style_bg_opa(body, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(body, 0, 0);
    lv_obj_set_flex_flow(body, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_dir(body, LV_DIR_VER);
    lv_obj_set_style_pad_right(body, 1, LV_PART_SCROLLBAR);
    errorLabel = lv_label_create(dialog);
    lv_obj_set_width(errorLabel, LV_PCT(100));
    lv_label_set_long_mode(errorLabel, LV_LABEL_LONG_WRAP);
    lv_obj_add_flag(errorLabel, LV_OBJ_FLAG_HIDDEN);
    auto *footer = lv_obj_create(dialog);
    lv_obj_remove_style_all(footer);
    lv_obj_set_size(footer, LV_PCT(100), 34);
    lv_obj_set_flex_flow(footer, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(footer, 8, 0);
    for (uintptr_t action : {Confirm, Cancel}) {
        auto *button = lv_button_create(footer);
        lv_obj_set_size(button, 90, 32);
        lv_obj_add_style(button, &ListRowStyle::baseStyle(), 0);
        lv_obj_set_style_bg_color(button, lv_color_hex(0x287bd0), LV_STATE_PRESSED);
        lv_obj_set_user_data(button, reinterpret_cast<void *>(action));
        attachInput(button);
        // Confirmation controls use a distinct fill, not another similar blue.
        for (lv_style_selector_t state : {LV_STATE_FOCUSED, LV_STATE_FOCUS_KEY, LV_STATE_EDITED}) {
            lv_obj_set_style_bg_color(button, lv_color_hex(0x67ea94), state);
            lv_obj_set_style_text_color(button, lv_color_hex(0x15171a), state);
        }
        auto *label = lv_label_create(button);
        lv_label_set_text(label, action == Confirm ? _("OK") : _("Cancel"));
        lv_obj_center(label);
    }
    for (const auto &saved : origin)
        if (saved.device)
            lv_indev_set_group(saved.device, dialogGroup);
    lv_group_set_default(previousGroup);
}

void SettingsPlugin::addLabel(const char *text)
{
    auto *label = lv_label_create(body);
    lv_obj_set_width(label, LV_PCT(100));
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label, text);
}

lv_obj_t *SettingsPlugin::addSwitch(const char *label, bool value)
{
    auto *control = lv_checkbox_create(body);
    lv_checkbox_set_text(control, label);
    lv_obj_set_state(control, LV_STATE_CHECKED, value);
    attachInput(control);
    return control;
}

lv_obj_t *SettingsPlugin::addText(const char *label, const char *value, uint32_t maxLength, bool password, const char *accepted)
{
    addLabel(label);
    auto *control = lv_textarea_create(body);
    lv_obj_set_width(control, LV_PCT(100));
    lv_textarea_set_one_line(control, true);
    lv_textarea_set_max_length(control, maxLength);
    lv_textarea_set_password_mode(control, password);
    if (accepted)
        lv_textarea_set_accepted_chars(control, accepted);
    lv_textarea_set_text(control, value);
    attachInput(control);
    return control;
}

lv_obj_t *SettingsPlugin::addDropdown(const char *label, const char *options, uint32_t selected)
{
    addLabel(label);
    auto *control = lv_dropdown_create(body);
    lv_obj_set_width(control, LV_PCT(100));
    lv_dropdown_set_options(control, options);
    lv_dropdown_set_selected(control, selected);
    attachInput(control);
    return control;
}

bool SettingsPlugin::loaded() const
{
    switch (editor) {
    case Editor::WiFi:
        return config.has_network;
    case Editor::GPS:
        return config.has_position;
    case Editor::Radio:
        return config.has_lora;
    case Editor::MQTT:
        return modules.has_mqtt;
    case Editor::Sound:
        return modules.has_external_notification;
    default:
        return ui.version == 1;
    }
}

void SettingsPlugin::requestPIN(bool screenOnly)
{
    pinRequired = true;
    showSettingsAfterPIN = screenOnly;
    createDialog(screenUnlock ? _("Unlock screen") : _("Unlock settings"));
    if (screenUnlock) {
        auto *footer = lv_obj_get_child(dialog, -1);
        auto *clearButton = lv_obj_get_child(footer, 1);
        lv_label_set_text(lv_obj_get_child(clearButton, 0), _("Clear"));
    }
    fields[0] = addText(_("6-digit PIN"), "", 6, true, "0123456789");
    lv_group_focus_obj(fields[0]);
}

void SettingsPlugin::createEditor()
{
    pinRequired = false;
    fields.fill(nullptr);
    const char *titles[] = {_("Wi-Fi"),          _("GPS"),   _("Radio"),   _("MQTT"),
                            _("Message popups"), _("Sound"), _("Display"), _("Input")};
    createDialog(titles[static_cast<uint8_t>(editor)]);
    if (!loaded()) {
        error(_("Settings have not loaded yet. Close and try again."));
        return;
    }
    char number[16];
    switch (editor) {
    case Editor::WiFi:
        fields[0] = addSwitch(_("Enable Wi-Fi"), config.network.wifi_enabled);
        fields[1] = addText(_("Network name"), config.network.wifi_ssid, 32);
        fields[2] = addText(_("Password"), config.network.wifi_psk, 64, true);
        break;
    case Editor::GPS:
        fields[0] = addSwitch(_("Enable GPS"), config.position.gps_mode == meshtastic_Config_PositionConfig_GpsMode_ENABLED);
        addLabel(_("Fixed position and GPS reporting options are preserved."));
        break;
    case Editor::Radio: {
        fields[0] = addSwitch(_("Enable radio transmission"), config.lora.tx_enabled);
        std::string regions;
        for (unsigned i = 0; i <= 22; ++i) {
            if (i)
                regions += '\n';
            regions += LoRaPresets::loRaRegionToString(static_cast<meshtastic_Config_LoRaConfig_RegionCode>(i));
        }
        if (config.lora.region > 22)
            regions += std::string("\n") + _("Current region");
        fields[1] = addDropdown(_("Region"), regions.c_str(), config.lora.region <= 22 ? config.lora.region : 23);
        std::string presets;
        for (unsigned i = 0; i <= 8; ++i) {
            if (i)
                presets += '\n';
            presets += LoRaPresets::modemPresetToString(static_cast<meshtastic_Config_LoRaConfig_ModemPreset>(i));
        }
        const bool knownPreset = config.lora.use_preset && config.lora.modem_preset <= 8;
        if (!knownPreset)
            presets += std::string("\n") + _("Current custom settings");
        fields[2] = addDropdown(_("Preset"), presets.c_str(), knownPreset ? config.lora.modem_preset : 9);
        snprintf(number, sizeof(number), "%u", config.lora.hop_limit);
        fields[3] = addText(_("Hop limit (0-7)"), number, 1, false, "01234567");
        break;
    }
    case Editor::MQTT:
        fields[0] = addSwitch(_("Enable MQTT"), modules.mqtt.enabled);
        addLabel(_("Server, credentials and encryption settings are preserved."));
        break;
    case Editor::Popups: {
        fields[0] = addSwitch(_("Show message popups"), ui.alert_enabled);
        auto *sound = lv_button_create(body);
        lv_obj_set_user_data(sound, reinterpret_cast<void *>(SoundSettings));
        attachInput(sound);
        auto *label = lv_label_create(sound);
        lv_label_set_text(label, _("Sound settings"));
        lv_obj_center(label);
        break;
    }
    case Editor::Sound:
        fields[0] = addSwitch(_("Play a sound for messages"),
                              modules.external_notification.enabled && modules.external_notification.alert_message_buzzer);
        addLabel(_("The current ringtone and notification outputs are preserved."));
        break;
    case Editor::Display:
        snprintf(number, sizeof(number), "%u", ui.screen_brightness);
        fields[0] = addText(_("Brightness (1-255)"), number, 3, false, "0123456789");
        snprintf(number, sizeof(number), "%u", ui.screen_timeout);
        fields[1] = addText(_("Screen timeout in seconds (0 = always on)"), number, 5, false, "0123456789");
        fields[2] = addDropdown(_("Theme"), "Dark\nLight\nRed", ui.theme);
        break;
    case Editor::Input:
        fields[0] = addSwitch(_("Double-space period"), doubleSpacePeriod);
        addLabel(_("In messages, press Space twice to insert a period."));
        break;
    }
    if (fields[0])
        lv_group_focus_obj(fields[0]);
}

void SettingsPlugin::error(const char *text)
{
    lv_label_set_text(errorLabel, text);
    lv_obj_remove_flag(errorLabel, LV_OBJ_FLAG_HIDDEN);
}

void SettingsPlugin::clearDialog(bool restore)
{
    if (!dialog)
        return;
    for (const auto &saved : origin) {
        if (saved.device)
            lv_indev_set_group(saved.device, saved.group);
        if (restore && saved.focus && lv_obj_is_valid(saved.focus))
            lv_group_focus_obj(saved.focus);
    }
    auto *old = dialog;
    dialog = nullptr;
    keyboard = nullptr;
    body = nullptr;
    errorLabel = nullptr;
    fields.fill(nullptr);
    lv_obj_delete(old);
    lv_group_delete(dialogGroup);
    dialogGroup = nullptr;
}

void SettingsPlugin::confirm()
{
    if (pinRequired) {
        const char *entered = lv_textarea_get_text(fields[0]);
        char expected[7];
        snprintf(expected, sizeof(expected), "%06u", ui.pin_code);
        if (ui.pin_code > 999999 || strlen(entered) != 6 || strcmp(entered, expected) != 0) {
            lv_textarea_set_text(fields[0], "");
            error(_("Incorrect PIN"));
            lv_group_focus_obj(fields[0]);
            return;
        }
        const bool screenOnly = showSettingsAfterPIN;
        clearDialog(true);
        pinRequired = false;
        if (screenUnlock) {
            screenUnlock = false;
            auto callback = std::move(unlocked);
            if (callback)
                callback();
            return;
        }
        if (screenOnly) {
            GfxPlugin::loadScreen(LV_SCR_LOAD_ANIM_NONE, 0);
            if (input.getKeyboard())
                lv_indev_set_group(input.getKeyboard(), group);
        } else {
            createEditor();
        }
        return;
    }
    if (!loaded() || !fields[0]) {
        error(_("Settings have not loaded yet. Close and try again."));
        return;
    }
    if (save()) {
        clearDialog(true);
        if (changed)
            changed();
    }
}

bool SettingsPlugin::savePeriod(bool enabled)
{
    ISpiLock::Guard bus;
    if (!fileSystem.exists("/prefs") && !fileSystem.mkdir("/prefs"))
        return false;
    const char *temporary = "/prefs/double-space-period.tmp";
    File file = fileSystem.open(temporary, FILE_WRITE);
    if (!file)
        return false;
    const uint8_t value = enabled ? '1' : '0';
    const bool written = file.write(&value, 1) == 1;
    file.close();
    if (!written || !fileSystem.rename(temporary, "/prefs/double-space-period")) {
        fileSystem.remove(temporary);
        return false;
    }
    doubleSpacePeriod = enabled;
    if (applyPeriod)
        applyPeriod(enabled);
    return true;
}

bool SettingsPlugin::save()
{
    bool accepted = false;
    // Copy the current configuration at confirmation time, changing only the
    // fields exposed by this editor. Other settings and credentials survive.
    switch (editor) {
    case Editor::WiFi: {
        auto next = config.network;
        const char *ssid = lv_textarea_get_text(fields[1]);
        const char *password = lv_textarea_get_text(fields[2]);
        if (strlen(ssid) >= sizeof(next.wifi_ssid) || strlen(password) >= sizeof(next.wifi_psk) ||
            (checked(fields[0]) && !*ssid)) {
            error(_("Enter a network name and a password that fit the device limits."));
            return false;
        }
        if (checked(fields[0]) == next.wifi_enabled && strcmp(ssid, next.wifi_ssid) == 0 && strcmp(password, next.wifi_psk) == 0)
            return true;
        next.wifi_enabled = checked(fields[0]);
        snprintf(next.wifi_ssid, sizeof(next.wifi_ssid), "%s", ssid);
        snprintf(next.wifi_psk, sizeof(next.wifi_psk), "%s", password);
        accepted = controller.sendConfig(meshtastic_Config_NetworkConfig{next});
        if (accepted)
            config.network = next;
        break;
    }
    case Editor::GPS: {
        auto next = config.position;
        if (checked(fields[0]) == (next.gps_mode == meshtastic_Config_PositionConfig_GpsMode_ENABLED))
            return true;
        next.gps_mode = checked(fields[0]) ? meshtastic_Config_PositionConfig_GpsMode_ENABLED
                                           : meshtastic_Config_PositionConfig_GpsMode_DISABLED;
        accepted = controller.sendConfig(meshtastic_Config_PositionConfig{next});
        if (accepted)
            config.position = next;
        break;
    }
    case Editor::Radio: {
        auto next = config.lora;
        next.tx_enabled = checked(fields[0]);
        const auto region = lv_dropdown_get_selected(fields[1]);
        const auto preset = lv_dropdown_get_selected(fields[2]);
        if (region <= 22)
            next.region = static_cast<meshtastic_Config_LoRaConfig_RegionCode>(region);
        if (preset <= 8) {
            next.use_preset = true;
            next.modem_preset = static_cast<meshtastic_Config_LoRaConfig_ModemPreset>(preset);
        }
        const char *hops = lv_textarea_get_text(fields[3]);
        if (!*hops) {
            error(_("Enter a hop limit from 0 to 7."));
            return false;
        }
        next.hop_limit = strtoul(hops, nullptr, 10);
        if (next.tx_enabled == config.lora.tx_enabled && next.region == config.lora.region &&
            next.use_preset == config.lora.use_preset && next.modem_preset == config.lora.modem_preset &&
            next.hop_limit == config.lora.hop_limit)
            return true;
        accepted = controller.sendConfig(meshtastic_Config_LoRaConfig{next});
        if (accepted)
            config.lora = next;
        break;
    }
    case Editor::MQTT: {
        auto next = modules.mqtt;
        if (next.enabled == checked(fields[0]))
            return true;
        next.enabled = checked(fields[0]);
        accepted = controller.sendConfig(meshtastic_ModuleConfig_MQTTConfig{next});
        if (accepted)
            modules.mqtt = next;
        break;
    }
    case Editor::Sound: {
        auto next = modules.external_notification;
        const bool enable = checked(fields[0]);
        if (enable == (next.enabled && next.alert_message_buzzer))
            return true;
        next.alert_message_buzzer = enable;
        if (enable) {
            next.enabled = true;
#ifdef USE_I2S_BUZZER
            next.use_i2s_as_buzzer = true;
            next.use_pwm = false;
#else
            next.use_pwm = true;
#endif
        }
        accepted = controller.sendConfig(meshtastic_ModuleConfig_ExternalNotificationConfig{next});
        if (accepted)
            modules.external_notification = next;
        break;
    }
    case Editor::Popups:
    case Editor::Display: {
        auto next = ui;
        if (editor == Editor::Popups) {
            next.alert_enabled = checked(fields[0]);
        } else {
            const char *brightness = lv_textarea_get_text(fields[0]);
            const char *timeout = lv_textarea_get_text(fields[1]);
            const unsigned long b = strtoul(brightness, nullptr, 10);
            const unsigned long t = strtoul(timeout, nullptr, 10);
            if (!*brightness || b < 1 || b > 255 || !*timeout || t > 65535) {
                error(_("Brightness must be 1-255; timeout must be 0-65535 seconds."));
                return false;
            }
            next.screen_brightness = b;
            next.screen_timeout = t;
            next.theme = static_cast<meshtastic_Theme>(lv_dropdown_get_selected(fields[2]));
        }
        if (next.alert_enabled == ui.alert_enabled && next.screen_brightness == ui.screen_brightness &&
            next.screen_timeout == ui.screen_timeout && next.theme == ui.theme)
            return true;
        accepted = controller.storeUIConfig(next);
        if (accepted) {
            if (applyUI)
                applyUI(next);
            ui = next;
        }
        break;
    }
    case Editor::Input:
        if (doubleSpacePeriod == checked(fields[0]))
            return true;
        accepted = savePeriod(checked(fields[0]));
        break;
    }
    if (!accepted)
        error(_("Could not save settings. Your changes are still open."));
    return accepted;
}

void SettingsPlugin::event(lv_event_t *e)
{
    auto *self = static_cast<SettingsPlugin *>(lv_event_get_user_data(e));
    auto *target = lv_event_get_target_obj(e);
    const auto code = lv_event_get_code(e);
    const auto action = reinterpret_cast<uintptr_t>(lv_obj_get_user_data(target));
    if (code == LV_EVENT_KEY && lv_event_get_key(e) == LV_KEY_ESC && self->dialog) {
        self->cancel();
        return;
    }
    if (code == LV_EVENT_READY && self->pinRequired && target == self->fields[0]) {
        self->confirm();
        return;
    }
    if (code == LV_EVENT_FOCUSED && self->dialog && lv_obj_check_type(target, &lv_textarea_class) &&
        !self->input.hasKeyboardDevice()) {
        if (!self->keyboard) {
            self->keyboard = lv_keyboard_create(self->dialog);
            lv_obj_set_size(self->keyboard, LV_PCT(100), 90);
            self->attachInput(self->keyboard);
        }
        lv_obj_remove_flag(self->keyboard, LV_OBJ_FLAG_HIDDEN);
        lv_keyboard_set_textarea(self->keyboard, target);
    }
    if ((code == LV_EVENT_READY || code == LV_EVENT_CANCEL) && target == self->keyboard) {
        lv_obj_add_flag(self->keyboard, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    if (code != LV_EVENT_SHORT_CLICKED)
        return;
    if (action == Confirm)
        self->confirm();
    else if (action == Cancel)
        self->cancel();
    else if (action == SoundSettings) {
        if (checked(self->fields[0]) != self->ui.alert_enabled) {
            self->error(_("Save or cancel your popup change before opening sound settings."));
            return;
        }
        self->clearDialog(true);
        self->editor = Editor::Sound;
        self->createEditor();
    } else if (action >= FirstEditor && action < FirstEditor + 8)
        self->open(static_cast<Editor>(action - FirstEditor));
}

#endif
