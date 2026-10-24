#if defined(ARDUINO_ARCH_ESP32) && defined(SDCARD_USE_SOFT_SPI)

#include "storage/FatFsStorage.h"
#include "FS.h"
#include "diskio_impl.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "esp_vfs_fat.h"
#include "ff.h"
#include "util/ILog.h"
#include "vfs_api.h"
#include <cstdlib>

/**
 * Bit-banged SPI SD card driver registered as a FatFs disk, for boards whose
 * hardware SPI hosts are all taken (SPI_SCK/SPI_MISO/SPI_MOSI/SDCARD_CS).
 */
namespace
{
constexpr const char *VFS_PATH = "/sd";
constexpr size_t MAX_FILES = 5;
constexpr uint32_t SECTOR = 512;

// SPI mode commands; bit 7 marks an application command (sent after CMD55)
enum : uint8_t {
    CMD0 = 0,
    CMD1 = 1,
    CMD8 = 8,
    CMD9 = 9,
    CMD12 = 12,
    CMD16 = 16,
    CMD17 = 17,
    CMD18 = 18,
    CMD24 = 24,
    CMD25 = 25,
    CMD55 = 55,
    CMD58 = 58,
    ACMD41 = 0x80 | 41,
};

enum CardKind : uint8_t { kNone, kMMC, kSD1, kSD2, kSDHC }; // only kSDHC is block addressed

struct {
    gpio_num_t sck = (gpio_num_t)SPI_SCK;
    gpio_num_t miso = (gpio_num_t)SPI_MISO;
    gpio_num_t mosi = (gpio_num_t)SPI_MOSI;
    gpio_num_t cs = (gpio_num_t)SDCARD_CS;
    bool slow = true; // card identification runs below 400 kHz
    CardKind kind = kNone;
    LBA_t sectors = 0;
    DSTATUS status = STA_NOINIT;
} sd;

int64_t deadline(uint32_t ms)
{
    return esp_timer_get_time() + (int64_t)ms * 1000;
}

bool expired(int64_t until)
{
    return esp_timer_get_time() >= until;
}

uint8_t xfer(uint8_t out)
{
    uint8_t in = 0;
    for (int bit = 7; bit >= 0; bit--) {
        gpio_set_level(sd.mosi, (out >> bit) & 1);
        if (sd.slow)
            esp_rom_delay_us(2);
        gpio_set_level(sd.sck, 1);
        in = (in << 1) | (gpio_get_level(sd.miso) & 1);
        if (sd.slow)
            esp_rom_delay_us(2);
        gpio_set_level(sd.sck, 0);
    }
    return in;
}

bool waitReady(uint32_t ms)
{
    int64_t until = deadline(ms);
    do {
        if (xfer(0xFF) == 0xFF)
            return true;
    } while (!expired(until));
    return false;
}

void cardRelease(void)
{
    gpio_set_level(sd.cs, 1);
    xfer(0xFF); // one more clock so the card releases MISO
}

bool cardSelect(void)
{
    gpio_set_level(sd.cs, 0);
    xfer(0xFF);
    if (waitReady(500))
        return true;
    cardRelease();
    return false;
}

uint8_t command(uint8_t cmd, uint32_t arg)
{
    if (cmd & 0x80) {
        cmd &= 0x7F;
        uint8_t res = command(CMD55, 0);
        if (res > 1)
            return res;
    }
    if (cmd != CMD12) {
        cardRelease();
        if (!cardSelect())
            return 0xFF;
    }
    xfer(0x40 | cmd);
    xfer(arg >> 24);
    xfer(arg >> 16);
    xfer(arg >> 8);
    xfer(arg);
    xfer(cmd == CMD0 ? 0x95 : cmd == CMD8 ? 0x87 : 0x01); // only these two are CRC checked
    if (cmd == CMD12)
        xfer(0xFF); // stuff byte
    uint8_t res;
    int tries = 10;
    do {
        res = xfer(0xFF);
    } while ((res & 0x80) && --tries);
    return res;
}

bool receive(uint8_t *buf, uint32_t len)
{
    int64_t until = deadline(200);
    uint8_t token;
    do {
        token = xfer(0xFF);
    } while (token == 0xFF && !expired(until));
    if (token != 0xFE)
        return false;
    for (uint32_t i = 0; i < len; i++)
        buf[i] = xfer(0xFF);
    xfer(0xFF); // CRC
    xfer(0xFF);
    return true;
}

bool transmit(const uint8_t *buf, uint8_t token)
{
    if (!waitReady(500))
        return false;
    xfer(token);
    if (token == 0xFD) // stop token of a multi-block write
        return true;
    for (uint32_t i = 0; i < SECTOR; i++)
        xfer(buf[i]);
    xfer(0xFF); // CRC
    xfer(0xFF);
    return (xfer(0xFF) & 0x1F) == 0x05;
}

LBA_t csdSectors(const uint8_t *csd)
{
    if ((csd[0] >> 6) == 1) { // CSD 2.0
        uint32_t size = ((uint32_t)(csd[7] & 0x3F) << 16) | ((uint32_t)csd[8] << 8) | csd[9];
        return (LBA_t)(size + 1) << 10;
    }
    uint8_t shift = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
    uint32_t size = (csd[8] >> 6) + ((uint32_t)csd[7] << 2) + ((uint32_t)(csd[6] & 3) << 10) + 1;
    return (LBA_t)size << (shift - 9);
}

DSTATUS diskInitialize(unsigned char)
{
    gpio_reset_pin(sd.cs);
    gpio_reset_pin(sd.sck);
    gpio_reset_pin(sd.mosi);
    gpio_reset_pin(sd.miso);
    gpio_set_direction(sd.cs, GPIO_MODE_OUTPUT);
    gpio_set_direction(sd.sck, GPIO_MODE_OUTPUT);
    gpio_set_direction(sd.mosi, GPIO_MODE_OUTPUT);
    gpio_set_direction(sd.miso, GPIO_MODE_INPUT);
    gpio_set_pull_mode(sd.miso, GPIO_PULLUP_ONLY);
    gpio_set_level(sd.cs, 1);
    gpio_set_level(sd.sck, 0);

    sd.slow = true;
    for (int i = 0; i < 10; i++) // at least 74 clocks with CS high
        xfer(0xFF);

    CardKind kind = kNone;
    if (command(CMD0, 0) == 1) {
        int64_t until = deadline(1000);
        uint8_t ocr[4];
        if (command(CMD8, 0x1AA) == 1) {
            for (int i = 0; i < 4; i++)
                ocr[i] = xfer(0xFF);
            if (ocr[2] == 0x01 && ocr[3] == 0xAA) {
                while (!expired(until) && command(ACMD41, 1UL << 30))
                    ;
                if (!expired(until) && command(CMD58, 0) == 0) {
                    for (int i = 0; i < 4; i++)
                        ocr[i] = xfer(0xFF);
                    kind = (ocr[0] & 0x40) ? kSDHC : kSD2;
                }
            }
        } else {
            uint8_t init;
            if (command(ACMD41, 0) <= 1) {
                kind = kSD1;
                init = ACMD41;
            } else {
                kind = kMMC;
                init = CMD1;
            }
            while (!expired(until) && command(init, 0))
                ;
            if (expired(until) || command(CMD16, SECTOR) != 0)
                kind = kNone;
        }
    }

    sd.slow = false;
    sd.sectors = 0;
    if (kind != kNone) {
        uint8_t csd[16];
        if (command(CMD9, 0) == 0 && receive(csd, sizeof(csd)))
            sd.sectors = csdSectors(csd);
    }
    cardRelease();
    sd.kind = kind;
    sd.status = kind == kNone ? STA_NOINIT : 0;
    return sd.status;
}

DSTATUS diskStatus(unsigned char)
{
    return sd.status;
}

DRESULT diskRead(unsigned char, unsigned char *buf, uint32_t sector, unsigned count)
{
    if (sd.status & STA_NOINIT)
        return RES_NOTRDY;
    if (sd.kind != kSDHC)
        sector *= SECTOR;
    if (count == 1) {
        if (command(CMD17, sector) == 0 && receive(buf, SECTOR))
            count = 0;
    } else if (command(CMD18, sector) == 0) {
        do {
            if (!receive(buf, SECTOR))
                break;
            buf += SECTOR;
        } while (--count);
        command(CMD12, 0);
    }
    cardRelease();
    return count ? RES_ERROR : RES_OK;
}

DRESULT diskWrite(unsigned char, const unsigned char *buf, uint32_t sector, unsigned count)
{
    if (sd.status & STA_NOINIT)
        return RES_NOTRDY;
    if (sd.kind != kSDHC)
        sector *= SECTOR;
    if (count == 1) {
        if (command(CMD24, sector) == 0 && transmit(buf, 0xFE))
            count = 0;
    } else if (command(CMD25, sector) == 0) {
        do {
            if (!transmit(buf, 0xFC))
                break;
            buf += SECTOR;
        } while (--count);
        if (!transmit(nullptr, 0xFD))
            count = 1;
    }
    cardRelease();
    return count ? RES_ERROR : RES_OK;
}

DRESULT diskIoctl(unsigned char, unsigned char cmd, void *buff)
{
    if (sd.status & STA_NOINIT)
        return RES_NOTRDY;
    DRESULT res = RES_OK;
    switch (cmd) {
    case CTRL_SYNC:
        res = cardSelect() ? RES_OK : RES_ERROR;
        break;
    case GET_SECTOR_COUNT:
        *(LBA_t *)buff = sd.sectors;
        res = sd.sectors ? RES_OK : RES_ERROR;
        break;
    case GET_SECTOR_SIZE:
        *(WORD *)buff = SECTOR;
        break;
    case GET_BLOCK_SIZE:
        *(DWORD *)buff = 1; // erase block size unknown
        break;
    case CTRL_TRIM:
        break;
    default:
        res = RES_PARERR;
        break;
    }
    cardRelease();
    return res;
}

const ff_diskio_impl_t softSpiDisk = {&diskInitialize, &diskStatus, &diskRead, &diskWrite, &diskIoctl};

class SoftSpiBus : public ISdBus
{
  public:
    SoftSpiBus(void) : vfs(std::make_shared<VFSImpl>()), fs(vfs) { vfs->mountpoint(VFS_PATH); }

