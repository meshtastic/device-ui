/*
 * SPDX-FileCopyrightText: 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LP5814_H
#define LP5814_H

#include "driver/i2c_master.h"
#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------
// I2C ADDRESS
// --------------------------------------------

#define LP5814_I2C_ADDR_DEFAULT (0x2C)

// --------------------------------------------
// REGISTER ADDRESS
// --------------------------------------------

#define LP5814_REG_DEVICE_CONFIG0 (0x00)
#define LP5814_REG_MAX_CURRENT (0x01)
#define LP5814_REG_ENABLE_CONTROL (0x02)
#define LP5814_REG_DIM_MODE (0x04)
#define LP5814_REG_ENGINE_MODE (0x05)
#define LP5814_REG_UPDATE (0x0F)

#define LP5814_REG_LED0_DC (0x14)
#define LP5814_REG_LED1_DC (0x15)
#define LP5814_REG_LED2_DC (0x16)
#define LP5814_REG_LED3_DC (0x17)

#define LP5814_REG_LED0_PWM (0x18)
#define LP5814_REG_LED1_PWM (0x19)
#define LP5814_REG_LED2_PWM (0x1A)
#define LP5814_REG_LED3_PWM (0x1B)

// --------------------------------------------
// DEVICE CONFIG0 REGISTER
// --------------------------------------------

#define LP5814_CHIP_EN_MASK (0x01)
#define LP5814_CHIP_EN_ENABLE (0x01)
#define LP5814_CHIP_EN_DISABLE (0x00)

// --------------------------------------------
// MAX CURRENT REGISTER
// --------------------------------------------

#define LP5814_MAX_CURRENT_MASK (0x01)
#define LP5814_MAX_CURRENT_25_5MA (0x00)
#define LP5814_MAX_CURRENT_51MA (0x01)

// --------------------------------------------
// ENABLE CONTROL REGISTER
// --------------------------------------------

#define LP5814_OUT0_EN_MASK (0x01)
#define LP5814_OUT1_EN_MASK (0x02)
#define LP5814_OUT2_EN_MASK (0x04)
#define LP5814_OUT3_EN_MASK (0x08)
#define LP5814_OUTX_EN_SHIFT(ch) (ch)

// --------------------------------------------
// LED CHANNEL BIT MASK
// --------------------------------------------

#define LP5814_LED0 (1 << 0)                                                   // Bit 0: LED0 channel
#define LP5814_LED1 (1 << 1)                                                   // Bit 1: LED1 channel
#define LP5814_LED2 (1 << 2)                                                   // Bit 2: LED2 channel
#define LP5814_LED3 (1 << 3)                                                   // Bit 3: LED3 channel
#define LP5814_LED_ALL (LP5814_LED0 | LP5814_LED1 | LP5814_LED2 | LP5814_LED3) // All channels

// --------------------------------------------
// MAX CURRENT MODE
// --------------------------------------------

typedef enum {
    LP5814_MAX_CURRENT_25_5_MA = LP5814_MAX_CURRENT_25_5MA,
    LP5814_MAX_CURRENT_51_MA = LP5814_MAX_CURRENT_51MA
} lp5814_max_current_t;

// --------------------------------------------
// DEVICE HANDLE STRUCTURE
// --------------------------------------------

typedef struct lp5814_handle_t {
    i2c_master_dev_handle_t i2c_dev;
    uint8_t dev_addr;
    bool initialized;
} lp5814_handle_t;

// --------------------------------------------
// PUBLIC API FUNCTIONS
// --------------------------------------------

/**
 * @brief Initialize the LP5814 LED driver.
 *
 *        This function initializes the LP5814 device by setting up the I2C device
 *        handle, configuring the device address, and performing initial chip enable.
 *
 * @param i2c_bus      Pointer to the I2C master bus handle.
 * @param dev_addr     I2C device address (7-bit, typically 0x2C).
 * @param handle       Pointer to store the LP5814 handle.
 *
 * @return
 *        - ESP_OK on success.
 *        - ESP_ERR_INVALID_ARG if any parameter is NULL.
 *        - ESP_FAIL if I2C communication fails.
 */
esp_err_t lp5814_init(i2c_master_bus_handle_t *i2c_bus, uint8_t dev_addr, lp5814_handle_t *handle);

/**
 * @brief Deinitialize the LP5814 LED driver.
 *
 *        This function disables the chip and frees resources.
 *
 * @param handle       Pointer to the LP5814 handle.
 *
 * @return
 *        - ESP_OK on success.
 *        - ESP_ERR_INVALID_ARG if handle is NULL.
 */
esp_err_t lp5814_deinit(lp5814_handle_t *handle);

/**
 * @brief Enable or disable the LP5814 chip.
 *
 *        When disabled, the chip enters low-power standby mode.
 *
 * @param handle       Pointer to the LP5814 handle.
 * @param enable       true to enable, false to disable.
 *
 * @return
 *        - ESP_OK on success.
 *        - ESP_ERR_INVALID_ARG if handle is NULL.
 *        - ESP_FAIL if I2C communication fails.
 */
esp_err_t lp5814_set_chip_enable(lp5814_handle_t *handle, bool enable);

/**
 * @brief Set the maximum current mode for all LEDs.
 *
 *        The LP5814 supports two current ranges:
 *        - 25.5mA max: 0.1mA to 25.5mA in 256 steps
 *        - 51mA max: 0.2mA to 51mA in 256 steps
 *
 * @param handle       Pointer to the LP5814 handle.
 * @param mode         Maximum current mode.
 *
 * @return
 *        - ESP_OK on success.
 *        - ESP_ERR_INVALID_ARG if handle is NULL.
 *        - ESP_FAIL if I2C communication fails.
 */
