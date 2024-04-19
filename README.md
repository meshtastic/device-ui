#

<div align="center">

<img alt="meshtastic" src="https://avatars.githubusercontent.com/u/61627050?s=200&v=4" width="80" height="80">

  <h1 align="center"> Meshtastic device-ui library</h1>
  <p style="font-size:20px;" align="center">A versatile UI library for the <a href="https://meshtastic.org">meshtastic® project</a> </p>
</div>

<!--Project specific badges here-->

<p align="center">
<a href="">
    <img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/mverch67/device-ui"></a>
    <a href="https://github.com/mverch67/device-ui/issues">
    <img alt="GitHub issues" src="https://img.shields.io/github/issues/mverch67/device-ui"></a>
  <a href="https://opencollective.com/meshtastic">
    <img alt="Open Collective backers" src="https://img.shields.io/opencollective/backers/meshtastic?label=support%20meshtastic">
  </a>
</p>
<div style="text-align: center;">

[![CLA assistant](https://cla-assistant.io/readme/badge/mverch67/device-ui)](https://cla-assistant.io/mverch67/device-ui)
[![Fiscal Contributors](https://opencollective.com/meshtastic/tiers/badge.svg?label=Fiscal%20Contributors&color=deeppink)](https://opencollective.com/meshtastic)
[![Vercel](https://img.shields.io/static/v1?label=Powered%20by&message=Vercel&style=flat&logo=vercel&color=000000)](https://vercel.com?utm_source=meshtastic&utm_campaign=oss)

</div>

## :wave: Introduction

### Meshtastic device-ui library for TFT and OLED screens

This C++ library is intended to support the following scenarios with enhanced screen UI:

- Integrated with meshtastic firmware for LoRa devices with TFT display (or potentially also OLED +PSRAM)
- Stand-alone TFT+MCU devices such as WT32-SC01, CYD or T-HMI connected with meshtastic LoRa devices
- Linux based devices with LoRa shield, e.g Raspberry Pi / Milk-V Duo/Mars with TFT display (hat or diy)
- Native Linux X11 application with SimRadio e.g. for tests, GUI simulation & debugging (MQTT only application)

<img src="docs/T-Deck.jpg" alt="scenario 1" width="260" height="200">
<img src="docs/CYD.png" alt="scenario 2" width="280" height="200">
<img src="docs/Pi400-TFT.jpg" alt="scenario 3" width="220" height="200">
<img src="docs/X11.png" alt="scenario 4" width="280" height="200">

## :pencil: TODOs

### :japanese_castle: General Architecture

    - [ ] Overall design (MVC approach)
      - [x] DisplayDriver inheritance hierarchy
      - [x] DisplayDriver factory
      - [x] TFT Driver
      - [ ] OLED Driver
      - [ ] E-Ink Driver
      - [x] View hierarchy
      - [x] View factory
      - [x] Controller design and interface implementation
      - [x] Controller <-> model interface
        - [x] Packet based thread safe interface
        - [x] serial communication interface
        - [x] protobuf encoding/decoding
      - [x] Logging interface
      - [ ] Add lvgl compatible input driver interface
      - [ ] Add interface for persistency via lvgl littlefs callbacks
        - [ ] Screen calibration data
        - [ ] Device settings (General)
        - [ ] Message storage
        - [ ] Serial connection config
    - [ ] Dynamic behavior
      - [x] Startup config
      - [x] Restart behavior
      - [x] Display sleep
      - [ ] Heartbeat timer based on device input actions
    - [ ] Localisation support
      - [ ] German translation
      - [ ] Spanish translation
      - [ ] Portuguese translation
      - [ ] French translation
      - [ ] Italian translation
      - [ ] (other languages with latin characters)
    - [ ] Support dynamic OLED / Color(TFT) selection
    - [ ] Add support for UI scaling and try eliminate fixed positioning (lvgl v9)
    - [ ] Allow co-existence of generated files/views by different SquareLine Studio projects
    - [ ] Fix PSRAM draw buffer issue and do buffer size optimization
    - [ ] Test native lvgl driver support
      - [ ] TFT drivers
      - [ ] OLED drivers
    - [ ] Check lovyanGFX double-buffering DMA capabilities
    - [ ] Check lvgl native driver DMA double-buffering
    - [ ] E-Ink support
    - [ ] RP2040 support

### Meshtastic UI (general)

    - [x] Boot Screen
    - [x] Home Screen
      - [x] Messages info
      - [x] Nodes info
      - [x] GPS info
      - [ ] WiFi
      - [ ] Bluetooth
      - [ ] MQTT
      - [x] Free memory
    - [ ] Nodes panel
      - [x] Scroll display and sorting
      - [ ] Node details
      - [ ] Repeater support (manual insertion)
      - [ ] LastHeard & time source handling improvements
      - [ ] Remote Node configuration
    - [x] Group channel panel
    - [ ] Chat panel
      - [x] Scroll container and messages display
      - [x] Virtual keyboard
      - [ ] Message acknowledgement
      - [x] Delete chat
    - [ ] Map
      - [ ] Tiles dynamic loading
        - [ ] SD card
        - [ ] WLAN
      - [ ] Pan & Zoom
      - [ ] Node locations
    - [ ] General Settings
      - [ ] Language
      - [ ] Timezone
      - [ ] Screen
      - [ ] Maps
      - [ ] Audio
    - [ ] Radio Settings
      - [ ] Frequency region configuration
      - [ ] Channel configuration & QR code display
    - [ ] Module Settings
    - [x] Status bar with battery symbol
    - [ ] UI Keyboard navigation & control

### :pager: T-Deck

    - [x] Firmware project integration
    - [ ] T-Deck UI
      - [x] 320x240 View
      - [x] GPS position
      - [ ] Node location and precision
      - [ ] Radio frequency display
      - [ ] Offline map display
    - [ ] I2C keyboard input handling
    - [ ] Trackball support (e.g. fast scrolling list views)
    - [ ] SD card support, mainly for offline maps or import/export (config & chat history)
    - [ ] (Optional): allow bluetooth connection (-> PhoneAPI queue re-design)
    - [ ] I2S Audio support?

### :watch: T-Watch

    - [ ] T-Watch UI
      - [ ] 240x240 View
      - [ ] Clock screen
    - [ ] Firmware project integration
    - [ ] Continue work on SquareLine Studio UI screens :tada:
    - [ ] Refactoring of common code with 320x240 view into base class

### OLED

    - [ ] Provide demo for OLED 128x64 screen
    - [ ] Space and RAM requirements analysis

### :penguin: Portduino (Raspberry / native linux)

    - [x] Project integration into firmware
    - [x] Display driver run-time configuration interface
    - [ ] Add missing settingsMap entries for DisplayDriverConfig
    - [ ] Integrate lvgl keyboard input driver
    - [ ] Add support for several SPI devices
    - [ ] Add pwm brightness control
    - [ ] Target environment cleanup

### :iphone: Stand-alone Device

    - [x] Dedicated device-ui project
    - [x] Sunton CYD support (320x240)
    - [x] LilyGo T-HMI support (320x240)
    - [ ] WT32-SC01 (Plus) support (480x320)
      - [x] Display driver
      - [ ] 480x320 view
    - [ ] Fix/Workaround serial light sleep UART reading issue (-> firmware)
    - [ ] Heartbeat timer
    - [x] Serial data send/receive
      - [x] UART connection support
      - [ ] WLAN connection support
      - [ ] Bluetooth connection support
    - [ ] Serial Interface configuration UI screen
    - [ ] Allow serial connection initialisation at runtime

## Architecture Overview device-ui library (Class Diagram)

<img src="docs/class-diagram.png" alt="class diagram">

## Stats

![Alt](https://repobeats.axiom.co/api/embed/2d7384f6fd2724a3b3b9d320fcd77717d7f92d7e.svg "Repobeats analytics image")