    IStorage::Status mount(bool formatIfNeeded) override
    {
        BYTE drv = 0xFF;
        if (ff_diskio_get_drive(&drv) != ESP_OK || drv == 0xFF)
            return IStorage::Status::CardError;
        ff_diskio_register(drv, &softSpiDisk);
        if (diskInitialize(drv) & STA_NOINIT) {
            ff_diskio_unregister(drv);
            return IStorage::Status::SlotEmpty;
        }

        char path[3] = {(char)('0' + drv), ':', '\0'};
        esp_vfs_fat_conf_t conf = {};
        conf.base_path = VFS_PATH;
        conf.fat_drive = path;
        conf.max_files = MAX_FILES;
        FATFS *fat = nullptr;
        if (esp_vfs_fat_register_cfg(&conf, &fat) != ESP_OK) {
            ff_diskio_unregister(drv);
            return IStorage::Status::CardError;
        }

        FRESULT res = f_mount(fat, path, 1);
        if (res == FR_NO_FILESYSTEM && formatIfNeeded) {
            void *work = malloc(FF_MAX_SS);
            if (work) {
                const MKFS_PARM opt = {(BYTE)FM_ANY, 0, 0, 0, 0};
                res = f_mkfs(path, &opt, work, FF_MAX_SS);
                free(work);
                if (res == FR_OK)
                    res = f_mount(fat, path, 1);
            }
        }
        if (res != FR_OK) {
            ILOG_WARN("soft SPI SD mount failed: %d", (int)res);
            f_mount(nullptr, path, 0);
            esp_vfs_fat_unregister_path(VFS_PATH);
            ff_diskio_unregister(drv);
            return res == FR_NO_FILESYSTEM ? IStorage::Status::NoFilesystem : IStorage::Status::CardError;
        }
        pdrv = drv;
        return IStorage::Status::Ok;
    }

