#pragma once

#include "GfxPlugin.h"
#include "meshtastic/device_ui.pb.h"
#include "meshtastic/localonly.pb.h"
#include <array>
#include <functional>

class ViewController;
class InputDriver;

// One set of staged editors serves both the Settings screen and dashboard rows.
class SettingsPlugin : public GfxPlugin
{
  public:
    enum class Editor : uint8_t { WiFi, GPS, Radio, MQTT, Popups, Sound, Display, Input };
    SettingsPlugin(ViewController &controller, InputDriver &input, meshtastic_DeviceUIConfig &ui, meshtastic_LocalConfig &config,
                   meshtastic_LocalModuleConfig &modules);
    ~SettingsPlugin() override;
    void configure(lv_obj_t *screen, lv_obj_t *panel, lv_group_t *settingsGroup, lv_indev_t *primaryInput);
    void loadScreen(lv_screen_load_anim_t anim = LV_SCR_LOAD_ANIM_NONE, uint32_t time = 0) override;
    void open(Editor editor);
    void unlockScreen(const std::function<void()> &onSuccess);
    bool isLocked() const { return screenUnlock; }
    void setOnApplyUI(const std::function<void(const meshtastic_DeviceUIConfig &)> &cb) { applyUI = cb; }
    void setOnChanged(const std::function<void()> &cb) { changed = cb; }
    void setOnDoubleSpacePeriod(const std::function<void(bool)> &cb) { applyPeriod = cb; }
    bool doubleSpacePeriodEnabled() const { return doubleSpacePeriod; }

  private:
    void captureOrigin();
    void createDialog(const char *title);
    void createEditor();
    void requestPIN(bool screenOnly);
    void clearDialog(bool restore);
    void confirm();
    void cancel();
    void error(const char *text);
    bool loaded() const;
    bool save();
    bool savePeriod(bool enabled);
    void attachInput(lv_obj_t *obj);
    lv_obj_t *addSwitch(const char *label, bool value);
    lv_obj_t *addText(const char *label, const char *value, uint32_t maxLength, bool password = false,
                      const char *accepted = nullptr);
    lv_obj_t *addDropdown(const char *label, const char *options, uint32_t selected);
    void addLabel(const char *text);
    static void event(lv_event_t *e);

    ViewController &controller;
    InputDriver &input;
    meshtastic_DeviceUIConfig &ui;
    meshtastic_LocalConfig &config;
    meshtastic_LocalModuleConfig &modules;
    std::function<void(const meshtastic_DeviceUIConfig &)> applyUI;
    std::function<void()> changed;
    std::function<void(bool)> applyPeriod;
    std::array<lv_obj_t *, 4> fields{};
    lv_obj_t *dialog = nullptr;
    lv_obj_t *body = nullptr;
    lv_obj_t *errorLabel = nullptr;
    lv_obj_t *keyboard = nullptr;
    lv_group_t *dialogGroup = nullptr;
    struct OriginInput {
        lv_indev_t *device = nullptr;
        lv_group_t *group = nullptr;
        lv_obj_t *focus = nullptr;
    };
    std::array<OriginInput, 4> origin{};
    Editor editor = Editor::WiFi;
    std::function<void()> unlocked;
    bool screenUnlock = false;
    bool pinRequired = false;
    bool showSettingsAfterPIN = false;
    bool doubleSpacePeriod = false;
};
