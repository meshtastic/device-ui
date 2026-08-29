#include "graphics/view/TFT/VirtualNodeList.h"

#include "core/lv_group_private.h"
#include "fonts.h"
#include "graphics/common/NodeListRowPresentation.h"
#include "images.h"
#include "lv_i18n.h"
#include "styles.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

namespace
{
VirtualNodeList *activeGroupNavigationList = nullptr;
const lv_color_t highlightOrange = lv_color_hex(0xff8c04);
const lv_color_t highlightBlueGreen = lv_color_hex(0x05f6cb);
const lv_color_t highlightBlue = lv_color_hex(0x436c70);
const lv_color_t highlightMidGray = lv_color_hex(0x808080);
const lv_color_t highlightMesh = lv_color_hex(0x67ea94);

size_t rowPoolSizeForViewport(lv_obj_t *parentPanel)
{
    lv_obj_update_layout(parentPanel);
    const int32_t contentHeight = lv_obj_get_height(parentPanel) - lv_obj_get_style_pad_top(parentPanel, LV_PART_MAIN) -
                                  lv_obj_get_style_pad_bottom(parentPanel, LV_PART_MAIN);
    if (contentHeight <= 0) {
        return 1;
    }
    constexpr int32_t rowPitch = VirtualNodeList::COLLAPSED_ROW_HEIGHT + VirtualNodeList::ROW_GAP;
    const size_t visibleRows = static_cast<size_t>(contentHeight + rowPitch * 2 - 2) / static_cast<size_t>(rowPitch);
    return std::clamp(visibleRows + 1, size_t{1}, VirtualNodeList::MAX_POOL_SIZE);
}

bool sameRenderContext(const NodeListRenderContext &left, const NodeListRenderContext &right)
{
    return left.ownNode == right.ownNode && left.hasOwnPosition == right.hasOwnPosition &&
           left.ownLatitude == right.ownLatitude && left.ownLongitude == right.ownLongitude &&
           left.metricUnits == right.metricUnits && left.highlightActiveChat == right.highlightActiveChat &&
           left.highlightPosition == right.highlightPosition && left.highlightTelemetry == right.highlightTelemetry &&
           left.highlightIaq == right.highlightIaq && std::strcmp(left.highlightName, right.highlightName) == 0;
}

template <size_t Size> void setRowText(lv_obj_t *label, char (&storage)[Size], const char *text)
{
    std::snprintf(storage, Size, "%s", text ? text : "");
    lv_label_set_text_static(label, storage);
}

void formatLastHeard(uint32_t lastHeard, uint32_t currentTime, char *buffer, size_t bufferSize)
{
    NodeListRowPresentation::formatLastHeardLabel(lastHeard, currentTime, _("now"), buffer, bufferSize);
}

void formatShortName(const NodeRecord &record, const NodeListRenderContext &context, char *buffer, size_t bufferSize)
{
    // Remote rows carry a distance line only when both positions are known.
    if (!context.hasOwnPosition || record.id == context.ownNode || !record.position.hasCoordinates()) {
        NodeListRowPresentation::formatShortDisplayName(buffer, bufferSize, record.user.short_name, record.id);
        return;
    }
    NodeListRowPresentation::formatShortNameWithDistance(
        buffer, bufferSize, record.user.short_name, record.id, context.hasOwnPosition, context.ownLatitude, context.ownLongitude,
        record.position.latitude, record.position.longitude, context.metricUnits);
}

void setHidden(lv_obj_t *obj, bool hidden)
{
    if (hidden) {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

void clearRowBinding(ReusableRow &row)
{
    row.boundId = 0;
    row.pressedId = 0;
    row.detailKeysValid = false;
    if (row.panel) {
        lv_obj_set_user_data(row.panel, nullptr);
    }
    if (row.btn) {
        lv_obj_set_user_data(row.btn, nullptr);
    }
    if (row.lblPos1) {
        lv_obj_set_user_data(row.lblPos1, nullptr);
    }
}

void moveGroupButtonToHead(lv_group_t *group, lv_obj_t *button)
{
    if (!group || !button) {
        return;
    }

    lv_obj_t **buttonNode = nullptr;
    lv_obj_t **head = static_cast<lv_obj_t **>(lv_ll_get_head(&group->obj_ll));
    if (head && *head == button) {
        return;
    }

    for (void *nodePtr = lv_ll_get_head(&group->obj_ll); nodePtr; nodePtr = lv_ll_get_next(&group->obj_ll, nodePtr)) {
        auto **node = static_cast<lv_obj_t **>(nodePtr);
        if (*node == button) {
            buttonNode = node;
            break;
        }
    }

    if (buttonNode && head) {
        lv_ll_move_before(&group->obj_ll, buttonNode, head);
    }
}

void moveGroupButtonToTail(lv_group_t *group, lv_obj_t *button)
{
    if (!group || !button) {
        return;
    }

    auto **tail = static_cast<lv_obj_t **>(lv_ll_get_tail(&group->obj_ll));
    if (tail && *tail == button) {
        return;
    }

    for (void *nodePtr = lv_ll_get_head(&group->obj_ll); nodePtr; nodePtr = lv_ll_get_next(&group->obj_ll, nodePtr)) {
        auto **node = static_cast<lv_obj_t **>(nodePtr);
        if (*node == button) {
            lv_ll_move_before(&group->obj_ll, node, nullptr);
            return;
        }
    }
}

void setRoleImage(const NodeRecord &record, lv_obj_t *img)
{
    const void *source = &img_node_client_image;
    if (record.unmessagable) {
        source = &img_unmessagable_image;
    } else {
        switch (record.user.role) {
        case meshtastic_Config_DeviceConfig_Role_ROUTER:
        case meshtastic_Config_DeviceConfig_Role_REPEATER:
        case meshtastic_Config_DeviceConfig_Role_ROUTER_LATE:
            source = &img_node_router_image;
            break;
        case meshtastic_Config_DeviceConfig_Role_ROUTER_CLIENT:
            source = &img_top_nodes_image;
            break;
        case meshtastic_Config_DeviceConfig_Role_TRACKER:
        case meshtastic_Config_DeviceConfig_Role_SENSOR:
        case meshtastic_Config_DeviceConfig_Role_LOST_AND_FOUND:
        case meshtastic_Config_DeviceConfig_Role_TAK_TRACKER:
            source = &img_node_sensor_image;
            break;
        case meshtastic_Config_DeviceConfig_Role_CLIENT:
        case meshtastic_Config_DeviceConfig_Role_CLIENT_MUTE:
        case meshtastic_Config_DeviceConfig_Role_CLIENT_HIDDEN:
        case meshtastic_Config_DeviceConfig_Role_TAK:
            source = &img_node_client_image;
            break;
        default:
            source = record.hasUser ? &img_node_client_image : &img_circle_question_image;
            break;
        }
    }
    NodeListRowPresentation::applyNodeImage(img, record.id, source, record.unmessagable, true);
}
} // namespace

VirtualNodeList::VirtualNodeList(lv_obj_t *parent, NodeListActionSink &sink) : parentPanel(parent), actionSink(sink)
{
    if (parentPanel) {
        lv_obj_add_event_cb(parentPanel, scrollEventCallback, LV_EVENT_SCROLL, this);
        lv_obj_add_event_cb(parentPanel, scrollEventCallback, LV_EVENT_SCROLL_END, this);
        createRowPool();
        attachGroupNavigation();
    }
}

VirtualNodeList::~VirtualNodeList()
{
    lv_anim_delete(this, expansionAnimationCallback);
    detachGroupNavigation();
    if (parentPanel) {
        lv_obj_remove_event_cb_with_user_data(parentPanel, scrollEventCallback, this);
    }
    if (spacer) {
        lv_obj_delete(spacer);
        spacer = nullptr;
    }
    for (auto &row : rowPool) {
        if (row.panel) {
            lv_obj_delete(row.panel);
            row.panel = nullptr;
        }
    }
    rowPool.clear();
}

void VirtualNodeList::createRowPool()
{
    if (!parentPanel) {
        return;
    }

    // Disable flex layout on parent so rows use absolute virtual coordinates
    lv_obj_set_style_layout(parentPanel, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Create spacer for virtual scroll height
    spacer = lv_obj_create(parentPanel);
    lv_obj_set_size(spacer, 1, 1);
    lv_obj_set_pos(spacer, 0, 0);
    lv_obj_remove_flag(spacer, static_cast<lv_obj_flag_t>(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));
    lv_obj_set_style_opa(spacer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(spacer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(spacer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    rowPool.resize(rowPoolSizeForViewport(parentPanel));

    for (size_t i = 0; i < rowPool.size(); ++i) {
        auto &row = rowPool[i];

        row.panel = lv_obj_create(parentPanel);
        lv_obj_set_pos(row.panel, 0, 0);
        lv_obj_set_size(row.panel, lv_pct(100), COLLAPSED_ROW_HEIGHT);
        lv_obj_set_align(row.panel, LV_ALIGN_TOP_LEFT);
        lv_obj_set_style_pad_top(row.panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_bottom(row.panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_remove_flag(row.panel, static_cast<lv_obj_flag_t>(LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_GESTURE_BUBBLE));
        add_style_node_panel_style(row.panel);

        row.img = lv_image_create(row.panel);
        lv_obj_set_pos(row.img, -5, 3);
        lv_obj_set_size(row.img, 32, 32);
        lv_image_set_src(row.img, &img_node_client_image);
        lv_image_set_pivot(row.img, 0, 0);
        lv_obj_clear_flag(row.img, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_align(row.img, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(row.img, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_image_recolor_opa(row.img, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(row.img, lv_color_hex(0x5d9388), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(row.img, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(row.img, lv_color_hex(0xff5555), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(row.img, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(row.img, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

        row.btn = lv_button_create(row.panel);
        lv_obj_set_pos(row.btn, 0, 0);
        lv_obj_set_size(row.btn, lv_pct(106), lv_pct(100));
        lv_obj_set_align(row.btn, LV_ALIGN_CENTER);
        add_style_node_button_style(row.btn);
        lv_obj_set_style_shadow_width(row.btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_max_height(row.btn, 132, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_min_height(row.btn, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_flag(row.btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_add_event_cb(row.btn, rowClickCallback, LV_EVENT_ALL, this);

        row.lblLong = lv_label_create(row.panel);
        lv_obj_set_pos(row.lblLong, -5, 35);
        lv_obj_set_size(row.lblLong, lv_pct(80), LV_SIZE_CONTENT);
        lv_label_set_long_mode(row.lblLong, LV_LABEL_LONG_SCROLL);
        lv_obj_set_style_align(row.lblLong, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

        row.lblShort = lv_label_create(row.panel);
        lv_obj_set_pos(row.lblShort, 30, 10);
        lv_obj_set_size(row.lblShort, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_label_set_long_mode(row.lblShort, LV_LABEL_LONG_WRAP);
        lv_obj_remove_flag(row.lblShort, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_align(row.lblShort, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(row.lblShort, &ui_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

        row.lblBat = lv_label_create(row.panel);
        lv_obj_set_pos(row.lblBat, 8, 17);
        lv_obj_set_size(row.lblBat, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_label_set_long_mode(row.lblBat, LV_LABEL_LONG_CLIP);
        lv_obj_remove_flag(row.lblBat, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_align(row.lblBat, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(row.lblBat, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

        row.lblLh = lv_label_create(row.panel);
        lv_obj_set_pos(row.lblLh, 8, 33);
        lv_obj_set_size(row.lblLh, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_label_set_long_mode(row.lblLh, LV_LABEL_LONG_CLIP);
        lv_obj_remove_flag(row.lblLh, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_align(row.lblLh, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(row.lblLh, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

        row.lblSig = lv_label_create(row.panel);
        lv_obj_set_width(row.lblSig, LV_SIZE_CONTENT);
        lv_obj_set_height(row.lblSig, LV_SIZE_CONTENT);
        lv_obj_set_pos(row.lblSig, 8, 1);
        lv_obj_remove_flag(row.lblSig, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_align(row.lblSig, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(row.lblSig, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

        row.lblPos1 = lv_label_create(row.panel);
        lv_obj_set_pos(row.lblPos1, -5, 49);
        lv_obj_set_size(row.lblPos1, 120, LV_SIZE_CONTENT);
        lv_label_set_long_mode(row.lblPos1, LV_LABEL_LONG_CLIP);
        lv_obj_remove_flag(row.lblPos1, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_align(row.lblPos1, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(row.lblPos1, lv_color_hex(0x05f6cb), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_event_cb(row.lblPos1, rowPositionCallback, LV_EVENT_CLICKED, this);
        lv_obj_add_flag(row.lblPos1, LV_OBJ_FLAG_HIDDEN);

        row.lblPos2 = lv_label_create(row.panel);
        lv_obj_set_pos(row.lblPos2, -5, 63);
        lv_obj_set_size(row.lblPos2, 108, LV_SIZE_CONTENT);
        lv_label_set_long_mode(row.lblPos2, LV_LABEL_LONG_SCROLL);
        lv_obj_remove_flag(row.lblPos2, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_align(row.lblPos2, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_flag(row.lblPos2, LV_OBJ_FLAG_HIDDEN);

        row.lblTm1 = lv_label_create(row.panel);
        lv_obj_set_pos(row.lblTm1, 8, 49);
        lv_obj_set_size(row.lblTm1, 130, LV_SIZE_CONTENT);
        lv_label_set_long_mode(row.lblTm1, LV_LABEL_LONG_CLIP);
        lv_obj_set_style_align(row.lblTm1, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(row.lblTm1, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_flag(row.lblTm1, LV_OBJ_FLAG_HIDDEN);

        row.lblTm2 = lv_label_create(row.panel);
        lv_obj_set_pos(row.lblTm2, 8, 63);
        lv_obj_set_size(row.lblTm2, 130, LV_SIZE_CONTENT);
        lv_label_set_long_mode(row.lblTm2, LV_LABEL_LONG_CLIP);
        lv_obj_set_style_align(row.lblTm2, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(row.lblTm2, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_flag(row.lblTm2, LV_OBJ_FLAG_HIDDEN);

        lv_label_set_text_static(row.lblShort, row.shortText);
        lv_label_set_text_static(row.lblLong, row.longText);
        lv_label_set_text_static(row.lblBat, row.batteryText);
        lv_label_set_text_static(row.lblLh, row.lastHeardText);
        lv_label_set_text_static(row.lblSig, row.signalText);
        lv_label_set_text_static(row.lblPos1, row.positionText);
        lv_label_set_text_static(row.lblPos2, row.position2Text);
        lv_label_set_text_static(row.lblTm1, row.telemetry1Text);
        lv_label_set_text_static(row.lblTm2, row.telemetry2Text);

        lv_obj_add_flag(row.panel, LV_OBJ_FLAG_HIDDEN);
    }
}

void VirtualNodeList::attachGroupNavigation()
{
    attachedGroup = lv_group_create();
    if (!attachedGroup) {
        return;
    }

    activeGroupNavigationList = this;
    lv_group_set_wrap(attachedGroup, false);
    lv_group_set_edge_cb(attachedGroup, groupEdgeCallback);
}

void VirtualNodeList::detachGroupNavigation()
{
    if (attachedGroup && activeGroupNavigationList == this) {
        activeGroupNavigationList = nullptr;
    }
    if (attachedGroup) {
        for (auto &row : rowPool) {
            if (row.btn) {
                lv_group_remove_obj(row.btn);
            }
        }
        lv_group_delete(attachedGroup);
    }
    attachedGroup = nullptr;
}

void VirtualNodeList::updateVirtualContentHeight()
{
    if (!currentIndex) {
        return;
    }

    const auto &ids = currentIndex->ids();

    if (spacer) {
        const int32_t height = rowY(ids.size());
        lv_obj_set_size(spacer, 1, height > 0 ? height : 1);
        lv_obj_set_pos(spacer, 0, 0);
    }
}

void VirtualNodeList::updateExpandedIndexCache()
{
    // The scan reruns only when the expanded ids or the index actually changed.
    if (expansionCacheValid && expansionCacheIndex == currentIndex &&
        expansionCacheGeneration == (currentIndex ? currentIndex->generation() : 0) && expansionCacheExpandedId == expandedId &&
        expansionCachePreviousId == previousExpandedId) {
        return;
    }

    expandedIndex = std::numeric_limits<size_t>::max();
    previousExpandedIndex = std::numeric_limits<size_t>::max();

    if (!currentIndex) {
        return;
    }

    const auto cacheIndex = [this](NodeId id) {
        if (!id) {
            return std::numeric_limits<size_t>::max();
        }
#ifdef UNIT_TEST
        ++expandedIndexScanCount;
#endif
        const auto position = currentIndex->indexOf(id);
        return position.value_or(std::numeric_limits<size_t>::max());
    };

    expandedIndex = cacheIndex(expandedId);
    if (previousExpandedId != expandedId) {
        previousExpandedIndex = cacheIndex(previousExpandedId);
    }

    expansionCacheValid = true;
    expansionCacheIndex = currentIndex;
    expansionCacheGeneration = currentIndex->generation();
    expansionCacheExpandedId = expandedId;
    expansionCachePreviousId = previousExpandedId;
}

VirtualNodeList::ScrollAnchor VirtualNodeList::captureScrollAnchor() const
{
    if (!parentPanel) {
        return {};
    }

    const int32_t scrollY = std::max<int32_t>(0, static_cast<int32_t>(lv_obj_get_scroll_y(parentPanel)));
    if (scrollY == 0) {
        return {};
    }
    const ReusableRow *anchorRow = nullptr;
    for (const auto &row : rowPool) {
        if (!row.boundId || !row.panel || lv_obj_has_flag(row.panel, LV_OBJ_FLAG_HIDDEN) || row.renderedY > scrollY) {
            continue;
        }
        if (!anchorRow || row.renderedY > anchorRow->renderedY) {
            anchorRow = &row;
        }
    }

    if (!anchorRow) {
        return {};
    }
    return {anchorRow->boundId, scrollY - anchorRow->renderedY};
}

void VirtualNodeList::restoreScrollAnchor(const ScrollAnchor &anchor)
{
    if (!parentPanel || !currentIndex || !anchor.id) {
        return;
    }

    const auto position = currentIndex->indexOf(anchor.id);
    if (position.has_value()) {
        lv_obj_scroll_to_y(parentPanel, rowY(position.value()) + anchor.offset, LV_ANIM_OFF);
    }
}

void VirtualNodeList::sync(const NodeStore &store, const VisibleNodeIndex &index, NodeId expanded, uint32_t now,
                           const NodeListRenderContext &context, bool forceRebind)
{
    const uint32_t nextTime = now != 0 ? now : static_cast<uint32_t>(std::time(nullptr));
    const bool indexChanged = currentIndex != &index || lastSyncedIndexGeneration != index.generation();
    const bool timeChanged = currentTime != nextTime && visibleLastHeardLabelsNeedRefresh(store, nextTime, context);
    const bool contextChanged = !sameRenderContext(renderContext, context);
    const ScrollAnchor scrollAnchor = indexChanged ? captureScrollAnchor() : ScrollAnchor{};
    const NodeId focusedId = currentFocusedId();
    const size_t fallbackFocusIndex = lastFocusedIndex;
    currentStore = &store;
    currentIndex = &index;
    requestExpanded(expanded);
    updateExpandedIndexCache();
    currentTime = nextTime;
    renderContext = context;

    const bool rebindVisibleRows = forceRebind || timeChanged || contextChanged;
    updateVirtualContentHeight();
    if (indexChanged) {
        restoreScrollAnchor(scrollAnchor);
    }
    refreshVisibleRows(indexChanged || rebindVisibleRows, rebindVisibleRows);
    restoreFocus(focusedId, fallbackFocusIndex);
    lastSyncedIndexGeneration = index.generation();
}

bool VirtualNodeList::visibleLastHeardLabelsNeedRefresh(const NodeStore &store, uint32_t nextTime,
                                                        const NodeListRenderContext &context) const
{
    if (currentTime == 0) {
        return false;
    }

    for (const auto &row : rowPool) {
        if (!row.boundId || !row.panel || lv_obj_has_flag(row.panel, LV_OBJ_FLAG_HIDDEN)) {
            continue;
        }
        const NodeRecord *record = store.find(row.boundId);
        if (!record) {
            continue;
        }
        const uint32_t effectiveLastHeard = record->id != 0 && record->id == context.ownNode ? nextTime : record->lastHeard;
        char nextText[sizeof(row.lastHeardText)]{};
        formatLastHeard(effectiveLastHeard, nextTime, nextText, sizeof(nextText));
        if (std::strcmp(nextText, row.lastHeardText) != 0) {
            return true;
        }
    }
    return false;
}

void VirtualNodeList::setExpanded(NodeId id)
{
    requestExpanded(id);
}

int32_t VirtualNodeList::rowHeight(NodeId id) const
{
    if (!expansionAnimationActive) {
        return id == expandedId ? EXPANDED_ROW_HEIGHT : COLLAPSED_ROW_HEIGHT;
    }
    if (id == expandedId && expandedId != 0) {
        return COLLAPSED_ROW_HEIGHT + (EXPANDED_ROW_HEIGHT - COLLAPSED_ROW_HEIGHT) * expansionProgress / 100;
    }
    if (id == previousExpandedId && previousExpandedId != 0) {
        return EXPANDED_ROW_HEIGHT - (EXPANDED_ROW_HEIGHT - COLLAPSED_ROW_HEIGHT) * expansionProgress / 100;
    }
    return COLLAPSED_ROW_HEIGHT;
}

int32_t VirtualNodeList::rowY(size_t index) const
{
    if (!currentIndex) {
        return 0;
    }

    const auto &ids = currentIndex->ids();
    const size_t count = ids.size();
    int32_t y = static_cast<int32_t>(index * (COLLAPSED_ROW_HEIGHT + ROW_GAP));
    if (index == count && count > 0) {
        y -= ROW_GAP;
    }

    const auto addExpandedDelta = [&](NodeId id, size_t position) {
        if (!id || position == std::numeric_limits<size_t>::max()) {
            return;
        }
        if (position < index) {
            y += rowHeight(id) - COLLAPSED_ROW_HEIGHT;
        }
    };
    addExpandedDelta(expandedId, expandedIndex);
    if (previousExpandedId != expandedId) {
        addExpandedDelta(previousExpandedId, previousExpandedIndex);
    }
    return y;
}

size_t VirtualNodeList::firstVisibleIndex(int32_t scrollY) const
{
    if (!currentIndex || currentIndex->ids().empty()) {
        return 0;
    }

    const size_t count = currentIndex->ids().size();
    size_t first = 0;
    size_t last = count;
    while (first < last) {
        const size_t middle = first + (last - first) / 2;
        if (rowY(middle + 1) <= scrollY) {
            first = middle + 1;
        } else {
            last = middle;
        }
    }
    return first;
}

bool VirtualNodeList::rowShowsExpandedDetails(NodeId id) const
{
    return id != 0 && (id == expandedId || (expansionAnimationActive && id == previousExpandedId));
}

int32_t VirtualNodeList::viewportHeight() const
{
    if (!parentPanel) {
        return 0;
    }
    return std::max<int32_t>(0, lv_obj_get_height(parentPanel) - lv_obj_get_style_pad_top(parentPanel, LV_PART_MAIN) -
                                    lv_obj_get_style_pad_bottom(parentPanel, LV_PART_MAIN));
}

int32_t VirtualNodeList::scrollYForNode(NodeId id) const
{
    if (!currentIndex || !parentPanel) {
        return 0;
    }
    const auto optIdx = currentIndex->indexOf(id);
    if (!optIdx.has_value()) {
        return std::max<int32_t>(0, lv_obj_get_scroll_y(parentPanel));
    }

    const int32_t scrollY = std::max<int32_t>(0, lv_obj_get_scroll_y(parentPanel));
    const int32_t top = rowY(optIdx.value());
    const int32_t bottom = top + rowHeight(id);
    const int32_t height = viewportHeight();
    if (top < scrollY) {
        return top;
    }
    if (height > 0 && bottom > scrollY + height) {
        return bottom - height;
    }
    return scrollY;
}

NodeId VirtualNodeList::currentFocusedId() const
{
    if (!attachedGroup) {
        return 0;
    }
    lv_obj_t *focused = lv_group_get_focused(attachedGroup);
    const size_t poolIndex = poolIndexForButton(focused);
    if (poolIndex >= rowPool.size()) {
        return 0;
    }
    return rowPool[poolIndex].boundId;
}

void VirtualNodeList::restoreFocus(NodeId focusedId, size_t fallbackIndex)
{
    if (!focusedId || !currentIndex || !attachedGroup || currentIndex->empty()) {
        return;
    }

    NodeId targetId = focusedId;
    if (!currentIndex->contains(targetId)) {
        const auto &ids = currentIndex->ids();
        if (fallbackIndex >= ids.size()) {
            fallbackIndex = ids.size() - 1;
        }
        targetId = ids[fallbackIndex];
    }
    focus(targetId);
}

void VirtualNodeList::requestExpanded(NodeId id)
{
    if (expansionAnimationActive) {
        if (id != expandedId) {
            pendingExpandedId = id;
            hasPendingExpandedId = true;
        } else {
            pendingExpandedId = 0;
            hasPendingExpandedId = false;
        }
        updateExpandedIndexCache();
        updateVirtualContentHeight();
        refreshVisibleRows(true, false, false);
        return;
    }

    if (id == expandedId) {
        return;
    }

    hasPendingExpandedId = false;
    previousExpandedId = expandedId;
    expandedId = id;
    expansionProgress = 0;
    expansionAnimationActive = true;
    updateExpandedIndexCache();

    lv_anim_t animation;
    lv_anim_init(&animation);
    lv_anim_set_var(&animation, this);
    lv_anim_set_values(&animation, 0, 100);
    lv_anim_set_duration(&animation, 200);
    lv_anim_set_path_cb(&animation, lv_anim_path_linear);
    lv_anim_set_exec_cb(&animation, expansionAnimationCallback);
    lv_anim_set_completed_cb(&animation, expansionAnimationFinished);
    lv_anim_start(&animation);

    updateVirtualContentHeight();
    refreshVisibleRows(true, true);
}

void VirtualNodeList::updateExpansionAnimation(int32_t progress)
{
    expansionProgress = progress;
    updateVirtualContentHeight();
    refreshVisibleRows(true, false, false);
}

void VirtualNodeList::finishExpansionAnimation()
{
    expansionProgress = 100;
    expansionAnimationActive = false;
    previousExpandedId = 0;
    updateExpandedIndexCache();

    if (hasPendingExpandedId) {
        const NodeId nextExpandedId = pendingExpandedId;
        hasPendingExpandedId = false;
        if (nextExpandedId != expandedId) {
            requestExpanded(nextExpandedId);
            return;
        }
    }

    updateVirtualContentHeight();
    refreshVisibleRows(true, true);
}

void VirtualNodeList::scrollTo(NodeId id, lv_anim_enable_t anim)
{
    if (!currentIndex || !parentPanel) {
        return;
    }
    const auto optIdx = currentIndex->indexOf(id);
    if (!optIdx.has_value()) {
        return;
    }

    lv_obj_scroll_to_y(parentPanel, scrollYForNode(id), anim);
    refreshVisibleRows();
}

void VirtualNodeList::focus(NodeId id)
{
    scrollTo(id, LV_ANIM_OFF);
    for (auto &row : rowPool) {
        if (row.boundId == id && !lv_obj_has_flag(row.panel, LV_OBJ_FLAG_HIDDEN)) {
            lv_group_focus_obj(row.btn);
            noteFocusedButton(row.btn);
            break;
        }
    }
}

size_t VirtualNodeList::poolIndexForButton(lv_obj_t *button) const
{
    for (size_t i = 0; i < rowPool.size(); ++i) {
        if (rowPool[i].btn == button) {
            return i;
        }
    }
    return rowPool.size();
}

void VirtualNodeList::noteFocusedButton(lv_obj_t *button)
{
    const size_t poolIndex = poolIndexForButton(button);
    if (poolIndex >= rowPool.size()) {
        return;
    }

    lastFocusedId = rowPool[poolIndex].boundId;
    if (currentIndex) {
        lastFocusedIndex = currentIndex->indexOf(lastFocusedId).value_or(std::numeric_limits<size_t>::max());
    }
}

bool VirtualNodeList::focusAdjacent(NodeId id, int direction)
{
    if (!currentIndex || direction == 0) {
        return false;
    }
    const auto current = currentIndex->indexOf(id);
    if (!current.has_value()) {
        return false;
    }

    const auto &ids = currentIndex->ids();
    const auto target = static_cast<int64_t>(current.value()) + direction;
    if (target < 0 || target >= static_cast<int64_t>(ids.size())) {
        return false;
    }

    focus(ids[static_cast<size_t>(target)]);
    return true;
}

void VirtualNodeList::handleGroupEdge(bool forward)
{
    NodeId focusedId = lastFocusedId;
    if (attachedGroup) {
        lv_obj_t *focused = lv_group_get_focused(attachedGroup);
        const size_t poolIndex = poolIndexForButton(focused);
        if (poolIndex < rowPool.size() && rowPool[poolIndex].boundId != 0) {
            focusedId = rowPool[poolIndex].boundId;
            lastFocusedId = focusedId;
        }
    }

    if (focusedId == 0) {
        actionSink.nodeFocusBoundary(forward);
        return;
    }

    const bool moved = focusAdjacent(focusedId, forward ? 1 : -1);
    if (!moved) {
        actionSink.nodeFocusBoundary(forward);
    }
}

void VirtualNodeList::bindRow(ReusableRow &row, const NodeRecord &record, bool isExpanded)
{
    row.boundId = record.id;
    lv_obj_set_user_data(row.panel, reinterpret_cast<void *>(static_cast<uintptr_t>(record.id)));
    lv_obj_set_user_data(row.btn, reinterpret_cast<void *>(static_cast<uintptr_t>(record.id)));
    lv_obj_set_user_data(row.lblPos1, reinterpret_cast<void *>(static_cast<uintptr_t>(record.id)));

    setRoleImage(record, row.img);
    if (!record.hasKey || record.hasBadKey) {
        lv_obj_set_style_border_color(row.img, lv_color_hex(0xff5555), LV_PART_MAIN | LV_STATE_DEFAULT);
    } else if (!record.unmessagable) {
        lv_obj_set_style_border_color(row.img, lv_obj_get_style_bg_color(row.img, LV_PART_MAIN), LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    const bool shortTextChanged =
        !row.detailKeysValid || row.detailShortId != record.id || row.detailOwnNode != renderContext.ownNode ||
        std::memcmp(row.detailShortName, record.user.short_name, sizeof(row.detailShortName)) != 0 ||
        row.detailHasOwnPosition != renderContext.hasOwnPosition || row.detailOwnLatitude != renderContext.ownLatitude ||
        row.detailOwnLongitude != renderContext.ownLongitude || row.detailLatitude != record.position.latitude ||
        row.detailLongitude != record.position.longitude || row.detailMetricUnits != renderContext.metricUnits;
    if (shortTextChanged) {
        formatShortName(record, renderContext, row.shortText, sizeof(row.shortText));
        lv_label_set_text_static(row.lblShort, row.shortText);
        lv_obj_set_pos(row.lblShort, 30, std::strchr(row.shortText, '\n') ? -1 : 10);
        row.detailShortId = record.id;
        row.detailOwnNode = renderContext.ownNode;
        std::memcpy(row.detailShortName, record.user.short_name, sizeof(row.detailShortName));
        row.detailHasOwnPosition = renderContext.hasOwnPosition;
        row.detailOwnLatitude = renderContext.ownLatitude;
        row.detailOwnLongitude = renderContext.ownLongitude;
    }
    setRowText(row.lblLong, row.longText, record.user.long_name);

    const bool hasBattery =
        record.hasDeviceMetrics && (record.deviceMetrics.battery_level != 0 || record.deviceMetrics.voltage != 0.0f);
    if (!row.detailKeysValid || row.detailHasBattery != hasBattery ||
        (hasBattery &&
         (row.detailBatteryLevel != record.deviceMetrics.battery_level || row.detailVoltage != record.deviceMetrics.voltage))) {
        if (hasBattery) {
            NodeListRowPresentation::formatBatteryLabel(record.deviceMetrics.battery_level, record.deviceMetrics.voltage,
                                                        row.batteryText, sizeof(row.batteryText));
        } else {
            row.batteryText[0] = '\0';
        }
        lv_label_set_text_static(row.lblBat, row.batteryText);
        row.detailHasBattery = hasBattery;
        row.detailBatteryLevel = record.deviceMetrics.battery_level;
        row.detailVoltage = record.deviceMetrics.voltage;
    }

    // The own-node row always reports "now"; ordering is pinned by the visible
    // index even as the model last-heard timestamp continues to update.
    const uint32_t effectiveLastHeard = record.id != 0 && record.id == renderContext.ownNode ? currentTime : record.lastHeard;
    formatLastHeard(effectiveLastHeard, currentTime, row.lastHeardText, sizeof(row.lastHeardText));
    lv_label_set_text_static(row.lblLh, row.lastHeardText);

    if (record.id == renderContext.ownNode && record.hasDeviceMetrics) {
        std::snprintf(row.signalText, sizeof(row.signalText), _("Util %0.1f%%  Air %0.1f%%"),
                      record.deviceMetrics.channel_utilization, record.deviceMetrics.air_util_tx);
    } else if (record.signalDisplay == NodeSignalDisplayKind::Hops && record.hopsAway >= 0) {
        std::snprintf(row.signalText, sizeof(row.signalText), _("hops: %d"), static_cast<int>(record.hopsAway));
    } else if (record.signalDisplay == NodeSignalDisplayKind::Rssi && (record.rssi != 0 || record.snr != 0.0f)) {
        std::snprintf(row.signalText, sizeof(row.signalText), "rssi: %d snr: %.1f", static_cast<int>(record.rssi), record.snr);
    } else {
        row.signalText[0] = '\0';
    }
    lv_label_set_text_static(row.lblSig, "");
    lv_label_set_text_static(row.lblSig, row.signalText);
    lv_obj_set_width(row.lblSig, LV_SIZE_CONTENT);
    lv_obj_set_height(row.lblSig, LV_SIZE_CONTENT);

    const bool showPosition = record.position.hasCoordinates();
    const bool positionChanged = !row.detailKeysValid || row.detailLatitude != record.position.latitude ||
                                 row.detailLongitude != record.position.longitude ||
                                 row.detailAltitude != record.position.altitude ||
                                 row.detailMetricUnits != renderContext.metricUnits;
    if (positionChanged) {
        if (showPosition) {
            NodeListRowPresentation::formatPositionLines(record.position.latitude, record.position.longitude,
                                                         record.position.altitude, renderContext.metricUnits, row.positionText,
                                                         sizeof(row.positionText), row.position2Text, sizeof(row.position2Text));
        } else {
            row.positionText[0] = '\0';
            row.position2Text[0] = '\0';
        }
        lv_label_set_text_static(row.lblPos1, row.positionText);
        lv_label_set_text_static(row.lblPos2, row.position2Text);
        row.detailLatitude = record.position.latitude;
        row.detailLongitude = record.position.longitude;
        row.detailAltitude = record.position.altitude;
    }
    if (isExpanded && showPosition && record.id != renderContext.ownNode) {
        lv_obj_add_flag(row.lblPos1, LV_OBJ_FLAG_CLICKABLE);
    } else {
        lv_obj_remove_flag(row.lblPos1, LV_OBJ_FLAG_CLICKABLE);
    }
    setHidden(row.lblPos1, !isExpanded || !showPosition);
    setHidden(row.lblPos2, !isExpanded || !showPosition);

    const bool showTelemetry1 = record.hasEnvironmentMetrics;
    const auto &env = record.environmentMetrics;
    const bool environmentChanged =
        !row.detailKeysValid || row.detailHasEnvironment != showTelemetry1 ||
        row.detailMetricUnits != renderContext.metricUnits ||
        (showTelemetry1 &&
         (row.detailEnvironment.temperature != env.temperature ||
          row.detailEnvironment.relative_humidity != env.relative_humidity ||
          row.detailEnvironment.barometric_pressure != env.barometric_pressure || row.detailEnvironment.iaq != env.iaq ||
          row.detailEnvironment.voltage != env.voltage || row.detailEnvironment.current != env.current));
    if (environmentChanged) {
        if (showTelemetry1) {
            NodeListRowPresentation::formatTelemetryLines(record.environmentMetrics, renderContext.metricUnits,
                                                          row.telemetry1Text, sizeof(row.telemetry1Text), row.telemetry2Text,
                                                          sizeof(row.telemetry2Text));
        } else {
            row.telemetry1Text[0] = '\0';
            row.telemetry2Text[0] = '\0';
        }
        lv_label_set_text_static(row.lblTm1, row.telemetry1Text);
        lv_label_set_text_static(row.lblTm2, row.telemetry2Text);
        row.detailHasEnvironment = showTelemetry1;
        if (showTelemetry1) {
            row.detailEnvironment = env;
        }
    }
    setHidden(row.lblTm1, !isExpanded || !showTelemetry1);
    const bool showTelemetry2 =
        record.hasEnvironmentMetrics && record.environmentMetrics.iaq > 0 && record.environmentMetrics.iaq < 1000;
    setHidden(row.lblTm2, !isExpanded || !showTelemetry2);

    row.detailMetricUnits = renderContext.metricUnits;
    row.detailKeysValid = true;

    applyHighlight(row, record);
}

void VirtualNodeList::applyHighlight(ReusableRow &row, const NodeRecord &record)
{
    lv_obj_set_style_border_width(row.panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    bool highlighted = false;
    if (renderContext.highlightActiveChat && record.hasActiveChat) {
        lv_obj_set_style_border_color(row.panel, highlightOrange, LV_PART_MAIN | LV_STATE_DEFAULT);
        highlighted = true;
    }
    if (renderContext.highlightPosition && record.position.hasCoordinates()) {
        lv_obj_set_style_border_color(row.panel, highlightBlueGreen, LV_PART_MAIN | LV_STATE_DEFAULT);
        highlighted = true;
    }
    if (renderContext.highlightTelemetry && record.hasEnvironmentMetrics) {
        lv_obj_set_style_border_color(row.panel, highlightBlue, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(row.panel, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        highlighted = true;
    }
    if (renderContext.highlightIaq && record.hasEnvironmentMetrics && record.environmentMetrics.iaq > 0 &&
        record.environmentMetrics.iaq < 1000) {
        const uint32_t iaq = record.environmentMetrics.iaq;
        const auto [iaqTextColor, iaqBgColor] = NodeListRowPresentation::iaqColors(iaq);
        lv_obj_set_style_border_color(row.panel, iaqBgColor, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(row.panel, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(row.lblTm2, iaqTextColor, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(row.lblTm2, iaqBgColor, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(row.lblTm2, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
        highlighted = true;
    } else {
        lv_obj_set_style_bg_opa(row.lblTm2, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_remove_local_style_prop(row.lblTm2, LV_STYLE_TEXT_COLOR, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_remove_local_style_prop(row.lblTm2, LV_STYLE_BG_COLOR, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    // Name highlight matches the rendered short-name text, including the id
    // fallback and the distance line.
    const char *name = renderContext.highlightName;
    if (name[0] != '\0' && (NodeListRowPresentation::containsCaseInsensitive(record.user.long_name, name) ||
                            NodeListRowPresentation::containsCaseInsensitive(row.shortText, name))) {
        lv_obj_set_style_border_color(row.panel, highlightMesh, LV_PART_MAIN | LV_STATE_DEFAULT);
        highlighted = true;
    }
    if (!highlighted) {
        lv_obj_set_style_border_color(row.panel, highlightMidGray, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(row.panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

void VirtualNodeList::refreshVisibleRows(bool force, bool rebind, bool reorder)
{
    if (!parentPanel || !currentIndex || !currentStore) {
        return;
    }

    const auto &ids = currentIndex->ids();
    if (ids.empty()) {
        for (auto &row : rowPool) {
            lv_obj_add_flag(row.panel, LV_OBJ_FLAG_HIDDEN);
            if (reorder && attachedGroup && row.btn && lv_obj_get_group(row.btn) == attachedGroup) {
                lv_group_remove_obj(row.btn);
            }
            clearRowBinding(row);
        }
        firstRenderedIndex = std::numeric_limits<size_t>::max();
        rowOrderDirty = false;
        return;
    }

    int32_t scrollY = lv_obj_get_scroll_y(parentPanel);
    if (scrollY < 0) {
        scrollY = 0;
    }

    size_t firstIdx = firstVisibleIndex(scrollY);

    // Buffer 1 row of overscan above if possible
    if (firstIdx > 0) {
        firstIdx--;
    }
    if (!ids.empty() && firstIdx >= ids.size()) {
        firstIdx = ids.size() - 1;
    }
    const size_t previousFirstRenderedIndex = firstRenderedIndex;
    if (!force && previousFirstRenderedIndex == firstIdx) {
        if (!rebind && !(reorder && rowOrderDirty)) {
            return;
        }
        if (!rowOrderDirty) {
            reorder = false;
        }
    } else if (!reorder) {
        rowOrderDirty = true;
    }
    firstRenderedIndex = firstIdx;

    constexpr size_t unusedRow = std::numeric_limits<size_t>::max();
    const size_t visibleCount = std::min(rowPool.size(), ids.size() - firstIdx);
    std::array<size_t, MAX_POOL_SIZE> assignedRows{};
    std::array<bool, MAX_POOL_SIZE> usedRows{};
    assignedRows.fill(unusedRow);

    for (size_t slot = 0; slot < visibleCount; ++slot) {
        const NodeId id = ids[firstIdx + slot];
        for (size_t rowIndex = 0; rowIndex < rowPool.size(); ++rowIndex) {
            if (!usedRows[rowIndex] && rowPool[rowIndex].boundId == id) {
                assignedRows[slot] = rowIndex;
                usedRows[rowIndex] = true;
                break;
            }
        }
    }

    for (size_t slot = 0; slot < visibleCount; ++slot) {
        if (assignedRows[slot] != unusedRow) {
            continue;
        }
        for (size_t rowIndex = 0; rowIndex < rowPool.size(); ++rowIndex) {
            if (!usedRows[rowIndex]) {
                assignedRows[slot] = rowIndex;
                usedRows[rowIndex] = true;
                break;
            }
        }
    }

    for (size_t slot = 0; slot < visibleCount; ++slot) {
        const size_t rowIndex = assignedRows[slot];
        if (rowIndex == unusedRow) {
            continue;
        }
        const size_t nodeIdx = firstIdx + slot;
        const NodeId id = ids[nodeIdx];
        auto &row = rowPool[rowIndex];
        const auto *rec = currentStore->find(id);
        if (rec) {
            if (rebind || row.boundId != id) {
                bindRow(row, *rec, rowShowsExpandedDetails(id));
            }
            const int32_t height = rowHeight(id);
            const int32_t y = rowY(nodeIdx);
            if (row.renderedHeight != height) {
                lv_obj_set_height(row.panel, height);
                row.renderedHeight = height;
            }
            if (row.renderedY != y) {
                lv_obj_set_y(row.panel, y);
                row.renderedY = y;
            }
            lv_obj_remove_flag(row.panel, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(row.panel, LV_OBJ_FLAG_HIDDEN);
            if (attachedGroup && row.btn && lv_obj_get_group(row.btn) == attachedGroup) {
                lv_group_remove_obj(row.btn);
            }
            clearRowBinding(row);
            usedRows[rowIndex] = false;
            assignedRows[slot] = unusedRow;
        }
    }

    for (size_t rowIndex = 0; rowIndex < rowPool.size(); ++rowIndex) {
        if (usedRows[rowIndex]) {
            continue;
        }
        auto &row = rowPool[rowIndex];
        lv_obj_add_flag(row.panel, LV_OBJ_FLAG_HIDDEN);
        if (reorder && attachedGroup && row.btn && lv_obj_get_group(row.btn) == attachedGroup) {
            lv_group_remove_obj(row.btn);
        }
        clearRowBinding(row);
    }

    if (!reorder) {
        return;
    }
    rowOrderDirty = false;

    if (attachedGroup) {
        for (size_t rowIndex = 0; rowIndex < rowPool.size(); ++rowIndex) {
            auto &row = rowPool[rowIndex];
            if (usedRows[rowIndex]) {
                if (row.btn && lv_obj_get_group(row.btn) != attachedGroup) {
                    lv_group_add_obj(attachedGroup, row.btn);
                }
            } else if (row.btn && lv_obj_get_group(row.btn) == attachedGroup) {
                lv_group_remove_obj(row.btn);
            }
        }
    }

    const bool allRowsAssigned = std::all_of(assignedRows.begin(), assignedRows.begin() + visibleCount,
                                             [unusedRow](size_t rowIndex) { return rowIndex != unusedRow; });
    const bool canRotateRows =
        allRowsAssigned && !force && !rebind && previousFirstRenderedIndex != std::numeric_limits<size_t>::max();
    const size_t forwardRows = canRotateRows && firstIdx > previousFirstRenderedIndex ? firstIdx - previousFirstRenderedIndex : 0;
    const size_t backwardRows =
        canRotateRows && previousFirstRenderedIndex > firstIdx ? previousFirstRenderedIndex - firstIdx : 0;

    if (forwardRows > 0 && forwardRows < visibleCount) {
        for (size_t slot = visibleCount - forwardRows; slot < visibleCount; ++slot) {
            const size_t rowIndex = assignedRows[slot];
            lv_obj_move_to_index(rowPool[rowIndex].panel, static_cast<int32_t>(visibleCount));
#ifdef UNIT_TEST
            ++panelOrderMoveCount;
#endif
            moveGroupButtonToTail(attachedGroup, rowPool[rowIndex].btn);
#ifdef UNIT_TEST
            ++groupOrderMoveCount;
#endif
        }
    } else if (backwardRows > 0 && backwardRows < visibleCount) {
        for (size_t slot = backwardRows; slot > 0; --slot) {
            const size_t rowIndex = assignedRows[slot - 1];
            lv_obj_move_to_index(rowPool[rowIndex].panel, 1);
#ifdef UNIT_TEST
            ++panelOrderMoveCount;
#endif
            moveGroupButtonToHead(attachedGroup, rowPool[rowIndex].btn);
#ifdef UNIT_TEST
            ++groupOrderMoveCount;
#endif
        }
    } else {
        if (spacer) {
            lv_obj_move_to_index(spacer, 0);
#ifdef UNIT_TEST
            ++panelOrderMoveCount;
#endif
        }
        for (size_t slot = 0; slot < visibleCount; ++slot) {
            const size_t rowIndex = assignedRows[slot];
            if (rowIndex != unusedRow && rowPool[rowIndex].panel) {
                lv_obj_move_to_index(rowPool[rowIndex].panel, static_cast<int32_t>(slot + 1));
#ifdef UNIT_TEST
                ++panelOrderMoveCount;
#endif
            }
        }
        for (size_t slot = visibleCount; slot > 0; --slot) {
            const size_t rowIndex = assignedRows[slot - 1];
            if (rowIndex != unusedRow && rowPool[rowIndex].btn) {
                moveGroupButtonToHead(attachedGroup, rowPool[rowIndex].btn);
#ifdef UNIT_TEST
                ++groupOrderMoveCount;
#endif
            }
        }
    }
#ifdef UNIT_TEST
    lv_obj_update_layout(parentPanel);
#endif
}

bool VirtualNodeList::refreshNode(NodeId id, uint32_t now, const NodeListRenderContext &context)
{
    if (!id || !currentStore) {
        return false;
    }

    currentTime = now != 0 ? now : static_cast<uint32_t>(std::time(nullptr));
    const bool contextChanged = !sameRenderContext(renderContext, context);

    const auto *rec = currentStore->find(id);
    if (!rec) {
        return false;
    }
    if (contextChanged) {
        renderContext = context;
        refreshVisibleRows(true, true);
        return true;
    }

    for (size_t index = 0; index < rowPool.size(); ++index) {
        auto &row = rowPool[index];
        if (row.boundId == id && row.panel && !lv_obj_has_flag(row.panel, LV_OBJ_FLAG_HIDDEN)) {
            bindRow(row, *rec, rowShowsExpandedDetails(id));
            const int32_t height = rowHeight(id);
            const auto position = currentIndex ? currentIndex->indexOf(id) : std::nullopt;
            const int32_t y = position.has_value() ? rowY(position.value()) : lv_obj_get_y(row.panel);
            if (row.renderedHeight != height) {
                lv_obj_set_height(row.panel, height);
                row.renderedHeight = height;
            }
            if (row.renderedY != y) {
                lv_obj_set_y(row.panel, y);
                row.renderedY = y;
            }
            return true;
        }
    }

    return false;
}

void VirtualNodeList::scrollEventCallback(lv_event_t *e)
{
    auto *self = static_cast<VirtualNodeList *>(lv_event_get_user_data(e));
    if (self) {
        self->refreshVisibleRows(false, false, lv_event_get_code(e) == LV_EVENT_SCROLL_END);
    }
}

void VirtualNodeList::rowClickCallback(lv_event_t *e)
{
    auto *self = static_cast<VirtualNodeList *>(lv_event_get_user_data(e));
    auto *btn = static_cast<lv_obj_t *>(lv_event_get_target(e));
    if (!self || !btn) {
        return;
    }

    ReusableRow *row = nullptr;
    for (auto &candidate : self->rowPool) {
        if (candidate.btn == btn) {
            row = &candidate;
            break;
        }
    }
    if (!row) {
        return;
    }

    const lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        row->pressedId = row->boundId;
    } else if (code == LV_EVENT_PRESS_LOST || code == LV_EVENT_CANCEL) {
        row->pressedId = 0;
    } else if (code == LV_EVENT_CLICKED) {
        const NodeId id = row->pressedId ? row->pressedId : row->boundId;
        row->pressedId = 0;
        if (id != 0 && !self->expansionAnimating()) {
            self->actionSink.nodeClicked(id);
        }
    } else if (code == LV_EVENT_LONG_PRESSED) {
        const NodeId id = row->pressedId ? row->pressedId : row->boundId;
        row->pressedId = 0;
        if (id != 0) {
            self->actionSink.nodeLongPressed(id);
        }
    } else if (code == LV_EVENT_FOCUSED) {
        const NodeId id = row->boundId;
        if (id != 0) {
            self->noteFocusedButton(btn);
        }
    }
}

#ifdef UNIT_TEST
bool VirtualNodeList::detailLabelsHiddenForTesting(NodeId id) const
{
    for (const auto &row : rowPool) {
        if (row.boundId == id) {
            return lv_obj_has_flag(row.lblPos1, LV_OBJ_FLAG_HIDDEN) && lv_obj_has_flag(row.lblPos2, LV_OBJ_FLAG_HIDDEN) &&
                   lv_obj_has_flag(row.lblTm1, LV_OBJ_FLAG_HIDDEN) && lv_obj_has_flag(row.lblTm2, LV_OBJ_FLAG_HIDDEN);
        }
    }
    return false;
}

bool VirtualNodeList::positionLabelClickableForTesting(NodeId id) const
{
    for (const auto &row : rowPool) {
        if (row.boundId == id) {
            return lv_obj_has_flag(row.lblPos1, LV_OBJ_FLAG_CLICKABLE);
        }
    }
    return false;
}

bool VirtualNodeList::longLabelsScrollForTesting(NodeId id) const
{
    for (const auto &row : rowPool) {
        if (row.boundId == id) {
            return lv_label_get_long_mode(row.lblLong) == LV_LABEL_LONG_SCROLL &&
                   lv_label_get_long_mode(row.lblPos1) == LV_LABEL_LONG_CLIP &&
                   lv_label_get_long_mode(row.lblPos2) == LV_LABEL_LONG_SCROLL;
        }
    }
    return false;
}

const char *VirtualNodeList::shortTextForTesting(NodeId id) const
{
    for (const auto &row : rowPool) {
        if (row.boundId == id && row.panel && !lv_obj_has_flag(row.panel, LV_OBJ_FLAG_HIDDEN)) {
            return row.shortText;
        }
    }
    return "";
}
#endif

void VirtualNodeList::rowPositionCallback(lv_event_t *e)
{
    auto *self = static_cast<VirtualNodeList *>(lv_event_get_user_data(e));
    auto *label = static_cast<lv_obj_t *>(lv_event_get_target(e));
    if (!self || !label) {
        return;
    }

    const auto id = static_cast<NodeId>(reinterpret_cast<uintptr_t>(lv_obj_get_user_data(label)));
    if (id != 0 && lv_event_get_code(e) == LV_EVENT_CLICKED) {
        self->actionSink.nodePositionClicked(id);
    }
}

void VirtualNodeList::groupEdgeCallback(lv_group_t *group, bool forward)
{
    (void)group;
    if (activeGroupNavigationList) {
        activeGroupNavigationList->handleGroupEdge(forward);
    }
}

void VirtualNodeList::expansionAnimationCallback(void *var, int32_t progress)
{
    auto *self = static_cast<VirtualNodeList *>(var);
    if (self) {
        self->updateExpansionAnimation(progress);
    }
}

void VirtualNodeList::expansionAnimationFinished(lv_anim_t *animation)
{
    auto *self = static_cast<VirtualNodeList *>(animation->var);
    if (self) {
        self->finishExpansionAnimation();
    }
}
