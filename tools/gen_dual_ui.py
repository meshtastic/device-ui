#!/usr/bin/env python3
"""Generate the dual-layout glue used by MUI_RUNTIME_ROTATION builds.

Both generated UI trees are linked into one binary so the screen rotation
becomes a runtime setting. The primary tree (the build's VIEW_*) keeps its
symbol names; the secondary tree is compiled with its colliding globals renamed
through a generated forced include, and a bridge republishes its widget
pointers into the symbols the C++ view addresses.

Usage:
  python3 tools/gen_dual_ui.py [primary_view]   regenerate
  python3 tools/gen_dual_ui.py --check [view]   verify the committed output
"""

import hashlib
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
OUT = os.path.join(ROOT, "tools", "generated", "dual")
VIEWS = ("ui_320x240", "ui_240x320")
DEFAULT_PRIMARY = "ui_320x240"
PREFIX = "MUI2_"

# Secondary-tree assets the primary tree does not provide and whose image table
# actually references them. Compiled once, never renamed.
SECONDARY_ONLY_ASSETS = (
    "ui_image_top_message_node_images.c",
    "ui_image_home_bell_slash_image.c",
)

# Shared asset definitions, C++-implemented actions, and type/tag tokens that
# are not linker symbols.
NEVER_RENAME = re.compile(
    r"^(img_|ui_font_|action_|objects_t|_objects_t|ScreensEnum|SCREEN_ID_|tick_screen_func_t$)"
)


def read(path):
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        return f.read()


def core_sources():
    """Every non-asset translation unit, derived from the trees rather than
    hardcoded: a newly generated one must not silently enter only one tree.

    Assets are recognised by the ui_image_/ui_font_ filename prefixes, which is
    how the UI generator names them. A future asset under a different prefix
    would be treated as a core source and compiled into both trees, and its
    definition would collide because NEVER_RENAME excludes those symbols from
    the rename list. The symmetry assert below cannot see that, so the
    prefixes have to stay in sync with the UI generator."""
    per_tree = {}
    for view in VIEWS:
        d = os.path.join(ROOT, "generated", view)
        per_tree[view] = sorted(
            n
            for n in os.listdir(d)
            if n.endswith(".c") and not n.startswith(("ui_image_", "ui_font_"))
        )
    a, b = (per_tree[v] for v in VIEWS)
    if a != b:
        sys.exit(
            "ERROR: generated core source sets diverged.\n  %s: %s\n  %s: %s"
            % (VIEWS[0], a, VIEWS[1], b)
        )
    return a


def objects_members(tree):
    src = read(os.path.join(ROOT, "generated", tree, "screens.h"))
    m = re.search(
        r"typedef\s+struct\s+_objects_t\s*\{(.*?)\}\s*objects_t\s*;", src, re.S
    )
    if not m:
        sys.exit("ERROR: objects_t definition not found in %s/screens.h" % tree)
    return re.findall(r"lv_obj_t\s*\*\s*([A-Za-z_][A-Za-z_0-9]*)\s*;", m.group(1))


def global_symbols(tree, sources):
    # Scan the sources, not the headers: styles.h declares only a subset of
    # styles.c's globals, so a header-driven rename list would be incomplete.
    syms = set()
    for src_name in sources:
        path = os.path.join(ROOT, "generated", tree, src_name)
        if not os.path.exists(path):
            continue
        src = read(path)
        src = re.sub(r"/\*.*?\*/", "", src, flags=re.S)
        src = re.sub(r"//[^\n]*", "", src)
        for m in re.finditer(
            r"^(?!static\b|typedef\b|extern\b|#|\}|\{)"
            r"(?:const\s+|unsigned\s+|signed\s+|struct\s+|enum\s+)*"
            r"[A-Za-z_][A-Za-z_0-9]*\s+\**\s*"
            r"([A-Za-z_][A-Za-z_0-9]*)\s*"
            r"(?:\([^;{]*\)\s*\{|\[[^\]]*\]\s*=|=|;)",
            src,
            re.M,
        ):
            syms.add(m.group(1))
    return {s for s in syms if not NEVER_RENAME.match(s)}


