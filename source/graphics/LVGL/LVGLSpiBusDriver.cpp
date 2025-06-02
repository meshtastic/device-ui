#if defined(LV_USE_ST7789) && LV_USE_ST7789 == 1

#include "graphics/LVGL/LVGLSpiBusDriver.h"
#include "util/ILog.h"

SPIClass *LVGLSpiBusDriver::spi = nullptr;
LVGLSpiBusDriver *LVGLSpiBusDriver::instance = nullptr;

LVGLSpiBusDriver::LVGLSpiBusDriver(uint16_t width, uint16_t height, CreateCB createDisplayFunc)
    : /* DisplayDeviceDriver(width, height), */
      rst(-1), dc(-1), cs(-1), sclk(-1), mosi(-1), miso(-1), spiBus(-1), lv_createDisplay(createDisplayFunc)
{
    ILOG_DEBUG("LVGLSpiBusDriver()...");
    instance = this;
}

void LVGLSpiBusDriver::init(int16_t spiBus, int16_t sclk, int16_t mosi, int16_t miso, int16_t dc, int16_t rst, int16_t cs)
{
    ILOG_DEBUG("LVGLSpiBusDriver::init(): spiBus=%d, sclk=%d, mosi=%d, miso=%d, dc=%d, rst=%d, cs=%d", spiBus, sclk, mosi, miso,
               dc, rst, cs);
    this->spiBus = spiBus;
    this->sclk = sclk;
    this->mosi = mosi;
    this->miso = miso;
    this->dc = dc;
    this->rst = rst;
    this->cs = cs;
    this->spi = new SPIClass(spiBus);
    this->spiSettings = SPISettings(40000000, MSBFIRST, SPI_MODE0);
    this->connect();
}

void LVGLSpiBusDriver::connect(void)
{
    ILOG_DEBUG("LVGLSpiBusDriver::connect()...");
    if (spi) {
#ifdef ARCH_ESP32
        spi->begin(sclk, miso, mosi, cs);
        spi->setClockDivider(SPI_CLOCK_DIV2);
#else
        spi->begin(40000000);
#endif
    }
    // Set pin modes
    pinMode(dc, OUTPUT);
    pinMode(rst, OUTPUT);

    if (cs != -1) {
        pinMode(cs, OUTPUT);
        digitalWrite(cs, HIGH);
    }

    // Reset the display
    if (rst != -1) {
        digitalWrite(rst, HIGH);
        delay(1);
        digitalWrite(rst, LOW);
        delay(10);
        digitalWrite(rst, HIGH);
    }
}

/**
 * Create an LCD display
 * @param hor_res       horizontal resolution
 * @param ver_res       vertical resolution
 * @param flags         default configuration settings (mirror, RGB ordering, etc.)
 * @param send_cmd      platform-dependent function to send a command to the LCD controller (usually uses polling transfer)
 * @param send_color    platform-dependent function to send pixel data to the LCD controller (usually uses DMA transfer: must
 *                      implement a 'ready' callback)
 * @return              pointer to the created display
 */
lv_display_t *LVGLSpiBusDriver::createDisplay(uint32_t hor_res, uint32_t ver_res)
{
    ILOG_DEBUG("lv_st7789_create...");
    if (spi) {
        lv_display_t *display = lv_createDisplay(hor_res, ver_res, LV_LCD_FLAG_NONE, lcd_send_cmd_cb, lcd_send_color_cb);
        lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
        return display;
    } else {
        ILOG_ERROR("SPI not initialized!");
        return nullptr;
    }
}

/**
 * Send short command to the LCD.
 * This function shall wait until the transaction finishes.
 *
 */
void LVGLSpiBusDriver::lcd_send_cmd_cb(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param,
                                       size_t param_size)
{
    ILOG_DEBUG("LVGLSpiBusDriver::lcd_send_cmd_cb() %d/%d bytes...", cmd_size, param_size);

    digitalWrite(instance->dc, LOW); // Command mode
    if (instance->cs != -1) {
        digitalWrite(instance->cs, LOW);
    }
    instance->spi->beginTransaction(instance->spiSettings);
    instance->spi->transfer((void*)cmd, cmd_size);

    digitalWrite(instance->dc, HIGH); // Data mode
    if (param && param_size > 0) {
        instance->spi->transfer((void*)param, param_size);
    }
    instance->spi->endTransaction();
    if (instance->cs != -1) {
        digitalWrite(instance->cs, HIGH);
    }
}

/**
 * Send large array of pixel data to the LCD. If necessary, this function has to do the byte-swapping.
 * This function can do the transfer in the background.
 *
 */
void LVGLSpiBusDriver::lcd_send_color_cb(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param,
                                         size_t param_size)
{
    ILOG_DEBUG("LVGLSpiBusDriver::lcd_send_color_cb() %d bytes...", param_size);

    digitalWrite(instance->dc, HIGH); // Data mode
    if (instance->cs != -1) {
        digitalWrite(instance->cs, LOW);
    }
    instance->spi->beginTransaction(instance->spiSettings);
    instance->spi->transfer((void*)param, param_size);
    instance->spi->endTransaction();
    if (instance->cs != -1) {
        digitalWrite(instance->cs, HIGH);
    }
}

#endif