esp_err_t lp5814_set_max_current(lp5814_handle_t *handle, lp5814_max_current_t mode);

/**
 * @brief Enable or disable LED output channels.
 *
 *        Each LED channel can be individually enabled or disabled.
 *        Multiple channels can be specified using bitwise OR (e.g., LP5814_LED0 | LP5814_LED2).
 *
 * @param handle       Pointer to the LP5814 handle.
 * @param led_mask     LED channel mask (LP5814_LED0 | LP5814_LED1 | LP5814_LED2 | LP5814_LED3).
 * @param enable       true to enable, false to disable.
 *
 * @return
 *        - ESP_OK on success.
 *        - ESP_ERR_INVALID_ARG if handle is NULL or led_mask is invalid.
 *        - ESP_FAIL if I2C communication fails.
 */
esp_err_t lp5814_set_led_enable(lp5814_handle_t *handle, uint32_t led_mask, bool enable);

/**
 * @brief
 *
 * @param handle
 * @param mode
 *
 * @return
 *        - ESP_OK on success.
 *        - ESP_ERR_INVALID_ARG if handle is NULL or led_mask is invalid.
 *        - ESP_FAIL if I2C communication fails.
 */
esp_err_t lp5814_set_led_dim_mode(lp5814_handle_t *handle, uint8_t mode);

/**
 * @brief
 *
 * @param handle
 * @param mode
 *
 * @return
 *        - ESP_OK on success.
 *        - ESP_ERR_INVALID_ARG if handle is NULL or led_mask is invalid.
 *        - ESP_FAIL if I2C communication fails.
 */
esp_err_t lp5814_set_led_engine_mode(lp5814_handle_t *handle, uint8_t mode);

/**
 * @brief
 *
 * @param handle
 * @param mode
 *
 * @return
 *        - ESP_OK on success.
 *        - ESP_ERR_INVALID_ARG if handle is NULL or led_mask is invalid.
 *        - ESP_FAIL if I2C communication fails.
 */
esp_err_t lp5814_set_led_update_param(lp5814_handle_t *handle);

/**
 * @brief Set dot current (analog brightness) for specific LEDs.
 *
 *        The dot current is an 8-bit value (0-255) that controls LED current
 *        in combination with the max current mode. This provides analog dimming.
 *        Multiple channels can be specified using bitwise OR.
 *
 * @param handle       Pointer to the LP5814 handle.
 * @param led_mask     LED channel mask (LP5814_LED0 | LP5814_LED1 | LP5814_LED2 | LP5814_LED3).
 * @param dc           Dot current value (0-255).
 *
 * @return
 *        - ESP_OK on success.
 *        - ESP_ERR_INVALID_ARG if handle is NULL, led_mask is invalid, or dc > 255.
 *        - ESP_FAIL if I2C communication fails.
 */
esp_err_t lp5814_set_led_dc(lp5814_handle_t *handle, uint32_t led_mask, uint8_t dc);

/**
 * @brief Set PWM duty cycle for specific LEDs.
 *
 *        The PWM is an 8-bit value (0-255) that controls LED brightness
 *        through pulse-width modulation. 0 = 0% duty cycle, 255 = 100% duty cycle.
 *        Multiple channels can be specified using bitwise OR.
 *
 * @param handle       Pointer to the LP5814 handle.
 * @param led_mask     LED channel mask (LP5814_LED0 | LP5814_LED1 | LP5814_LED2 | LP5814_LED3).
 * @param pwm          PWM value (0-255).
 *
 * @return
 *        - ESP_OK on success.
 *        - ESP_ERR_INVALID_ARG if handle is NULL, led_mask is invalid, or pwm > 255.
 *        - ESP_FAIL if I2C communication fails.
 */
esp_err_t lp5814_set_led_pwm(lp5814_handle_t *handle, uint32_t led_mask, uint8_t pwm);

/**
 * @brief Set both DC and PWM for specific LEDs in one call.
 *
 *        This is a convenience function that sets both the dot current and
 *        PWM value for LED channels. Multiple channels can be specified using bitwise OR.
 *
 * @param handle       Pointer to the LP5814 handle.
 * @param led_mask     LED channel mask (LP5814_LED0 | LP5814_LED1 | LP5814_LED2 | LP5814_LED3).
 * @param dc           Dot current value (0-255).
 * @param pwm          PWM value (0-255).
 *
 * @return
 *        - ESP_OK on success.
 *        - ESP_ERR_INVALID_ARG if handle is NULL, led_mask is invalid, or values > 255.
 *        - ESP_FAIL if I2C communication fails.
 */
esp_err_t lp5814_set_led_dc_pwm(lp5814_handle_t *handle, uint32_t led_mask, uint8_t dc, uint8_t pwm);

/**
 * @brief Calculate the actual LED current in mA.
 *
 *        This helper function calculates the actual LED current based on
 *        the max current mode and dot current value.
 *
 * @param mode         Maximum current mode.
 * @param dc           Dot current value (0-255).
 *
 * @return             LED current in mA.
 */
float lp5814_calculate_current(lp5814_max_current_t mode, uint8_t dc);

#ifdef __cplusplus
}
#endif

#endif // LP5814_H