def asset_symbol(name):
    """Linker symbol an asset translation unit defines, or None."""
    if name.startswith("ui_image_"):
        return "img_" + name[len("ui_image_") : -len(".c")]
    if name.startswith("ui_font_"):
        return name[: -len(".c")]
    return None


def tree_sources(tree, name):
    path = os.path.join(ROOT, "generated", tree, name)
    return read(path) if os.path.exists(path) else ""


def asset_inventory(sources):
    """Shared assets (present in both trees) must be byte-identical, because
    only the primary copy is compiled. Assets unique to one tree have to be
    compiled additionally whenever that tree is the secondary one, so the
    inventory is computed for both directions."""
    h = hashlib.sha256()
    dirs = {v: os.path.join(ROOT, "generated", v) for v in VIEWS}
    names = set()
    for d in dirs.values():
        names |= {
            n
            for n in os.listdir(d)
            if n.startswith(("ui_image_", "ui_font_")) and n.endswith(".c")
        }

    shared, unique = [], {v: [] for v in VIEWS}
    for name in sorted(names):
        present = [v for v in VIEWS if os.path.exists(os.path.join(dirs[v], name))]
        if len(present) == len(VIEWS):
            a, b = (read(os.path.join(dirs[v], name)) for v in VIEWS)
            if a != b:
                sys.exit(
                    "ERROR: asset %s is no longer identical between trees; the shared-asset "
                    "deduplication would silently use the primary bytes" % name
                )
            shared.append(name)
            h.update(name.encode())
            h.update(hashlib.sha256(a.encode()).digest())
        else:
            unique[present[0]].append(name)

    # An unlisted unique asset that the tree's own sources reference would be a
    # missing definition once that tree is compiled as the secondary one.
    for view, extras in unique.items():
        body = "".join(tree_sources(view, s) for s in sources)
        for name in extras:
            if name in SECONDARY_ONLY_ASSETS:
                continue
            sym = asset_symbol(name)
            if sym and re.search(r"\b%s\b" % re.escape(sym), body):
                sys.exit(
                    "ERROR: %s-only asset %s is now referenced by that tree but is excluded "
                    "from the build; add it to SECONDARY_ONLY_ASSETS" % (view, name)
                )

    listed = {
        v: sorted(n for n in unique[v] if n in SECONDARY_ONLY_ASSETS) for v in VIEWS
    }
    for view in VIEWS:
        for name in listed[view]:
            h.update(("%s/%s" % (view, name)).encode())
            h.update(
                hashlib.sha256(read(os.path.join(dirs[view], name)).encode()).digest()
            )
    return h.hexdigest(), len(shared), listed