    void unmount(void) override
    {
        if (pdrv == 0xFF)
            return;
        char path[3] = {(char)('0' + pdrv), ':', '\0'};
        f_mount(nullptr, path, 0);
        esp_vfs_fat_unregister_path(VFS_PATH);
        ff_diskio_unregister(pdrv);
        pdrv = 0xFF;
        sd.status = STA_NOINIT;
    }

    uint8_t drive(void) const override { return pdrv; }

    StorageInfo::CardType cardType(void) override
    {
        switch (sd.kind) {
        case kMMC:
            return StorageInfo::eMMC;
        case kSD1:
        case kSD2:
            return StorageInfo::eSD;
        case kSDHC:
            return (uint64_t)sd.sectors * SECTOR > 32ULL * 1024 * 1024 * 1024 ? StorageInfo::eSDXC : StorageInfo::eSDHC;
        default:
            return StorageInfo::eNone;
        }
    }

    fs::FS *arduinoFS(void) override { return &fs; }

  private:
    uint8_t pdrv = 0xFF;
    std::shared_ptr<VFSImpl> vfs;
    fs::FS fs;
};
} // namespace

std::unique_ptr<ISdBus> createSoftSpiBus(void)
{
    return std::unique_ptr<ISdBus>(new SoftSpiBus);
}

#endif
