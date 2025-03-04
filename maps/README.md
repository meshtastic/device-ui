# Map Tile Starter-Kit

This folder contains a basic set of map tiles in different styles for Meshtastic UI enabled devices with SD card support. **Each zip file includes zoom levels 1 - 6 of Earth's surface**, so whereever you are you'll be able to zoom out and see in what country's location you've been beamed to.

Refer to [Credits and Attribution](#Credits-and-Attribution) for the origin of the provided map tiles.

For size reductions each single tile has been converted to 8-bit palette using the following imagemagick command:

```bash
find maps -name "*.png" -exec mogrify -colors 256 -depth 8  +dither -define png:color-type=3 -alpha Background {} \;
```

## Installing

Prepare a SDCard < 32GB and format it with Fat32 preferrably with the SD formatting tool provided by [www.sdcard.org](https://www.sdcard.org/downloads/).

For now, if your card has a bigger size you'll need to create a ~30GB partition and format it to Fat32 with your OS. exFat is not yet supported but is being worked on.

Unzip the contents of the zip file(s) into the SD card's root folder retaining the subfolder structure `/maps/\<style\>/z/x/y`.

Insert the SD card into the MUI device and switch it on. On MUI's home screen you can check if the card is detected properly: scroll down to the SD card image button where you'll find information about the size and used space of the SD card.

Pressing on the Map button in the main button bar will open the map panel and show the surrounding area of where you are positioned according your GPS, or near other nodes with position, or at your saved home position, or at a default location. In case you see only empty tiles which may be the case when you have not yet downloaded detailed map tiles for your location, zoom out to at least level 6 which will then load the tiles on your SD card.

Long pressing the Map button allows to choose between the map styles found on SD card and reveals sliders to control brightness and contrast of the tiles. Another tap on the Map button toggles the controls.

## Downloading Tiles

The map tiles are in .png format of size 256x256 pixel and zoom levels 1 - 20, where 1 represents the entire earth and 20 a mid-sized building.

While there are preparations to provide a graphical tool for convenient downloading of map tiles one of MUI's sponsors Roberto (@droberin) has already adapted a python script for automated download of compatible map tiles using map raster tile APIs from different configurable providers. With this script you can extent the existing map styles by further zoom levels or download your favorite map styles according your preference.

Python script link: https://gist.github.com/droberin/b333a216d860361e329e74f59f4af4ba

Please read the instructions carefully and don't abuse the freely provided services.

Note: when choosing a region for downloading tiles you're advised to adapt the zoom level from the outer part (e.g. level 6 - 8) to the center of the area of interest (e.g. level 15 or 16). The surrounding box is determined by two locations (lat/lon) SW / NE east of Greenwich, or NW / SE west of Greenwich, respectively, which are to be entered into the config of the downloader script. This way you can save a lot of time, space and credits.

<img src="../docs/tile_pyramid.png" alt="Tile Zoom Levels">

If you like to check of how many tiles an area is composed of you can make use of this [Tile Calculator](https://tools.geofabrik.de/calc) provided by Geofabrik.
<br>

# Extra maps

A small group of map tile enthusiasts (special thanks to @joyel24 & @teddy1602) like to share their downloaded tiles. In this section you'll find torrents for downloading complete sets of map tiles of various zoom levels.

## France

Entire France Atlas zoom 6 to 13: [Torrent Magnet Link](https://tinyurl.com/43n7uwv3) ~1.29GB md5: 1e21c497e124647dda528d71011e133c
<br>Entire France Outdoors zoom 1 to 13: [Torrent Magnet Link](https://tinyurl.com/3xhpn7j7) ~3.9GB md5: c02b89e9d5d2c7d5b3d902e6429dd5c4

## Netherlands
Entire Netherlands zoom 1 to 14 including Asmterdan until zoom 17: [Torrent Magnet Link](https://tinyurl.com/4vpuhd7n) ~956MB md5: 30ccf6484c99ae0b0765cff19acfc67e

<br>Refer to [Credits and Attribution](#Credits-and-Attribution) for the origin of the provided map tiles.

### md5sum calculation

`dir=<root_torrent_dir> ; find "$dir" -type f -exec md5sum {} \; | sed "s~$dir~~g" | LC_ALL=C sort -d | md5sum`

<br>

# Credits and Attribution

Attribution to Thunderforest, Geoapify, and OpenStreetMap for providing the tile API services:
_<br>atlas, outdoors © https://www.thunderforest.com/terms - Creative Commons licence CC-BY-SA 2.0_
_<br>positron, dark-matter-brown © https://www.geoapify.com/terms-and-conditions/ - Open Data Common Open Database License, "ODbL" 1.0_
_<br>osm © https://www.osm.org/copyright - Open Data Commons Open Database License, "ODbL" 1.0_
