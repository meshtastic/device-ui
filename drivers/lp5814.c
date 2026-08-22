/*
 * SPDX-FileCopyrightText: 2026
 *
 * SPDX-License-Identifier: MIT
 */

#include "lp5814.h"
#include <string.h>

// --------------------------------------------
// PRIVATE FUNCTIONS
// --------------------------------------------

/**
 * @brief Write a single byte to a LP5814 register.
 *
 * @param handle   Pointer to the LP5814 handle.
 * @param reg      Register address.
 * @param data     Data byte to write.
 *
 * @return
 *        - ESP_OK on success.
 *        - ESP_ERR_INVALID_ARG if handle is NULL.
 *        - ESP_FAIL if I2C communication fails.
 */
static esp_err_t lp5814_write_register(lp5814_handle_t *handle, uint8_t reg, uint8_t data)
{
    if (handle == NULL || !handle->initialized) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t buffer[2];
    buffer[0] = reg;
    buffer[1] = data;

    return i2c_master_transmit(handle->i2c_dev, buffer, sizeof(buffer), -1);
}

/**
 * @brief Read a single byte from a LP5814 register.
 *
 * @param handle   Pointer to the LP5814 handle.
 * @param reg      Register address.
 * @param data     Pointer to store the read data.
 *
 * @return
 *        - ESP_OK on success.
 *        - ESP_ERR_INVALID_ARG if handle or data is NULL.
 *        - ESP_FAIL if I2C communication fails.
 */
static esp_err_t lp5814_read_register(lp5814_handle_t *handle, uint8_t reg, uint8_t *data)
{
    if (handle == NULL || data == NULL || !handle->initialized) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = i2c_master_transmit_receive(handle->i2c_dev, &reg, 1, data, 1, -1);
    return ret;
}

/**
 * @brief Modify bits in a register while preserving other bits.
 *
 * @param handle   Pointer to the LP5814 handle.
 * @param reg      Register address.
 * @param mask     Bit mask for bits to modify.
 * @param data     Data to write to masked bits.
 *
 * @return
 *        - ESP_OK on success.
 *        - ESP_ERR_INVALID_ARG if handle is NULL.
 *        - ESP_FAIL if I2C communication fails.
 */
static esp_err_t lp5814_modify_register(lp5814_handle_t *handle, uint8_t reg, uint8_t mask, uint8_t data)
{
    uint8_t current_value;
    esp_err_t ret = lp5814_read_register(handle, reg, &current_value);
    if (ret != ESP_OK) {
        return ret;
    }

    uint8_t new_value = (current_value & ~mask) | (data & mask);
    return lp5814_write_register(handle, reg, new_value);
}

/**
 * @brief Get DC register address for a specific LED.
 *
 * @param led_mask LED channel mask (bit position).
 *
 * @return         Register address.
 */
static inline uint8_t lp5814_get_dc_register(uint32_t led_mask)
{
    // Extract bit position (0-3) from mask
    uint8_t led_num = 0;
    switch (led_mask) {
    case LP5814_LED0:
        led_num = 0;
        break;
    case LP5814_LED1:
        led_num = 1;
        break;
    case LP5814_LED2:
        led_num = 2;
        break;
    case LP5814_LED3:
        led_num = 3;
        break;
    default:
        led_num = 0;
        break;
    }
    return LP5814_REG_LED0_DC + led_num;
}

/**
 * @brief Get PWM register address for a specific LED.
 *
 * @param led_mask LED channel mask (bit position).
 *
 * @return         Register address.
 */
static inline uint8_t lp5814_get_pwm_register(uint32_t led_mask)
{
    // Extract bit position (0-3) from mask
    uint8_t led_num = 0;
    switch (led_mask) {
    case LP5814_LED0:
        led_num = 0;
        break;
    case LP5814_LED1:
        led_num = 1;
        break;
    case LP5814_LED2:
        led_num = 2;
        break;
    case LP5814_LED3:
        led_num = 3;
        break;
    default:
        led_num = 0;
        break;
    }
    return LP5814_REG_LED0_PWM + led_num;
}

// --------------------------------------------
// PUBLIC API IMPLEMENTATION
// --------------------------------------------

esp_err_t lp5814_init(i2c_master_bus_handle_t *i2c_bus, uint8_t dev_addr, lp5814_handle_t *handle)
{
    if (i2c_bus == NULL || handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // Configure I2C device
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = dev_addr,
        .scl_speed_hz = 100000, // I2C max speed is 100kHz
    };

    esp_err_t ret = i2c_master_bus_add_device(*i2c_bus, &dev_cfg, &handle->i2c_dev);
    if (ret != ESP_OK) {
        return ret;
    }

    handle->dev_addr = dev_addr;
    handle->initialized = true;

    // Initialize device: enable chip and set default values
    ret = lp5814_write_register(handle, LP5814_REG_DEVICE_CONFIG0, LP5814_CHIP_EN_ENABLE);
    if (ret != ESP_OK) {
        handle->initialized = false;
        i2c_master_bus_rm_device(handle->i2c_dev);
        return ret;
    }

    // Set default max current to 25.5mA
    ret = lp5814_set_max_current(handle, LP5814_MAX_CURRENT_51MA);
    if (ret != ESP_OK) {
        handle->initialized = false;
        i2c_master_bus_rm_device(handle->i2c_dev);
        return ret;
    }

    // Disable all LED outputs initially
    ret = lp5814_write_register(handle, LP5814_REG_ENABLE_CONTROL, 0x00);
    if (ret != ESP_OK) {
        handle->initialized = false;
        i2c_master_bus_rm_device(handle->i2c_dev);
        return ret;
    }

    lp5814_set_led_dim_mode(handle, 0x4E);
    lp5814_set_led_engine_mode(handle, 0xF0);
    lp5814_set_led_dc(handle, LP5814_LED_ALL, 200);
    lp5814_set_led_enable(handle, LP5814_LED_ALL, true);
    lp5814_set_led_update_param(handle);
    lp5814_set_led_pwm(handle, LP5814_LED_ALL, 0);

    return ESP_OK;
}

