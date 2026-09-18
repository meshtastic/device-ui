#include "graphics/plugin/Plugins.h"
#ifdef MUI_MAP_PLUGIN

#include "graphics/plugin/MapPlugin.h"
#include "graphics/map/AsyncTileService.h"
#include "graphics/map/MapTileSettings.h"
#include "graphics/map/PMTileService.h"
#include "graphics/map/TileProvider.h"
#include "images.h"
#include "screens.h"
#include <algorithm>
#include <cstdio>
#include <cstring>

#if defined(ARCH_PORTDUINO)
#include "graphics/map/CURLService.h"
#elif defined(ARDUINO_ARCH_ESP32)
#include "graphics/map/URLService.h"
#endif
#if defined(SENSECAP_INDICATOR)
#include "graphics/map/RemoteSDService.h"
#elif defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC) || defined(SDCARD_SHARE_SPI)
#include "graphics/map/SDCardService.h"
#elif defined(HAS_SDCARD)
#include "graphics/map/SdFatService.h"
#endif

namespace
{
constexpr const char *osmUrl = "https://tile.openstreetmap.org/{z}/{x}/{y}.png";

void focusStyle(lv_obj_t *obj)
{
    for (lv_style_selector_t state : {LV_STATE_FOCUSED, LV_STATE_FOCUS_KEY, LV_STATE_EDITED}) {
        lv_obj_set_style_outline_color(obj, lv_palette_main(LV_PALETTE_BLUE), state);
        lv_obj_set_style_outline_width(obj, 3, state);
        lv_obj_set_style_outline_pad(obj, -3, state);
        lv_obj_set_style_outline_opa(obj, LV_OPA_COVER, state);
    }
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
}

bool validPosition(int32_t lat, int32_t lon)
{
    return lat >= -850511287 && lat <= 850511287 && lon >= -1800000000 && lon <= 1800000000 && (lat || lon);
}
} // namespace

MapPlugin::MapPlugin() : GfxPlugin("map") {}

MapPlugin::~MapPlugin()
{
    // The tile worker must stop before its SD save target and LVGL images die.
    map.reset();
}

void MapPlugin::init(lv_obj_t *p, WidgetResolver resolver, std::size_t count, lv_group_t *g, lv_indev_t *input,
                     RegisterWidget registration)
{
    GfxPlugin::init(p, std::move(resolver), count, g, input, registration);
    canvas = getWidget(static_cast<WidgetIndex>(Widget::Canvas));
    if (parent && canvas) {
        createControls();
        lv_obj_add_event_cb(parent, event, LV_EVENT_SCREEN_LOADED, this);
        lv_obj_add_event_cb(parent, event, LV_EVENT_SCREEN_UNLOAD_START, this);
        lv_obj_add_event_cb(canvas, event, LV_EVENT_GESTURE, this);
    }
}

void MapPlugin::registerStandardWidgets()
{
    setWidget(static_cast<WidgetIndex>(Widget::Canvas), objects.map_panel);
    setWidget(static_cast<WidgetIndex>(Widget::Title), objects.top_map_label);
    setWidget(static_cast<WidgetIndex>(Widget::Back), objects.top_map_back_button);
}

lv_obj_t *MapPlugin::button(lv_obj_t *container, const char *text)
{
    auto *obj = lv_button_create(container);
    lv_obj_set_size(obj, 32, 30);
    lv_obj_set_style_pad_all(obj, 3, 0);
    lv_obj_set_style_shadow_width(obj, 0, 0);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, 0);
    focusStyle(obj);
    auto *label = lv_label_create(obj);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    lv_obj_add_event_cb(obj, event, LV_EVENT_ALL, this);
    if (group)
        lv_group_add_obj(group, obj);
    return obj;
}