def build_artifacts():
    """Return {filename: content} for every generated file.

    The output is orientation-neutral: both trees expose the same member and
    symbol sets and the bridge copies field-wise in canonical order, so one set
    of artifacts serves either tree as the primary one."""
    first, second = VIEWS
    sources = core_sources()
    a_members, b_members = objects_members(first), objects_members(second)
    if set(a_members) != set(b_members):
        sys.exit(
            "ERROR: objects_t member sets diverged.\n  %s-only: %s\n  %s-only: %s"
            % (
                first,
                sorted(set(a_members) - set(b_members)),
                second,
                sorted(set(b_members) - set(a_members)),
            )
        )
    members = sorted(set(a_members))

    a_syms, b_syms = global_symbols(first, sources), global_symbols(second, sources)
    if a_syms != b_syms:
        sys.exit(
            "ERROR: global symbol sets are not symmetric; the rename list would be wrong.\n"
            "  %s-only: %s\n  %s-only: %s"
            % (first, sorted(a_syms - b_syms), second, sorted(b_syms - a_syms))
        )
    syms = sorted(a_syms)
    ui_ptrs = sorted(
        s
        for s in syms
        if s.startswith("ui_") and s not in ("ui_init", "ui_tick", "ui_init_boot")
    )
    assets_fp, shared_assets, unique_assets = asset_inventory(sources)
    fp = hashlib.sha256(
        (
            "|".join(sources)
            + "#"
            + "|".join(members)
            + "#"
            + "|".join(syms)
            + "#"
            + "|".join(ui_ptrs)
            + "#"
            + assets_fp
        ).encode()
    ).hexdigest()

    banner = "// GENERATED by tools/gen_dual_ui.py - do not edit.\n"
    out = {}

    # lvgl.h must come before the defines so the generic names (objects,
    # images, add_style, ...) cannot leak into the dependency headers.
    rename = [
        banner,
        "// Forced include for the secondary generated tree's sources only.\n",
        "#pragma once\n#include <lvgl.h>\n\n",
    ]
    rename += ["#define %s %s%s\n" % (s, PREFIX, s) for s in syms]
    out["ui_secondary_rename.h"] = "".join(rename)

    # The two objects_t structs share member names but not member order, so the
    # bridge copies field-wise by name. Neither side may include both trees'
    # screens.h (identical include guards), hence the flat intermediate arrays.
    sec = [
        banner,
        "// Secondary tree (renamed): flatten objects + ui_* pointers into\n",
        "// canonical (alphabetical) order.\n",
        '#include "screens.h"\n\n',
        "lv_obj_t *mui_secondary_objects_flat[%d];\n" % len(members),
        "lv_obj_t *mui_secondary_ui_flat[%d];\n\n" % len(ui_ptrs),
        "void mui_bridge_collect_secondary(void)\n{\n",
    ]
    sec += [
        "    mui_secondary_objects_flat[%d] = objects.%s;\n" % (i, n)
        for i, n in enumerate(members)
    ]
    sec += [
        "    mui_secondary_ui_flat[%d] = %s;\n" % (i, n) for i, n in enumerate(ui_ptrs)
    ]
    sec.append("}\n")
    out["bridge_secondary.c"] = "".join(sec)

    pri = [
        banner,
        "// Primary tree: publish the flattened secondary pointers into the\n",
        "// symbols the C++ view addresses.\n",
        '#include "screens.h"\n\n',
        "extern lv_obj_t *mui_secondary_objects_flat[%d];\n" % len(members),
        "extern lv_obj_t *mui_secondary_ui_flat[%d];\n" % len(ui_ptrs),
        "void mui_bridge_collect_secondary(void);\n\n",
        "void mui_bridge_publish_secondary(void)\n{\n",
        "    mui_bridge_collect_secondary();\n",
    ]
    pri += [
        "    objects.%s = mui_secondary_objects_flat[%d];\n" % (n, i)
        for i, n in enumerate(members)
    ]
    pri += [
        "    %s = mui_secondary_ui_flat[%d];\n" % (n, i) for i, n in enumerate(ui_ptrs)
    ]
    pri.append("}\n")
    out["bridge_primary.c"] = "".join(pri)

    # The secondary tree attaches its own renamed style objects, so theme
    # mutations and C++ add_style_* calls must reach the active tree or the
    # secondary layout renders unthemed. init_style_* is internal to styles.c
    # and never called from C++, so it is renamed but not routed.
    style_funcs = sorted(
        s for s in syms if s.startswith(("get_style_", "add_style_", "remove_style_"))
    )
    style_objs = sorted(s for s in syms if s.startswith("style_"))
    rt = [
        banner,
        "// Force-included into the C++ sources that reference generated styles.\n",
        '#pragma once\n#include "graphics/ScreenRotation.h"\n#include "styles.h"\n\n',
        'extern "C" {\n',
    ]
    for fn in style_funcs:
        if fn.startswith("get_style_"):
            rt.append("lv_style_t *%s%s(void);\n" % (PREFIX, fn))
        else:
            rt.append("void %s%s(lv_obj_t *obj);\n" % (PREFIX, fn))
    rt += ["extern lv_style_t %s%s;\n" % (PREFIX, ob) for ob in style_objs]
    rt.append("}\n\n")
    for fn in style_funcs:
        if fn.startswith("get_style_"):
            rt.append(
                "static inline lv_style_t *mui_route_%s(void)\n"
                "{ return ScreenRotation::usesSecondaryTree() ? %s%s() : %s(); }\n"
                "#define %s mui_route_%s\n" % (fn, PREFIX, fn, fn, fn, fn)
            )
        else:
            rt.append(
                "static inline void mui_route_%s(lv_obj_t *obj)\n"
                "{ if (ScreenRotation::usesSecondaryTree()) %s%s(obj); else %s(obj); }\n"
                "#define %s mui_route_%s\n" % (fn, PREFIX, fn, fn, fn, fn)
            )
    for ob in style_objs:
        rt.append(
            "static inline lv_style_t &mui_route_%s(void)\n"
            "{ return ScreenRotation::usesSecondaryTree() ? %s%s : %s; }\n"
            "#define %s (mui_route_%s())\n" % (ob, PREFIX, ob, ob, ob, ob)
        )
    out["style_routing.h"] = "".join(rt)

    # Single source of truth for the build: which sources each side compiles,
    # and which glue files may exist in this directory.
    glue_sources = sorted(n for n in out if n.endswith(".c"))
    manifest = [
        "members=%d\nrenamed_symbols=%d\nui_pointers=%d\nshared_assets=%d\nfingerprint=%s\n"
        % (len(members), len(syms), len(ui_ptrs), shared_assets, fp)
    ]
    manifest += ["core_source %s\n" % s for s in sources]
    manifest += ["glue_source %s\n" % s for s in glue_sources]
    # per-direction: extra sources to compile when <view> is the secondary tree
    for view in VIEWS:
        manifest += ["secondary_asset %s %s\n" % (view, n) for n in unique_assets[view]]
    manifest += ["sym %s\n" % s for s in syms]
    out["manifest.txt"] = "".join(manifest)

    return out, dict(
        members=len(members),
        syms=len(syms),
        ui_ptrs=len(ui_ptrs),
        style_funcs=len(style_funcs),
        style_objs=len(style_objs),
        fp=fp,
    )


