# See https://docs.platformio.org/en/latest/manifests/library-json/fields/build/extrascript.html
Import("env")
import os
import subprocess
from os.path import basename, join, realpath

# Base srcFilter. Cannot be set in library.json.
src_filter = [
    "+<resources>",
    "+<locale>",
    "+<source>"
]

DUAL_UI_VIEWS = ("ui_320x240", "ui_240x320")


def define_name(item):
    # CPPDEFINES entries are either "NAME" or a ("NAME", value) pair
    if isinstance(item, str):
        return item
    if isinstance(item, (list, tuple)) and item:
        return str(item[0])
    return ""


view = None
runtime_rotation = False
for item in env.get("CPPDEFINES", []):
    name = define_name(item)
    # Add generated view directory to include path dependending on VIEW_* macro
    if name.startswith("VIEW_"):
        view = f"ui_{name[5:]}".lower()  # Ex value: "ui_320x240"
        env.Append(CPPPATH=[realpath(join("generated", view))])
        src_filter.append(f"+<generated/{view}>")
    elif name == "MUI_RUNTIME_ROTATION":
        runtime_rotation = True
    # Add portduino directory to include path dependending on ARCH_PORTDUINO macro
    elif name == "ARCH_PORTDUINO":
        env.Append(CPPPATH=[realpath("portduino")])
        src_filter.append("+<portduino>")

if runtime_rotation:
    # Link both perpendicular layouts so the rotation becomes a runtime setting.
    # The primary tree (this build's VIEW_*) supplies every shared asset; the
    # secondary tree is compiled with its colliding globals renamed through a
    # generated forced include.
    if view not in DUAL_UI_VIEWS:
        raise Exception("MUI_RUNTIME_ROTATION supports only %s, not %r"
                        % (" and ".join(DUAL_UI_VIEWS), view))
    secondary = DUAL_UI_VIEWS[1] if view == DUAL_UI_VIEWS[0] else DUAL_UI_VIEWS[0]

    lib_root = realpath(".")
    generated = join(lib_root, "tools", "generated", "dual")
    generator = join(lib_root, "tools", "gen_dual_ui.py")

    # Fails the build when the committed glue no longer matches the UI trees.
    check = subprocess.run([env.subst("$PYTHONEXE"), generator, "--check", view],
                           capture_output=True, text=True)
    if check.returncode != 0:
        raise Exception("MUI_RUNTIME_ROTATION: %s%s" % (check.stdout, check.stderr))

    core_sources, secondary_assets, glue_sources = [], [], []
    with open(join(generated, "manifest.txt")) as f:
        for line in f:
            key, _, value = line.strip().partition(" ")
            if key == "core_source":
                core_sources.append(value)
            elif key == "glue_source":
                glue_sources.append(value)
            elif key == "secondary_asset":
                # "secondary_asset <view> <file>": only when <view> is the
                # secondary tree does its unique asset have to be compiled
                view_name, _, asset = value.partition(" ")
                if view_name == secondary:
                    secondary_assets.append(asset)

    for src_name in core_sources + secondary_assets:
        if os.path.exists(join(lib_root, "generated", secondary, src_name)):
            src_filter.append(f"+<generated/{secondary}/{src_name}>")
    # listed rather than globbed, so a stale .c cannot enter the build
    for src_name in glue_sources:
        src_filter.append(f"+<tools/generated/dual/{src_name}>")
    env.Append(CPPPATH=[generated])

    rename_flags = ["-include", join(generated, "ui_secondary_rename.h"),
                    "-I", realpath(join("generated", secondary))]
    routing_flags = ["-include", join(generated, "style_routing.h")]
    STYLE_ROUTED_CXX = ("Themes.cpp", "TFTView_320x240.cpp")

    def apply_dual_ui_flags(env, node):
        path = realpath(node.srcnode().get_abspath())
        name = basename(path)
        secondary_tu = os.sep + secondary + os.sep in path and name not in secondary_assets
        # the secondary-side bridge needs the secondary screens.h and the
        # renames; the primary-side bridge must see the primary tree untouched
        if secondary_tu or name == "bridge_secondary.c":
            return env.Object(node, CPPFLAGS=env.get("CPPFLAGS", []) + rename_flags)
        if name in STYLE_ROUTED_CXX:
            return env.Object(node, CXXFLAGS=env.get("CXXFLAGS", []) + routing_flags)
        return node

    env.AddBuildMiddleware(apply_dual_ui_flags, "*")

# Only `Replace` is supported for SRC_FILTER, not `Append` or `Prepend`
env.Replace(SRC_FILTER=src_filter)

# Dump construction environment (for debug purposes)
# print("meshtastic-device-ui Library ENV:")
# print(env.Dump())
