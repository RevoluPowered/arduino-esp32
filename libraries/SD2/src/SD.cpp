#include "SD.h"

#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
//#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
//#include "sdmmc_cmd.h"

namespace SD2
{
    // todo: move these into class so they can be updated before begin gets called.
    #define PIN_NUM_MISO 2
    #define PIN_NUM_MOSI 15
    #define PIN_NUM_CLK  14
    #define PIN_NUM_CS   13 
    static const char * TAG = "SD2 library";


    boolean SD::begin( card_connection_mode mode, boolean auto_format_if_mount_failed )
    {
        sdmmc_host_t host;
        sdmmc_slot_config_t slot_config;
        switch(mode)
        {
            case DEFAULT_PIN_MAPPING:
                ESP_LOGI(TAG, "Using SDMMC peripheral");
                host = SDMMC_HOST_DEFAULT();

                // This initializes the slot without card detect (CD) and write protect (WP) signals.
                // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
                slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
            break;
            case INTERNAL_PULLUP_MAPPING:
                ESP_LOGI(TAG, "Using SDMMC peripheral with internal pullups - note external pullups still required!");
                host = SDMMC_HOST_DEFAULT();

                // This initializes the slot without card detect (CD) and write protect (WP) signals.
                // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
                slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

                // GPIOs 15, 2, 4, 12, 13 should have external 10k pull-ups.
                // Internal pull-ups are not sufficient. However, enabling internal pull-ups
                // does make a difference some boards, so we do that here.
                gpio_set_pull_mode(15, GPIO_PULLUP_ONLY);   // CMD, needed in 4- and 1- line modes
                gpio_set_pull_mode(2, GPIO_PULLUP_ONLY);    // D0, needed in 4- and 1-line modes
                gpio_set_pull_mode(4, GPIO_PULLUP_ONLY);    // D1, needed in 4-line mode only
                gpio_set_pull_mode(12, GPIO_PULLUP_ONLY);   // D2, needed in 4-line mode only
                gpio_set_pull_mode(13, GPIO_PULLUP_ONLY);   // D3, needed in 4- and 1-line modes
            case SPI_MAPPING:
                ESP_LOGI(TAG, "Using SPI peripheral to access SD card");

                sdmmc_host_t host = SDSPI_HOST_DEFAULT();
                sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
                slot_config.gpio_miso = PIN_NUM_MISO;
                slot_config.gpio_mosi = PIN_NUM_MOSI;
                slot_config.gpio_sck  = PIN_NUM_CLK;
                slot_config.gpio_cs   = PIN_NUM_CS;
                // This initializes the slot without card detect (CD) and write protect (WP) signals.
                // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
            break;
            case SINGLE_LINE_SDMODE:
                ESP_LOGI(TAG, "Using Single line SD Mode");
                host = SDMMC_HOST_DEFAULT();
                host.flags = SDMMC_HOST_FLAG_1BIT;

                slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
                slot_config.width = 1;
                
            break;
            default:
                ESP_LOGE(TAG, "Error: configuration specified doesn't exist for SDMODE flag");
                return false;
            break;

            // Options for mounting the filesystem.
            // If format_if_mount_failed is set to true, SD card will be partitioned and
            // formatted in case when mounting fails.
            esp_vfs_fat_sdmmc_mount_config_t mount_config = {
                .format_if_mount_failed = auto_format_if_mount_failed,
                .max_files = 5, // todo: might need to make this configurable.
                .allocation_unit_size = 16 * 1024
            };

            // Use settings defined above to initialize SD card and mount FAT filesystem.
            // Note: esp_vfs_fat_sdmmc_mount is an all-in-one convenience function.
            // Please check its source code and implement error recovery when developing
            // production applications.
            sdmmc_card_t* card;
            esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

            if (ret != ESP_OK) {
                if (ret == ESP_FAIL) {
                    ESP_LOGE(TAG, "Failed to mount filesystem. "
                        "If you want the card to be formatted, set format_if_mount_failed = true.");
                } 
                else {
                    ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                        "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
                }
                return false;
            }

            // Card has been initialized, print its properties
            sdmmc_card_print_info(stdout, card);
            status = true;
            return true;
        }
    }

    boolean SD::end()
    {
        if(status)
        {
            esp_vfs_fat_sdmmc_unmount();
            ESP_LOGI(TAG, "Card unmounted");
            return true;
        }
        else
        {
            ESP_LOGI(TAG, "Card was never mounted so not unmounting!");
        }
    }
}