void MapPlugin::createControls()
{
    lv_obj_update_layout(parent);
    const int32_t top = lv_obj_get_y(canvas);
    const int32_t height = std::max<int32_t>(36, lv_obj_get_height(parent) - top - 42);
    lv_obj_set_size(canvas, LV_PCT(100), height);
    lv_obj_remove_flag(canvas, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    toolbar = lv_obj_create(parent);
    lv_obj_set_size(toolbar, LV_PCT(100), 42);
    lv_obj_align(toolbar, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_radius(toolbar, 0, 0);
    lv_obj_set_style_border_width(toolbar, 0, 0);
    lv_obj_set_style_pad_all(toolbar, 5, 0);
    lv_obj_set_style_pad_column(toolbar, 5, 0);
    lv_obj_set_flex_flow(toolbar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(toolbar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scroll_dir(toolbar, LV_DIR_HOR);
    lv_obj_set_scrollbar_mode(toolbar, LV_SCROLLBAR_MODE_OFF);

    zoomLabel = lv_label_create(toolbar);
    lv_obj_set_width(zoomLabel, 27);
    lv_obj_set_style_text_font(zoomLabel, &lv_font_montserrat_12, 0);
    zoom = lv_slider_create(toolbar);
    lv_obj_set_size(zoom, 70, 10);
    lv_obj_set_flex_grow(zoom, 1);
    lv_obj_set_style_min_width(zoom, 50, 0);
    focusStyle(zoom);
    lv_slider_set_range(zoom, 2, TileProvider::maxZoom());
    lv_obj_add_event_cb(zoom, event, LV_EVENT_ALL, this);
    if (group)
        lv_group_add_obj(group, zoom);
    panLeft = button(toolbar, LV_SYMBOL_LEFT);
    panRight = button(toolbar, LV_SYMBOL_RIGHT);
    panUp = button(toolbar, LV_SYMBOL_UP);
    panDown = button(toolbar, LV_SYMBOL_DOWN);
    home = button(toolbar, LV_SYMBOL_HOME);
    follow = button(toolbar, "GPS");
    lv_obj_set_width(follow, 38);
    lv_obj_add_flag(follow, LV_OBJ_FLAG_CHECKABLE);
    sourceButton = button(toolbar, "Source");
    lv_obj_set_width(sourceButton, 55);
    updateZoom();

    attribution = lv_label_create(parent);
    lv_obj_set_style_text_font(attribution, &lv_font_montserrat_10, 0);
    lv_obj_set_style_bg_color(attribution, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(attribution, LV_OPA_80, 0);
    lv_obj_set_style_text_color(attribution, lv_color_black(), 0);
    lv_obj_align(attribution, LV_ALIGN_BOTTOM_RIGHT, -2, -44);
    lv_obj_add_flag(attribution, LV_OBJ_FLAG_HIDDEN);

    notice = button(parent, "");
    noticeLabel = lv_obj_get_child(notice, 0);
    lv_obj_set_size(notice, LV_PCT(85), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(notice, 8, 0);
    lv_obj_set_style_bg_color(notice, lv_color_hex(0x20252b), 0);
    lv_obj_set_style_bg_opa(notice, LV_OPA_90, 0);
    lv_obj_set_style_text_color(notice, lv_color_white(), 0);
    lv_obj_set_width(noticeLabel, LV_PCT(100));
    lv_label_set_long_mode(noticeLabel, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(noticeLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(notice, LV_ALIGN_CENTER, 0, -15);
    lv_obj_add_flag(notice, LV_OBJ_FLAG_HIDDEN);

    sourcePanel = lv_obj_create(parent);
    lv_obj_set_size(sourcePanel, LV_PCT(96), LV_PCT(94));
    lv_obj_center(sourcePanel);
    lv_obj_set_style_pad_all(sourcePanel, 8, 0);
    lv_obj_set_style_pad_row(sourcePanel, 6, 0);
    lv_obj_set_flex_flow(sourcePanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_dir(sourcePanel, LV_DIR_VER);
    auto *heading = lv_label_create(sourcePanel);
    lv_label_set_text(heading, "Map source");
    sourceDropdown = lv_dropdown_create(sourcePanel);
    lv_obj_set_width(sourceDropdown, LV_PCT(100));
    focusStyle(sourceDropdown);
    lv_obj_add_event_cb(sourceDropdown, event, LV_EVENT_ALL, this);
    urlInput = lv_textarea_create(sourcePanel);
    lv_obj_set_width(urlInput, LV_PCT(100));
    lv_textarea_set_one_line(urlInput, true);
    lv_textarea_set_max_length(urlInput, 300);
    lv_textarea_set_placeholder_text(urlInput, "https://.../{z}/{x}/{y}.png");
    lv_obj_set_style_text_font(urlInput, &lv_font_montserrat_12, 0);
    focusStyle(urlInput);
    lv_obj_add_event_cb(urlInput, event, LV_EVENT_ALL, this);
    sourceMessage = lv_label_create(sourcePanel);
    lv_obj_set_width(sourceMessage, LV_PCT(100));
    lv_label_set_long_mode(sourceMessage, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(sourceMessage, &lv_font_montserrat_12, 0);
    auto *actions = lv_obj_create(sourcePanel);
    lv_obj_set_size(actions, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(actions, 0, 0);
    lv_obj_set_style_border_width(actions, 0, 0);
    lv_obj_set_flex_flow(actions, LV_FLEX_FLOW_ROW);
    lv_obj_remove_flag(actions, LV_OBJ_FLAG_SCROLLABLE);
    applyButton = button(actions, "Use URL");
    closeButton = button(actions, "Back to map");
    lv_obj_set_width(applyButton, 100);
    lv_obj_set_width(closeButton, 120);
    if (group) {
        lv_group_add_obj(group, sourceDropdown);
        lv_group_add_obj(group, urlInput);
        // Keep the form's traversal order the same as its visual order.
        lv_group_remove_obj(applyButton);
        lv_group_remove_obj(closeButton);
        lv_group_add_obj(group, applyButton);
        lv_group_add_obj(group, closeButton);
    }
    lv_obj_add_flag(sourcePanel, LV_OBJ_FLAG_HIDDEN);
    auto *back = getWidget(static_cast<WidgetIndex>(Widget::Back));
    if (back) {
        lv_obj_add_event_cb(back, event, LV_EVENT_KEY, this);
        lv_obj_add_event_cb(back, event, LV_EVENT_FOCUSED, this);
        lv_obj_add_event_cb(back, event, LV_EVENT_SHORT_CLICKED, this);
        if (group)
            lv_group_add_obj(group, back);
    }
}

void MapPlugin::setConfig(meshtastic_DeviceUIConfig *c, std::function<void()> save)
{
    config = c;
    saveConfig = std::move(save);
    storageDirty = true;
}

void MapPlugin::setOwnNode(uint32_t id)
{
    ownNode = id;
    MapTileSettings::setUniqueId(id);
}

void MapPlugin::updateStorage(ISdCard *sd, bool present)
{
    if (sd != card || present != mounted || (sd && !sd->isUpdated()))
        storageDirty = true;
    card = sd;
    mounted = present;
    if (!mounted)
        MapTileSettings::setSaveOK(false);
}

void MapPlugin::updateNetwork(bool enabled, bool connected, bool known)
{
    const bool recovered = known && connected && (!networkKnown || !wifiConnected);
    wifiEnabled = enabled;
    wifiConnected = connected;
    networkKnown = known;
    if (recovered && map)
        map->forceRedraw();
}

void MapPlugin::createMap()
{
    if (map || !canvas)
        return;
    ITileService *tiles = nullptr;
    IMapFileSystem *archive = nullptr;
#if defined(SENSECAP_INDICATOR)
    tiles = new RemoteSDService();
    archive = new RemoteMapFileSystem();
#elif defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC) || defined(SDCARD_SHARE_SPI)
    tiles = new SDCardService();
    archive = new SDMapFileSystem();
#elif defined(HAS_SDCARD)
    tiles = new SdFatService();
    archive = new SdFatMapFileSystem();
#endif
#if defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC) || defined(HAS_SDCARD) || defined(SDCARD_SHARE_SPI) || defined(SENSECAP_INDICATOR)
    map.reset(new MapPanel(canvas, new PMTileService(tiles, archive)));
#else
    map.reset(new MapPanel(canvas, tiles));
#endif
    auto save = [tiles](const char *name, void *data, size_t size) { return tiles && tiles->save(name, data, size); };
#if defined(ARDUINO_ARCH_ESP32)
    map->setBackupService(new AsyncTileService(new URLService(save)));
#elif defined(ARCH_PORTDUINO)
    map->setBackupService(new AsyncTileService(new CURLService(save)));
#endif
    map->setNoTileImage(&img_no_tile_image);
    homeMarker = lv_label_create(canvas);
    lv_label_set_text(homeMarker, LV_SYMBOL_HOME);
    lv_obj_set_style_text_color(homeMarker, lv_color_hex(0x00aa55), 0);
    lv_obj_add_flag(homeMarker, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(homeMarker, event, LV_EVENT_CLICKED, this);
    map->setHomeLocationImage(homeMarker);
    gpsMarker = lv_label_create(canvas);
    lv_label_set_text(gpsMarker, LV_SYMBOL_GPS);
    lv_obj_set_style_text_color(gpsMarker, lv_color_hex(0x006cff), 0);
    lv_obj_add_flag(gpsMarker, LV_OBJ_FLAG_HIDDEN);
    if (hasGps)
        map->setGpsPositionImage(gpsMarker);
    if (config && config->map_data.has_home) {
        map->setHomeLocation(config->map_data.home.latitude * 1e-7f, config->map_data.home.longitude * 1e-7f);
        MapTileSettings::setDefaultZoom(config->map_data.home.zoom);
        map->setZoom(config->map_data.home.zoom);
    } else if (hasGps) {
        map->setHomeLocation(latitude * 1e-7f, longitude * 1e-7f);
        map->setZoom(13);
    } else {
        map->setZoom(3);
    }
    if (hasGps)
        map->setGpsPosition(latitude * 1e-7f, longitude * 1e-7f);
    for (auto &entry : nodes)
        addNode(entry.first, entry.second);
    setFollowGps(config && config->map_data.follow_gps, false);
    updateZoom();
}

void MapPlugin::updateNode(uint32_t id, int32_t lat, int32_t lon, const char *name)
{
    if (!validPosition(lat, lon)) {
        removeNode(id);
        return;
    }
    auto &node = nodes[id];
    const bool created = !node.marker;
    node.latitude = lat;
    node.longitude = lon;
    node.name = name ? name : "";
    if (!map)
        return;
    if (created)
        addNode(id, node);
    else {
        lv_label_set_text(lv_obj_get_child(node.marker, 0), node.name.c_str());
        map->update(id, lat * 1e-7f, lon * 1e-7f);
    }
}

void MapPlugin::addNode(uint32_t id, Node &node)
{
    node.marker = lv_button_create(canvas);
    lv_group_remove_obj(node.marker);
    lv_obj_set_size(node.marker, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(node.marker, 3, 0);
    lv_obj_set_style_shadow_width(node.marker, 0, 0);
    lv_obj_set_style_text_font(node.marker, &lv_font_montserrat_12, 0);
    lv_obj_add_flag(node.marker, LV_OBJ_FLAG_HIDDEN);
    auto *label = lv_label_create(node.marker);
    lv_label_set_text(label, node.name.c_str());
    lv_obj_add_event_cb(node.marker, event, LV_EVENT_CLICKED, this);
    map->add(id, node.latitude * 1e-7f, node.longitude * 1e-7f, [this](uint32_t nodeId, uint16_t x, uint16_t y, uint8_t z) {
        auto it = nodes.find(nodeId);
        if (it == nodes.end() || !it->second.marker)
            return;
        auto *marker = it->second.marker;
        if (!z) {
            lv_obj_add_flag(marker, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_remove_flag(marker, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_pos(marker, int32_t(x) - lv_obj_get_width(marker) / 2, int32_t(y) - lv_obj_get_height(marker));
            lv_obj_move_foreground(marker);
        }
    });
}

void MapPlugin::removeNode(uint32_t id)
{
    auto it = nodes.find(id);
    if (it == nodes.end())
        return;
    if (map)
        map->remove(id);
    if (it->second.marker)
        lv_obj_delete(it->second.marker);
    nodes.erase(it);
}

void MapPlugin::setGpsPosition(int32_t lat, int32_t lon)
{
    if (!validPosition(lat, lon))
        return;
    const bool hadGps = hasGps;
    if (hadGps && latitude == lat && longitude == lon)
        return;
    latitude = lat;
    longitude = lon;
    hasGps = true;
    if (map) {
        map->setGpsPositionImage(gpsMarker);
        map->setGpsPosition(lat * 1e-7f, lon * 1e-7f);
        if (following && !hadGps)
            map->setLocked(true);
    }
}

void MapPlugin::discoverSources()
{
    storageDirty = false;
    sources.clear();
    hasSdSource = false;
    MapTileSettings::setSaveOK(false);
    MapTileSettings::setPMTiles(false);
    if (mounted && card) {
        MapTileSettings::setPrefix("/maps");
        auto found = card->loadMapStyles("/maps");
        if (found.empty() && TileProvider::selectedTemplate() < 0 && card->setUrlProvider("/maps", "OpenStreetMap", osmUrl))
            found.insert("OpenStreetMap");
        sources.assign(found.begin(), found.end());
        hasSdSource = !sources.empty();
        if (hasSdSource) {
            char saved[MapTileSettings::TILE_STYLE_SIZE] = {};
            if (config)
                MapTileSettings::styleToDir(config->map_data.style, saved, sizeof(saved));
            auto selected = std::find(sources.begin(), sources.end(), saved);
            selectSource(selected == sources.end() ? 0 : std::distance(sources.begin(), selected), false);
        }
    }
    updateSourceControls();
    if (map && mounted)
        map->forceRedraw();
}

void MapPlugin::selectSource(std::size_t index, bool persist)
{
    if (index >= sources.size() || !mounted || !card)
        return;
    const auto &name = sources[index];
    const bool legacy = name == "/map";
    MapTileSettings::setPrefix(legacy ? "/map" : "/maps");
    MapTileSettings::setTileStyle(legacy ? "" : name.c_str());
    MapTileSettings::setPMTiles(!legacy && card->hasMapArchive("/maps", name.c_str()));
    auto url = legacy ? std::string() : card->getUrlProvider("/maps", name.c_str());
    MapTileSettings::setTileProvider(-1);
    if (!url.empty())
        TileProvider::selectTemplate(TileProvider::addTemplate("URL: " + name, url));
    // Selecting a URL backed by this SD directory must keep caching enabled.
    MapTileSettings::setSaveOK(!url.empty());
    if (config && persist) {
        config->has_map_data = true;
        snprintf(config->map_data.style, sizeof(config->map_data.style), "%s", legacy ? "" : name.c_str());
        if (saveConfig)
            saveConfig();
    }
    if (map) {
        map->setZoom(MapTileSettings::getZoomLevel());
        map->forceRedraw();
    }
    updateZoom();
}

void MapPlugin::updateSourceControls()
{
    if (!sourcePanel)
        return;
    std::string options;
    uint32_t selected = 0;
    char currentStyle[MapTileSettings::TILE_STYLE_SIZE];
    MapTileSettings::styleToDir(MapTileSettings::getTileDir(), currentStyle, sizeof(currentStyle));
    for (std::size_t i = 0; i < sources.size(); ++i) {
        if (!options.empty())
            options += '\n';
        options += sources[i];
        if (sources[i] == currentStyle)
            selected = i;
    }
    lv_dropdown_set_options(sourceDropdown, options.empty() ? "No SD map sources" : options.c_str());
    lv_dropdown_set_selected(sourceDropdown, selected);
    lv_obj_set_state(sourceDropdown, LV_STATE_DISABLED, options.empty());
    lv_textarea_set_text(urlInput, TileProvider::url().c_str());
    lv_label_set_text(sourceMessage, mounted ? "SD maps and saved online sources. Viewed online tiles are cached on the card."
                                           : "Insert an SD card for offline maps and tile caching, or enter an online tile URL.");
    const auto url = TileProvider::url();
    const bool osm = url.find("openstreetmap.org") != std::string::npos;
    lv_label_set_text(attribution, osm ? "© OpenStreetMap contributors" : "");
    lv_obj_set_flag(attribution, LV_OBJ_FLAG_HIDDEN, !osm);
}

void MapPlugin::applyUrl()
{
    std::string url = lv_textarea_get_text(urlInput);
    const auto begin = url.find_first_not_of(" \r\n\t");
    const auto end = url.find_last_not_of(" \r\n\t");
    url = begin == std::string::npos ? "" : url.substr(begin, end - begin + 1);
    if ((url.rfind("https://", 0) != 0 && url.rfind("http://", 0) != 0) || url.find("{z}") == std::string::npos ||
        url.find("{x}") == std::string::npos || url.find("{y}") == std::string::npos) {
        lv_label_set_text(sourceMessage, "Use an http(s) tile URL containing {z}, {x} and {y}.");
        return;
    }
    // The standard OSM service requires persistent caching. Its default source
    // is created on the SD card, and an explicit URL follows the same rule.
    if (url == osmUrl && !mounted) {
        lv_label_set_text(sourceMessage, "Insert an SD card to cache OpenStreetMap tiles, then use this URL.");
        return;
    }
    if (mounted && card) {
        std::string style;
        for (const auto &source : sources) {
            if (source != "/map" && card->getUrlProvider("/maps", source.c_str()) == url) {
                style = source;
                break;
            }
        }
        if (style.empty()) {
            // Give each URL a stable cache directory; never reuse another
            // provider's z/x/y files when the user changes the template.
            uint32_t hash = 2166136261u;
            for (unsigned char c : url)
                hash = (hash ^ c) * 16777619u;
            char name[24];
            snprintf(name, sizeof(name), "Online-%08x", static_cast<unsigned>(hash));
            style = url == osmUrl ? "OpenStreetMap" : name;
            if (!card->setUrlProvider("/maps", style.c_str(), url.c_str())) {
                lv_label_set_text(sourceMessage, "Could not save the map source. Check the SD card and try again.");
                return;
            }
            sources.push_back(style);
        }
        hasSdSource = true;
        selectSource(std::distance(sources.begin(), std::find(sources.begin(), sources.end(), style)), true);
    } else {
        TileProvider::selectTemplate(TileProvider::addTemplate("Online", url));
        MapTileSettings::setSaveOK(false);
        MapTileSettings::setPMTiles(false);
        if (map) {
            map->setZoom(MapTileSettings::getZoomLevel());
            map->forceRedraw();
        }
    }
    updateSourceControls();
    closeSources();
}

void MapPlugin::showSources()
{
    if (!sourcePanel)
        return;
    updateSourceControls();
    sourcesOpen = true;
    auto *back = getWidget(static_cast<WidgetIndex>(Widget::Back));
    if (back)
        lv_obj_add_state(back, LV_STATE_DISABLED);
    lv_obj_add_flag(toolbar, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(notice, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(sourcePanel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(sourcePanel);
    if (group) {
        lv_group_set_editing(group, false);
        lv_group_focus_obj(sources.empty() ? urlInput : sourceDropdown);
    }
}

void MapPlugin::closeSources()
{
    if (!sourcePanel)
        return;
    sourcesOpen = false;
    auto *back = getWidget(static_cast<WidgetIndex>(Widget::Back));
    if (back)
        lv_obj_remove_state(back, LV_STATE_DISABLED);
    lv_dropdown_close(sourceDropdown);
    lv_obj_add_flag(sourcePanel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(toolbar, LV_OBJ_FLAG_HIDDEN);
    deferZoomFocus = visible;
    updateStatus();
}

void MapPlugin::updateZoom()
{
    if (!zoom)
        return;
    lv_slider_set_range(zoom, 2, TileProvider::maxZoom());
    lv_slider_set_value(zoom, MapTileSettings::getZoomLevel(), LV_ANIM_OFF);
    lv_label_set_text_fmt(zoomLabel, "z%d", MapTileSettings::getZoomLevel());
}

void MapPlugin::focusZoom()
{
    if (!visible || sourcesOpen || !zoom || !group)
        return;
    // A settings or PIN overlay can take input during the screen animation.
    // Deferred map work must not reclaim its focus while that overlay is open.
    if (indev && lv_indev_get_group(indev) != group)
        return;
    lv_group_focus_obj(zoom);
    if (indev && lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER)
        lv_group_set_editing(group, true);
}

void MapPlugin::saveHome()
{
    if (!map || !config)
        return;
    map->setHomePosition();
    float lat, lon;
    map->getHomeLocation(lat, lon);
    config->has_map_data = true;
    config->map_data.has_home = true;
    config->map_data.home.latitude = lat * 1e7;
    config->map_data.home.longitude = lon * 1e7;
    config->map_data.home.zoom = MapTileSettings::getZoomLevel();
    MapTileSettings::setDefaultZoom(MapTileSettings::getZoomLevel());
    if (saveConfig)
        saveConfig();
}

void MapPlugin::setFollowGps(bool enabled, bool persist)
{
    following = enabled;
    if (map)
        map->setLocked(enabled && hasGps);
    if (follow)
        lv_obj_set_state(follow, LV_STATE_CHECKED, enabled);
    if (config && persist && config->map_data.follow_gps != enabled) {
        config->has_map_data = true;
        config->map_data.follow_gps = enabled;
        if (saveConfig)
            saveConfig();
    }
}

void MapPlugin::updateStatus()
{
    if (!notice || !map || sourcesOpen || !visible)
        return;
    const auto status = map->getTileStatus();
    const bool online = TileProvider::selectedTemplate() >= 0;
    const bool failed = status == MapPanel::TileStatus::Incomplete || status == MapPanel::TileStatus::Unavailable;
    std::string text;
    NoticeAction action = NoticeAction::None;
    if (status == MapPanel::TileStatus::Ready) {
        // Already visible cached tiles remain useful when Wi-Fi or SD disappears.
    } else if (!mounted && !online) {
        text = "No SD card mounted\nInsert a card or choose an online source";
        action = NoticeAction::Source;
    } else if (!hasSdSource && !online) {
        text = "No map source\nChoose online tiles or add SD maps";
        action = NoticeAction::Source;
    } else if (!mounted && TileProvider::url() == osmUrl) {
        text = "SD card required for OpenStreetMap\nInsert a card for tile caching";
        action = NoticeAction::Source;
    } else if (failed && online && networkKnown && (!wifiEnabled || !wifiConnected)) {
        text = wifiEnabled ? "Wi-Fi disconnected\nOpen Wi-Fi settings to load map tiles"
                           : "Wi-Fi is off\nOpen Wi-Fi settings to load map tiles";
        action = onOpenWifi ? NoticeAction::Wifi : NoticeAction::Source;
    } else if (failed) {
        text = !online ? "SD tiles unavailable here\nTry another area, zoom or source"
                       : status == MapPanel::TileStatus::Incomplete ? "Some map tiles unavailable\nCheck the source or try another zoom"
                                                                   : "Map tiles unavailable\nCheck the connection or map source";
        action = NoticeAction::Source;
    } else {
        text = "Loading map tiles...";
    }
    if (group && lv_group_get_focused(group) == notice && (text.empty() || action == NoticeAction::None))
        focusZoom();
    noticeAction = action;
    if (text != statusText) {
        statusText = text;
        lv_label_set_text(noticeLabel, text.c_str());
    }
    lv_obj_set_flag(notice, LV_OBJ_FLAG_HIDDEN, text.empty());
    lv_obj_set_state(notice, LV_STATE_DISABLED, action == NoticeAction::None);
    if (!text.empty())
        lv_obj_move_foreground(notice);
}

void MapPlugin::showPanel()
{
    onShow();
}

void MapPlugin::onShow()
{
    visible = true;
    createMap();
    if (storageDirty)
        discoverSources();
    updateZoom();
    updateStatus();
    focusZoom();
}

void MapPlugin::onHide()
{
    visible = false;
    closeSources();
    if (group)
        lv_group_set_editing(group, false);
}

void MapPlugin::task_handler(time_t millis)
{
    GfxPlugin::task_handler(millis);
    if (!visible || !map)
        return;
    if (storageDirty)
        discoverSources();
    if (!sourcesOpen)
        map->task_handler();
    if (deferZoomFocus) {
        deferZoomFocus = false;
        focusZoom();
    }
    const auto now = lv_tick_get();
    if (now - lastStatus >= 250) {
        lastStatus = now;
        updateStatus();
        auto *title = getWidget(static_cast<WidgetIndex>(Widget::Title));
        if (title)
            lv_label_set_text_fmt(title, "Locations (%u/%u)", static_cast<unsigned>(map->getObjectsOnMap()),
                                  static_cast<unsigned>(nodes.size()));
    }
}

void MapPlugin::event(lv_event_t *e)
{
    static_cast<MapPlugin *>(lv_event_get_user_data(e))->handleMapEvent(e);
}

void MapPlugin::handleMapEvent(lv_event_t *e)
{
    auto *obj = lv_event_get_target_obj(e);
    const auto code = lv_event_get_code(e);
    if (code == LV_EVENT_SCREEN_LOADED) {
        onShow();
        return;
    }
    if (code == LV_EVENT_SCREEN_UNLOAD_START) {
        onHide();
        return;
    }
    if (code == LV_EVENT_SHORT_CLICKED && obj == getWidget(static_cast<WidgetIndex>(Widget::Back))) {
        if (onBack)
            onBack();
        return;
    }
    if (code == LV_EVENT_KEY && lv_event_get_key(e) == LV_KEY_ESC) {
        lv_event_stop_processing(e);
        if (sourcesOpen)
            closeSources();
        else if (onBack)
            onBack();
        return;
    }
    if (code == LV_EVENT_FOCUSED && obj != zoom && group && obj != urlInput && obj != sourceDropdown)
        lv_group_set_editing(group, false);
    if (code == LV_EVENT_VALUE_CHANGED) {
        if (obj == zoom && map) {
            map->setZoom(lv_slider_get_value(zoom));
            updateZoom();
        } else if (obj == sourceDropdown && sourcesOpen) {
            selectSource(lv_dropdown_get_selected(sourceDropdown), true);
            updateSourceControls();
        }
        return;
    }
    if (code == LV_EVENT_LONG_PRESSED && obj == home) {
        saveHome();
        return;
    }
    if (code == LV_EVENT_GESTURE && obj == canvas && map && map->redrawComplete()) {
        auto *active = lv_indev_active();
        if (active) {
            const auto direction = lv_indev_get_gesture_dir(active);
            setFollowGps(false, true);
            map->scroll(direction == LV_DIR_LEFT ? -1 : direction == LV_DIR_RIGHT ? 1 : 0,
                        direction == LV_DIR_TOP ? -1 : direction == LV_DIR_BOTTOM ? 1 : 0);
        }
        return;
    }
    // SHORT_CLICKED avoids moving home immediately after a long-press save.
    if (code == LV_EVENT_SHORT_CLICKED && obj == home && map) {
        setFollowGps(false, true);
        map->moveHome();
        updateZoom();
        return;
    }
    if (code != LV_EVENT_CLICKED)
        return;
    if (obj == sourceButton)
        showSources();
    else if (obj == closeButton)
        closeSources();
    else if (obj == applyButton)
        applyUrl();
    else if (obj == notice) {
        if (noticeAction == NoticeAction::Wifi && onOpenWifi)
            onOpenWifi();
        else if (noticeAction == NoticeAction::Source)
            showSources();
    } else if (obj == follow)
        setFollowGps(lv_obj_has_state(follow, LV_STATE_CHECKED), true);
    else if (obj == homeMarker && onOpenNode && ownNode)
        onOpenNode(ownNode);
    else if (map && (obj == panLeft || obj == panRight || obj == panUp || obj == panDown)) {
        if (map->redrawComplete()) {
            setFollowGps(false, true);
            map->scroll(obj == panLeft ? 1 : obj == panRight ? -1 : 0, obj == panUp ? 1 : obj == panDown ? -1 : 0);
        }
    } else if (onOpenNode) {
        for (const auto &node : nodes) {
            if (node.second.marker == obj) {
                onOpenNode(node.first);
                break;
            }
        }
    }
}

#endif // MUI_MAP_PLUGIN