def main():
    check_only = "--check" in sys.argv[1:]
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    # accepted for symmetry with the build invocation; the output is the same
    # for either primary, so it only has to be a supported view
    view = args[0] if args else DEFAULT_PRIMARY
    if view not in VIEWS:
        sys.exit(
            "ERROR: MUI_RUNTIME_ROTATION supports only %s; view %r is not one of them"
            % (" and ".join(VIEWS), view)
        )

    artifacts, stats = build_artifacts()

    if check_only:
        # Regenerate everything and byte-compare: a fingerprint check alone
        # would pass on hand-edited output.
        for name, want in sorted(artifacts.items()):
            path = os.path.join(OUT, name)
            if not os.path.exists(path):
                sys.exit(
                    "ERROR: %s is missing; rerun: python3 tools/gen_dual_ui.py" % name
                )
            if read(path) != want:
                sys.exit(
                    "ERROR: %s does not match freshly generated output; rerun: "
                    "python3 tools/gen_dual_ui.py" % name
                )
        # the build globs this directory, so a stale file left behind would be
        # compiled even though --check never generated it
        unexpected = sorted(set(os.listdir(OUT)) - set(artifacts))
        if unexpected:
            sys.exit(
                "ERROR: unexpected file(s) in %s: %s; remove them and rerun the generator"
                % (os.path.relpath(OUT, ROOT), ", ".join(unexpected))
            )
        print("dual-UI glue is current (fingerprint %s)" % stats["fp"][:16])
        return

    os.makedirs(OUT, exist_ok=True)
    for name, content in sorted(artifacts.items()):
        with open(os.path.join(OUT, name), "w") as f:
            f.write(content)

    print("objects_t members: %d (name sets match)" % stats["members"])
    print(
        "renamed globals: %d (symmetric across trees), ui_* pointers: %d"
        % (stats["syms"], stats["ui_ptrs"])
    )
    print(
        "style routing: %d functions + %d objects"
        % (stats["style_funcs"], stats["style_objs"])
    )
    print("wrote %s" % OUT)


if __name__ == "__main__":
    main()