esp_err_t lp5814_deinit(lp5814_handle_t *handle)
{
    if (handle == NULL || !handle->initialized) {
        return ESP_ERR_INVALID_ARG;
    }

    // Disable all LEDs and chip
    lp5814_write_register(handle, LP5814_REG_ENABLE_CONTROL, 0x00);
    lp5814_write_register(handle, LP5814_REG_DEVICE_CONFIG0, LP5814_CHIP_EN_DISABLE);

    // Remove I2C device
    esp_err_t ret = i2c_master_bus_rm_device(handle->i2c_dev);

    handle->initialized = false;
    return ret;
}

esp_err_t lp5814_set_chip_enable(lp5814_handle_t *handle, bool enable)
{
    if (handle == NULL || !handle->initialized) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t value = enable ? LP5814_CHIP_EN_ENABLE : LP5814_CHIP_EN_DISABLE;
    return lp5814_write_register(handle, LP5814_REG_DEVICE_CONFIG0, value);
}

esp_err_t lp5814_set_max_current(lp5814_handle_t *handle, lp5814_max_current_t mode)
{
    if (handle == NULL || !handle->initialized) {
        return ESP_ERR_INVALID_ARG;
    }

    return lp5814_modify_register(handle, LP5814_REG_MAX_CURRENT, LP5814_MAX_CURRENT_MASK, (uint8_t)mode);
}

esp_err_t lp5814_set_led_enable(lp5814_handle_t *handle, uint32_t led_mask, bool enable)
{
    if (handle == NULL || !handle->initialized || led_mask == 0 || (led_mask & ~0x0F)) {
        return ESP_ERR_INVALID_ARG;
    }

    // led_mask contains bits 0-3 for LEDs 0-3, which can be used directly
    uint8_t mask = (uint8_t)led_mask;
    uint8_t value = enable ? mask : 0;

    return lp5814_modify_register(handle, LP5814_REG_ENABLE_CONTROL, mask, value);
}

esp_err_t lp5814_set_led_dim_mode(lp5814_handle_t *handle, uint8_t mode)
{
    if (handle == NULL || !handle->initialized) {
        return ESP_ERR_INVALID_ARG;
    }

    return lp5814_write_register(handle, LP5814_REG_DIM_MODE, mode);
}

esp_err_t lp5814_set_led_engine_mode(lp5814_handle_t *handle, uint8_t mode)
{
    if (handle == NULL || !handle->initialized) {
        return ESP_ERR_INVALID_ARG;
    }

    return lp5814_write_register(handle, LP5814_REG_ENGINE_MODE, mode);
}

esp_err_t lp5814_set_led_update_param(lp5814_handle_t *handle)
{
    if (handle == NULL || !handle->initialized) {
        return ESP_ERR_INVALID_ARG;
    }

    return lp5814_write_register(handle, LP5814_REG_UPDATE, 0x55);
}

esp_err_t lp5814_set_led_dc(lp5814_handle_t *handle, uint32_t led_mask, uint8_t dc)
{
    if (handle == NULL || !handle->initialized || led_mask == 0 || (led_mask & ~0x0F)) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;

    // Iterate through each LED bit in the mask
    for (uint8_t led = 0; led < 4; led++) {
        if (led_mask & (1 << led)) {
            uint8_t reg = LP5814_REG_LED0_DC + led;
            ret = lp5814_write_register(handle, reg, dc);
            if (ret != ESP_OK) {
                return ret;
            }
        }
    }

    return ret;
}

esp_err_t lp5814_set_led_pwm(lp5814_handle_t *handle, uint32_t led_mask, uint8_t pwm)
{
    if (handle == NULL || !handle->initialized || led_mask == 0 || (led_mask & ~0x0F)) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;

    // Iterate through each LED bit in the mask
    for (uint8_t led = 0; led < 4; led++) {
        if (led_mask & (1 << led)) {
            uint8_t reg = LP5814_REG_LED0_PWM + led;
            ret = lp5814_write_register(handle, reg, pwm);
            if (ret != ESP_OK) {
                return ret;
            }
        }
    }

    return ret;
}

esp_err_t lp5814_set_led_dc_pwm(lp5814_handle_t *handle, uint32_t led_mask, uint8_t dc, uint8_t pwm)
{
    if (handle == NULL || !handle->initialized || led_mask == 0 || (led_mask & ~0x0F)) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = lp5814_set_led_dc(handle, led_mask, dc);
    if (ret != ESP_OK) {
        return ret;
    }

    return lp5814_set_led_pwm(handle, led_mask, pwm);
}

float lp5814_calculate_current(lp5814_max_current_t mode, uint8_t dc)
{
    if (mode == LP5814_MAX_CURRENT_51_MA) {
        // 51mA max: 0.2mA to 51mA in 256 steps
        // Current = 0.2 + (dc * 0.2)
        return 0.2f + (dc * 0.2f);
    } else {
        // 25.5mA max: 0.1mA to 25.5mA in 256 steps
        // Current = 0.1 + (dc * 0.1)
        return 0.1f + (dc * 0.1f);
    }
}