#pragma once

#include "graphics/common/NodeStore.h"
#include "graphics/common/VisibleNodeIndex.h"
#include "lvgl.h"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

class NodeListActionSink
{
  public:
    virtual void nodeClicked(NodeId id) = 0;
    virtual void nodeLongPressed(NodeId id) = 0;
    virtual void nodeFocusBoundary(bool forward) {}
    virtual void nodePositionClicked(NodeId id) {}
    virtual ~NodeListActionSink() = default;
};

struct NodeListRenderContext {
    NodeId ownNode = 0;
    bool hasOwnPosition = false;
    int32_t ownLatitude = 0;
    int32_t ownLongitude = 0;
    bool metricUnits = true;
    bool highlightActiveChat = false;
    bool highlightPosition = false;
    bool highlightTelemetry = false;
    bool highlightIaq = false;
    char highlightName[64]{};
};

struct ReusableRow {
    lv_obj_t *panel = nullptr;
    lv_obj_t *img = nullptr;
    lv_obj_t *btn = nullptr;
    lv_obj_t *lblLong = nullptr;
    lv_obj_t *lblShort = nullptr;
    lv_obj_t *lblBat = nullptr;
    lv_obj_t *lblLh = nullptr;
    lv_obj_t *lblSig = nullptr;
    lv_obj_t *lblPos1 = nullptr;
    lv_obj_t *lblPos2 = nullptr;
    lv_obj_t *lblTm1 = nullptr;
    lv_obj_t *lblTm2 = nullptr;
    char shortText[32]{};
    char longText[40]{};
    char batteryText[32]{};
    char lastHeardText[32]{};
    char signalText[32]{};
    char positionText[48]{};
    char position2Text[32]{};
    char telemetry1Text[64]{};
    char telemetry2Text[48]{};
    NodeId boundId = 0;
    NodeId pressedId = 0;
    int32_t renderedY = std::numeric_limits<int32_t>::min();
    int32_t renderedHeight = std::numeric_limits<int32_t>::min();

    // Float-heavy detail strings are reformatted only when their source values
    // or unit mode change; recycled rows start invalidated.
    bool detailKeysValid = false;
    bool detailMetricUnits = true;
    bool detailHasBattery = false;
    bool detailHasEnvironment = false;
    uint32_t detailBatteryLevel = 0;
    float detailVoltage = 0;
    int32_t detailLatitude = 0;
    int32_t detailLongitude = 0;
    int32_t detailAltitude = 0;
    NodeEnvironmentMetrics detailEnvironment{};
    // Short-name label memoization (fallback + distance line)
    NodeId detailShortId = 0;
    NodeId detailOwnNode = 0;
    char detailShortName[5]{};
    bool detailHasOwnPosition = false;
    int32_t detailOwnLatitude = 0;
    int32_t detailOwnLongitude = 0;
};

class VirtualNodeList
{
  public:
    static constexpr size_t MAX_POOL_SIZE = 9;
    static constexpr int32_t COLLAPSED_ROW_HEIGHT = 53;
    static constexpr int32_t EXPANDED_ROW_HEIGHT = 83;
    static constexpr int32_t ROW_GAP = 5;

    VirtualNodeList(lv_obj_t *parent, NodeListActionSink &sink);
    ~VirtualNodeList();

    void sync(const NodeStore &store, const VisibleNodeIndex &index, NodeId expanded = 0, uint32_t currentTime = 0,
              const NodeListRenderContext &context = {}, bool forceRebind = false);
    void setExpanded(NodeId id);
    bool expansionAnimating() const { return expansionAnimationActive; }
    void scrollTo(NodeId id, lv_anim_enable_t anim = LV_ANIM_OFF);
    void focus(NodeId id);
    lv_group_t *navigationGroup() const { return attachedGroup; }
#ifdef UNIT_TEST
    int32_t rowHeightForTesting(NodeId id) const { return rowHeight(id); }
    int32_t rowYForTesting(size_t index) const { return rowY(index); }
    int32_t scrollYForNodeForTesting(NodeId id) const { return scrollYForNode(id); }
    NodeId focusedIdForTesting() const { return currentFocusedId(); }
    bool detailLabelsHiddenForTesting(NodeId id) const;
    bool positionLabelClickableForTesting(NodeId id) const;
    bool longLabelsScrollForTesting(NodeId id) const;
    const char *shortTextForTesting(NodeId id) const;
    size_t rowPoolSizeForTesting() const { return rowPool.size(); }
    void setExpansionProgressForTesting(int32_t progress) { updateExpansionAnimation(progress); }
    void finishExpansionForTesting()
    {
        lv_anim_delete(this, expansionAnimationCallback);
        finishExpansionAnimation();
        if (parentPanel) {
            lv_obj_update_layout(parentPanel);
        }
    }
    bool expansionAnimationRegisteredForTesting() const
    {
        return lv_anim_get(const_cast<VirtualNodeList *>(this), expansionAnimationCallback) != nullptr;
    }
    uint32_t expandedIndexScanCountForTesting() const { return expandedIndexScanCount; }
    uint32_t panelOrderMoveCountForTesting() const { return panelOrderMoveCount; }
    uint32_t groupOrderMoveCountForTesting() const { return groupOrderMoveCount; }
#endif

