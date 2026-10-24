#if defined(ARDUINO_ARCH_ESP32) && defined(HAS_SD_MMC)

#include "ff.h" // diskio_sdmmc.h needs BYTE but does not include ff.h itself

#include "storage/FatFsStorage.h"
#include "FS.h"
#include "diskio_sdmmc.h"
#include "driver/sdmmc_host.h"
#include "esp_vfs_fat.h"
#include "sd_protocol_defs.h"
#include "sdmmc_cmd.h"
#include "util/ILog.h"
#include "vfs_api.h"

#ifndef BOARD_MAX_SDMMC_FREQ
#define BOARD_MAX_SDMMC_FREQ SDMMC_FREQ_HIGHSPEED
#endif

#ifndef SD_MMC_HOST_SLOT
#define SD_MMC_HOST_SLOT SDMMC_HOST_SLOT_1
#endif

namespace
{
constexpr const char *VFS_PATH = "/sdcard";

/**
 * 1-bit SDIO card on SD_SCLK_PIN (CLK), SD_MOSI_PIN (CMD) and SD_MISO_PIN (D0).
 */
class SdmmcBus : public ISdBus
{
  public:
    SdmmcBus(void) : vfs(std::make_shared<VFSImpl>()), fs(vfs) { vfs->mountpoint(VFS_PATH); }

    IStorage::Status mount(bool formatIfNeeded) override
    {
        esp_vfs_fat_sdmmc_mount_config_t mountConfig = {};
        mountConfig.format_if_mount_failed = formatIfNeeded;
        mountConfig.max_files = 5;
        mountConfig.allocation_unit_size = 16 * 1024;

        sdmmc_host_t host = SDMMC_HOST_DEFAULT();
        host.slot = SD_MMC_HOST_SLOT;
        host.flags = SDMMC_HOST_FLAG_1BIT;
        host.max_freq_khz = BOARD_MAX_SDMMC_FREQ;

        sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
        slot.clk = (gpio_num_t)SD_SCLK_PIN;
        slot.cmd = (gpio_num_t)SD_MOSI_PIN;
        slot.d0 = (gpio_num_t)SD_MISO_PIN;
        slot.width = 1;
        // unused data lines must not claim the slot's default GPIOs
        slot.d1 = GPIO_NUM_NC;
        slot.d2 = GPIO_NUM_NC;
        slot.d3 = GPIO_NUM_NC;
        slot.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

        card = nullptr;
        esp_err_t err = esp_vfs_fat_sdmmc_mount(VFS_PATH, &host, &slot, &mountConfig, &card);
        if (err == ESP_OK)
            return IStorage::Status::Ok;
        card = nullptr;
        ILOG_WARN("SD_MMC mount failed: 0x%x %s", (int)err, esp_err_to_name(err));
        switch (err) {
        case ESP_FAIL: // card answered, filesystem did not mount
            return IStorage::Status::NoFilesystem;
        case ESP_ERR_TIMEOUT:
        case ESP_ERR_NOT_FOUND:
        case ESP_ERR_INVALID_RESPONSE:
            return IStorage::Status::SlotEmpty;
        default:
            return IStorage::Status::CardError;
        }
    }

    void unmount(void) override
    {
        if (card) {
            esp_vfs_fat_sdcard_unmount(VFS_PATH, card);
            card = nullptr;
        }
    }

    uint8_t drive(void) const override { return card ? ff_diskio_get_pdrv_card(card) : 0xFF; }

    StorageInfo::CardType cardType(void) override
    {
        if (!card)
            return StorageInfo::eNone;
        if (card->is_mmc)
            return StorageInfo::eMMC;
        if (!(card->ocr & SD_OCR_SDHC_CAP))
            return StorageInfo::eSD;
        uint64_t size = (uint64_t)card->csd.capacity * card->csd.sector_size;
        return size > 32ULL * 1024 * 1024 * 1024 ? StorageInfo::eSDXC : StorageInfo::eSDHC;
    }

    fs::FS *arduinoFS(void) override { return &fs; }

  private:
    sdmmc_card_t *card = nullptr;
    std::shared_ptr<VFSImpl> vfs;
    fs::FS fs;
};
} // namespace

std::unique_ptr<ISdBus> createSdmmcBus(void)
{
    return std::unique_ptr<ISdBus>(new SdmmcBus);
}

#endif
