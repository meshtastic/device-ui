# See https://docs.platformio.org/en/latest/manifests/library-json/fields/build/extrascript.html
Import("env")
from os.path import join, realpath
# Base srcFilter. Cannot be set in library.json.
src_filter = [
    "+<resources>",
    "+<locale>",
    "+<source>"
]

for item in env.get("CPPDEFINES", []):
    # Add generated view directory to include path dependending on VIEW_* macro
    if isinstance(item,str) and item.startswith("VIEW_"):
        view = f"ui_{item[5:]}".lower()  # Ex value: "ui_320x240"
        env.Append(CPPPATH=[realpath(join("generated", view))])
        src_filter.append(f"+<generated/{view}>")
    # Add portduino directory to include path dependending on ARCH_PORTDUINO macro
    elif item == "ARCH_PORTDUINO":
        env.Append(CPPPATH=[realpath("portduino")])
        src_filter.append("+<portduino>")

# Only `Replace` is supported for SRC_FILTER, not `Append` or `Prepend`
env.Replace(SRC_FILTER=src_filter)

# Dump construction environment (for debug purposes)
# print("meshtastic-device-ui Library ENV:")
# print(env.Dump())