    void refreshVisibleRows(bool force = false, bool rebind = false, bool reorder = true);
    bool refreshNode(NodeId id, uint32_t currentTime, const NodeListRenderContext &context);

  private:
    struct ScrollAnchor {
        NodeId id = 0;
        int32_t offset = 0;
    };

    void createRowPool();
    void updateVirtualContentHeight();
    void updateExpandedIndexCache();
    ScrollAnchor captureScrollAnchor() const;
    void restoreScrollAnchor(const ScrollAnchor &anchor);
    void bindRow(ReusableRow &row, const NodeRecord &record, bool isExpanded);
    void applyHighlight(ReusableRow &row, const NodeRecord &record);
    bool visibleLastHeardLabelsNeedRefresh(const NodeStore &store, uint32_t nextTime, const NodeListRenderContext &context) const;
    int32_t rowHeight(NodeId id) const;
    int32_t rowY(size_t index) const;
    size_t firstVisibleIndex(int32_t scrollY) const;
    bool rowShowsExpandedDetails(NodeId id) const;
    int32_t viewportHeight() const;
    int32_t scrollYForNode(NodeId id) const;
    NodeId currentFocusedId() const;
    void restoreFocus(NodeId focusedId, size_t fallbackIndex);
    void requestExpanded(NodeId id);
    void updateExpansionAnimation(int32_t progress);
    void finishExpansionAnimation();
    void attachGroupNavigation();
    void detachGroupNavigation();
    void handleGroupEdge(bool forward);
    void noteFocusedButton(lv_obj_t *button);
    bool focusAdjacent(NodeId id, int direction);
    size_t poolIndexForButton(lv_obj_t *button) const;

    static void scrollEventCallback(lv_event_t *e);
    static void rowClickCallback(lv_event_t *e);
    static void rowPositionCallback(lv_event_t *e);
    static void groupEdgeCallback(lv_group_t *group, bool forward);
    static void expansionAnimationCallback(void *var, int32_t progress);
    static void expansionAnimationFinished(lv_anim_t *animation);

    lv_obj_t *parentPanel = nullptr;
    lv_obj_t *spacer = nullptr;
    NodeListActionSink &actionSink;

    const NodeStore *currentStore = nullptr;
    const VisibleNodeIndex *currentIndex = nullptr;
    NodeId expandedId = 0;
    NodeId previousExpandedId = 0;
    NodeId pendingExpandedId = 0;
    size_t expandedIndex = std::numeric_limits<size_t>::max();
    size_t previousExpandedIndex = std::numeric_limits<size_t>::max();
    uint32_t currentTime = 0;
    NodeListRenderContext renderContext{};

    std::vector<ReusableRow> rowPool;
    size_t firstRenderedIndex = std::numeric_limits<size_t>::max();
    uint32_t lastSyncedIndexGeneration = 0;
    bool expansionCacheValid = false;
    const VisibleNodeIndex *expansionCacheIndex = nullptr;
    uint32_t expansionCacheGeneration = 0;
    NodeId expansionCacheExpandedId = 0;
    NodeId expansionCachePreviousId = 0;
    lv_group_t *attachedGroup = nullptr;
    NodeId lastFocusedId = 0;
    size_t lastFocusedIndex = std::numeric_limits<size_t>::max();
    bool rowOrderDirty = false;
    bool expansionAnimationActive = false;
    bool hasPendingExpandedId = false;
    int32_t expansionProgress = 100;
#ifdef UNIT_TEST
    mutable uint32_t expandedIndexScanCount = 0;
    uint32_t panelOrderMoveCount = 0;
    uint32_t groupOrderMoveCount = 0;
#endif
};
