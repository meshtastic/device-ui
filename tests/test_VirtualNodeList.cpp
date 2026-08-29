#include "graphics/common/NodeListRowPresentation.h"
#include "graphics/view/TFT/VirtualNodeList.h"

#include <doctest/doctest.h>

#include <cstdio>
#include <cstring>

namespace
{
struct RecordingSink : NodeListActionSink {
    NodeId clicked = 0;
    NodeId longPressed = 0;
    NodeId positionClicked = 0;
    bool hitBoundary = false;

    void nodeClicked(NodeId id) override { clicked = id; }
    void nodeLongPressed(NodeId id) override { longPressed = id; }
    void nodeFocusBoundary(bool) override { hitBoundary = true; }
    void nodePositionClicked(NodeId id) override { positionClicked = id; }
};

void ensureLvgl()
{
    static bool initialized = false;
    if (initialized) {
        return;
    }
    lv_init();
    lv_display_create(320, 240);
    initialized = true;
}

meshtastic_User makeUser(NodeId id, const char *name)
{
    meshtastic_User user = meshtastic_User_init_default;
    std::snprintf(user.short_name, sizeof(user.short_name), "%04x", static_cast<unsigned int>(id & 0xffff));
    std::snprintf(user.long_name, sizeof(user.long_name), "%s", name);
    user.role = meshtastic_Config_DeviceConfig_Role_CLIENT;
    return user;
}

lv_obj_t *makeParent(lv_obj_t *screen)
{
    auto *parent = lv_obj_create(screen);
    lv_obj_set_size(parent, 180, VirtualNodeList::COLLAPSED_ROW_HEIGHT * 2 + VirtualNodeList::ROW_GAP);
    lv_obj_set_style_pad_all(parent, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_scroll_dir(parent, LV_DIR_VER);
    return parent;
}

lv_obj_t *makeParentWithHeight(lv_obj_t *screen, int32_t height)
{
    auto *parent = lv_obj_create(screen);
    lv_obj_set_size(parent, 180, height);
    lv_obj_set_style_pad_all(parent, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_scroll_dir(parent, LV_DIR_VER);
    return parent;
}

void addNode(NodeStore &store, NodeId id, uint32_t lastHeard, const char *name)
{
    store.upsertUser(id, 0, lastHeard, makeUser(id, name), false);
}
} // namespace

TEST_CASE("node row formats imperial mile distance with one decimal before rounding")
{
    char text[32]{};

    NodeListRowPresentation::formatShortNameWithDistance(text, sizeof(text), nullptr, 0x1111, true, 0, 0, 179695, 0, false);

    CHECK(std::strstr(text, "1.2 mi") != nullptr);
}

TEST_CASE("virtual node list allocates enough rows for viewport pitch and overscan")
{
    ensureLvgl();
    auto *screen = lv_obj_create(nullptr);
    auto *parent = makeParentWithHeight(screen, VirtualNodeList::COLLAPSED_ROW_HEIGHT * 2 + VirtualNodeList::ROW_GAP * 2 + 1);
    RecordingSink sink;
    {
        VirtualNodeList list(parent, sink);

        CHECK(list.rowPoolSizeForTesting() == 4);
    }
    lv_obj_delete(screen);
}

TEST_CASE("virtual node list keeps focused node identity across reorder")
{
    ensureLvgl();
    auto *screen = lv_obj_create(nullptr);
    auto *parent = makeParent(screen);
    RecordingSink sink;
    NodeStore store;
    VisibleNodeIndex index;
    NodeListFilter filter;
    filter.curTime = 1000;

    addNode(store, 0x1111, 300, "Alpha");
    addNode(store, 0x2222, 200, "Bravo");
    addNode(store, 0x3333, 100, "Charlie");
    index.rebuild(store, filter, 0);
    {
        VirtualNodeList list(parent, sink);
        list.sync(store, index, 0, filter.curTime);
        list.focus(0x2222);
        REQUIRE(list.focusedIdForTesting() == 0x2222);

        store.updateLastHeard(0x3333, 400);
        index.rebuild(store, filter, 0);
        list.sync(store, index, 0, filter.curTime);

        CHECK(list.focusedIdForTesting() == 0x2222);
    }
    lv_obj_delete(screen);
}

TEST_CASE("virtual node list moves removed focus to nearest visible row")
{
    ensureLvgl();
    auto *screen = lv_obj_create(nullptr);
    auto *parent = makeParent(screen);
    RecordingSink sink;
    NodeStore store;
    VisibleNodeIndex index;
    NodeListFilter filter;
    filter.curTime = 1000;

    addNode(store, 0x1111, 300, "Alpha");
    addNode(store, 0x2222, 200, "Bravo");
    addNode(store, 0x3333, 100, "Charlie");
    index.rebuild(store, filter, 0);
    {
        VirtualNodeList list(parent, sink);
        list.sync(store, index, 0, filter.curTime);
        list.focus(0x2222);

        store.remove(0x2222);
        index.rebuild(store, filter, 0);
        list.sync(store, index, 0, filter.curTime);

        CHECK(list.focusedIdForTesting() == 0x3333);
    }
    lv_obj_delete(screen);
}

TEST_CASE("virtual node list uses minimal focus scroll and hides collapsed details")
{
    ensureLvgl();
    auto *screen = lv_obj_create(nullptr);
    auto *parent = makeParent(screen);
    RecordingSink sink;
    NodeStore store;
    VisibleNodeIndex index;
    NodeListFilter filter;
    filter.curTime = 1000;

    addNode(store, 0x1111, 400, "Alpha with a long visible name");
    addNode(store, 0x2222, 300, "Bravo");
    addNode(store, 0x3333, 200, "Charlie");
    addNode(store, 0x4444, 100, "Delta");
    store.updatePosition(0x1111, {true, 377749000, -1224194000, 42, 5, 0});
    meshtastic_EnvironmentMetrics env = meshtastic_EnvironmentMetrics_init_default;
    env.temperature = 21.5f;
    env.barometric_pressure = 1013.2f;
    env.iaq = 80;
    store.updateEnvironmentMetrics(0x1111, env);
    index.rebuild(store, filter, 0);
    {
        VirtualNodeList list(parent, sink);
        list.sync(store, index, 0, filter.curTime);

        CHECK(list.detailLabelsHiddenForTesting(0x1111));
        CHECK_FALSE(list.positionLabelClickableForTesting(0x1111));
        CHECK(list.longLabelsScrollForTesting(0x1111));
        CHECK(list.scrollYForNodeForTesting(0x1111) == 0);
        CHECK(list.scrollYForNodeForTesting(0x3333) > 0);
        CHECK(list.scrollYForNodeForTesting(0x3333) < list.rowYForTesting(2));

        list.setExpanded(0x1111);
        list.finishExpansionForTesting();
        CHECK_FALSE(list.detailLabelsHiddenForTesting(0x1111));
        CHECK(list.positionLabelClickableForTesting(0x1111));
    }

    lv_obj_delete(screen);
}

TEST_CASE("virtual node list repairs navigation order after deferred boundary scroll")
{
    ensureLvgl();
    auto *screen = lv_obj_create(nullptr);
    auto *parent = makeParent(screen);
    RecordingSink sink;
    NodeStore store;
    VisibleNodeIndex index;
    NodeListFilter filter;
    filter.curTime = 1000;

    addNode(store, 0x1111, 500, "Alpha");
    addNode(store, 0x2222, 400, "Bravo");
    addNode(store, 0x3333, 300, "Charlie");
    addNode(store, 0x4444, 200, "Delta");
    addNode(store, 0x5555, 100, "Echo");
    index.rebuild(store, filter, 0);
    {
        VirtualNodeList list(parent, sink);
        list.sync(store, index, 0, filter.curTime);
        const uint32_t baselineGroupMoves = list.groupOrderMoveCountForTesting();

        lv_obj_scroll_to_y(parent, list.rowYForTesting(2), LV_ANIM_OFF);
        list.refreshVisibleRows(false, false, false);
        CHECK(list.groupOrderMoveCountForTesting() == baselineGroupMoves);

        list.refreshVisibleRows(false, false, true);
        CHECK(list.groupOrderMoveCountForTesting() > baselineGroupMoves);

        const uint32_t repairedGroupMoves = list.groupOrderMoveCountForTesting();
        lv_obj_scroll_to_y(parent, list.rowYForTesting(2) + 1, LV_ANIM_OFF);
        list.refreshVisibleRows(false, false, true);
        CHECK(list.groupOrderMoveCountForTesting() == repairedGroupMoves);
    }

    lv_obj_delete(screen);
}

TEST_CASE("virtual node list removes missing records from navigation group during stale index refresh")
{
    ensureLvgl();
    auto *screen = lv_obj_create(nullptr);
    auto *parent = makeParent(screen);
    RecordingSink sink;
    NodeStore store;
    VisibleNodeIndex index;
    NodeListFilter filter;
    filter.curTime = 1000;

    addNode(store, 0x1111, 300, "Alpha");
    addNode(store, 0x2222, 200, "Bravo");
    addNode(store, 0x3333, 100, "Charlie");
    index.rebuild(store, filter, 0);
    {
        VirtualNodeList list(parent, sink);
        list.sync(store, index, 0, filter.curTime);
        REQUIRE(lv_group_get_obj_count(list.navigationGroup()) == 3);

        store.remove(0x2222);
        list.refreshVisibleRows(true, false, true);

        CHECK(lv_group_get_obj_count(list.navigationGroup()) == 2);
    }

    lv_obj_delete(screen);
}

TEST_CASE("virtual node list rebinds all visible rows when refresh node changes render context")
{
    ensureLvgl();
    auto *screen = lv_obj_create(nullptr);
    auto *parent = makeParent(screen);
    RecordingSink sink;
    NodeStore store;
    VisibleNodeIndex index;
    NodeListFilter filter;
    filter.curTime = 1000;

    addNode(store, 0x1111, 300, "Alpha");
    addNode(store, 0x2222, 200, "Bravo");
    store.updatePosition(0x1111, {true, 179695, 0, 0, 0, 0});
    store.updatePosition(0x2222, {true, 269542, 0, 0, 0, 0});
    index.rebuild(store, filter, 0);
    {
        NodeListRenderContext context;
        context.hasOwnPosition = true;
        context.metricUnits = true;

        VirtualNodeList list(parent, sink);
        list.sync(store, index, 0, filter.curTime, context);
        REQUIRE(std::strstr(list.shortTextForTesting(0x1111), "km") != nullptr);
        REQUIRE(std::strstr(list.shortTextForTesting(0x2222), "km") != nullptr);

        context.metricUnits = false;
        REQUIRE(list.refreshNode(0x1111, filter.curTime, context));

        CHECK(std::strstr(list.shortTextForTesting(0x1111), "mi") != nullptr);
        CHECK(std::strstr(list.shortTextForTesting(0x2222), "mi") != nullptr);
    }

    lv_obj_delete(screen);
}
