/*
* The MIT License (MIT)
*
* Author: Assam Boudjelthia
* Copyright (c) 2018 Rohm Semiconductor.
*
* This program and the accompanying materials are made available under the
* terms of the The MIT License which is available at
* https://opensource.org/licenses/MIT.
*
* SPDX-License-Identifier: MIT
*/

#include <unistd.h>
#include <math.h>
#include "kxtj3.h"
#include "upm_utilities.h"

#define SW_RESET_MAX_LOOP_COUNT 10
#define SW_RESET_READ_WAIT_MICRO_S 50000

#define SELF_TEST_SAMPLE_COUNT 10
#define SELF_TEST_DIFFERENCE_THRESHOLD 0.5f

#define DATA_BUFFER_LENGTH 6

/**
 * @brief Acceleration steps in (g) for each range setting.
 * Used to calculate acceleration_scale to convert
 * raw data to readable acceleration data.
 */
#define RANGE_2G_8BIT_STEP 0.016f
#define RANGE_4G_8BIT_STEP 0.031f
#define RANGE_8G_8BIT_STEP 0.0625f
#define RANGE_16G_8BIT_STEP 0.125f

#define RANGE_2G_12BIT_STEP 0.001f
#define RANGE_4G_12BIT_STEP 0.002f
#define RANGE_8G_12BIT_STEP 0.0039f
#define RANGE_16G_12BIT_STEP 0.0078f

#define RANGE_8G_14BIT_STEP 0.00098f
#define RANGE_16G_14BIT_STEP 0.00195f
#define EARTH_GRAVITY 9.81f

/**
 * @brief Map of ODR register values to ODR in Hz
 * used to calculate sampling time in seconds
 */
struct odr_map_t
{
    uint8_t odr_reg_bit;
    float odr_in_Hz;
};

/**
 * @brief ODR register values maping with ODR in Hz
 */
const struct odr_map_t odr_map_in_Hz[] = {
    {KXTJ3_ODR_0P781, 0.781f},
    {KXTJ3_ODR_1P563, 1.563f},
    {KXTJ3_ODR_3P125, 3.125f},
    {KXTJ3_ODR_6P25, 6.25f},
    {KXTJ3_ODR_12P5, 12.5f},
    {KXTJ3_ODR_25, 25.0f},
    {KXTJ3_ODR_50, 50.0f},
    {KXTJ3_ODR_100, 100.0f},
    {KXTJ3_ODR_200, 200.0f},
    {KXTJ3_ODR_400, 400.0f},
    {KXTJ3_ODR_800, 800.0f},
    {KXTJ3_ODR_1600, 1600.0f}};
/**
 * @brief ODR register values maping with ODR in Hz for
 * wake-up function
 */
const struct odr_map_t odr_map_in_Hz_wakeup[] = {
    {KXTJ3_ODR_WAKEUP_0P781, 0.781f},
    {KXTJ3_ODR_WAKEUP_1P563, 1.563f},
    {KXTJ3_ODR_WAKEUP_3P125, 3.125f},
    {KXTJ3_ODR_WAKEUP_6P25, 6.25f},
    {KXTJ3_ODR_WAKEUP_12P5, 12.5f},
    {KXTJ3_ODR_WAKEUP_25, 25.0f},
    {KXTJ3_ODR_WAKEUP_50, 50.0f},
    {KXTJ3_ODR_WAKEUP_100, 100.0f}};

/**
 * @brief Coordinates structure
 */
struct Coordinates
{
    float x, y, z;
};

/**
@brief Inits the I2C connections and returns status of initialization

@param dev The sensor context
@param bus I2C bus number
@param addr I2C addr of the sensor
@return true if initialization successful or false for failure
*/
static bool kxtj3_check_mraa_i2c_connection(kxtj3_context dev, int bus, uint8_t addr);

/**
@brief Checks if the sensor WHO_AM_I value is correct

@param dev The sensor context
@return true if value correct, or false if mismatch
*/
static bool kxtj3_check_who_am_i(kxtj3_context dev);

/**
@brief Calculates the ODR sample time from an ODR register value

@param odr One of KXTJ3_ODR_T values of ODR register configurations
@return the float time value
*/
static float kxtj3_odr_val_to_sec(KXTJ3_ODR_T odr);

/**
@brief Calculates the ODR sample time from an ODR register value for wake-up function

@param odr One of KXTJ3_ODR_WAKEUP_T values of ODR register configurations for wake-up
@return the float time value
*/
static float kxtj3_odr_val_to_sec_wakeup(KXTJ3_ODR_WAKEUP_T odr);

/**
@brief Sets the sensor default values for ODR, resolution (with its scale),
G range (both normal and wake-up modes)

@param dev The sensor context
*/
static void kxtj3_set_default_values(const kxtj3_context dev);

/**
@brief Read the value of a provided register

@param dev The sensor context
@param reg The register address to read from
@param data A pointer to variable for storing the value read
@return A UPM result
*/
static upm_result_t kxtj3_read_register(const kxtj3_context dev, uint8_t reg, uint8_t *data);

/**
@brief Read the values starting from a provided register, of specific length

@param dev The sensor context
@param reg The register address to start reading from
@param data A pointer to variable for storing the value read
@param len The number of bytes to read
@return A UPM result
*/
static upm_result_t kxtj3_read_registers(const kxtj3_context dev, uint8_t reg, uint8_t *data, int len);

/**
@brief Writes a value to a provided register

@param dev The sensor context
@param reg The register address to write to
@param val byte of data to write
@return A UPM result
*/
static upm_result_t kxtj3_write_register(const kxtj3_context dev, uint8_t reg, uint8_t val);

/**
@brief Sets a specific bit on in a provided register

@param dev The sensor context
@param reg register to write into
@param bit_mask The bit to set, as a register mask
@return A UPM result
*/
static upm_result_t kxtj3_set_bit_on(const kxtj3_context dev, uint8_t reg, uint8_t bit_mask);

/**
@brief Clear a specific bit (set off) in a provided register

@param dev The sensor context
@param reg register address to write into
@param bit_mask The bit to set, as a register mask
@return A UPM result
*/
static upm_result_t kxtj3_set_bit_off(const kxtj3_context dev, uint8_t reg, uint8_t bit_mask);

/**
@brief Sets a register value or its bits according to a provided mask

@param dev The sensor context
@param reg The register address to write to
@param val byte data to write
@param bit_mask The bits or register mask
@return A UPM result
*/
static upm_result_t kxtj3_set_bits_with_mask(const kxtj3_context dev, uint8_t reg, uint8_t val, uint8_t bit_mask);

/**
@brief Checks whether a given G range setting uses 14-bit mode

@param g_range One of KXTJ3_G_RANGE_T value for available acceleration settings
@return true if range is 14-bit based, false otherwise
*/
static bool kxtj3_is_14_bit_range(KXTJ3_G_RANGE_T g_range_mode);

/**
@brief Maps the acceleration_scale (that is used to calculate the acceleration data in g unit)
with the G range and resolution mode. Changes the acceleration_scale value in sensor context.

@param dev The sensor context
@param g_range The G range setting, one of KXTJ3_G_RANGE_T values
*/
static void kxtj3_map_g_range_to_resolution(kxtj3_context dev, KXTJ3_G_RANGE_T g_range);
/**
@brief Calculates the average of coordinates for a sample of data (SELF_TEST_SAMPLE_COUNT).
This is used by the self-test functionality.

@param dev The sensor context
@return Coordinates struct that contains value of x, y and z
*/
static struct Coordinates kxtj3_get_sample_averaged_data(kxtj3_context dev);

/**
@brief Check whether the self-test acceleration data difference is within the permitted threshold (0.5g)

@param before The Coordinates struct before the self-test
@param during The Coordinates struct of the self-test
@return true if difference is below threshold, false otherwise
*/
static bool kxtj3_check_self_test_difference(struct Coordinates before, struct Coordinates during);

/**
@brief Checks the digital communication register (DCST_RESP) register value with an expected value

@param dev The sensor context
@param expected_val The expected byte value of the register
@return true if values match, false otherwise.
*/
static bool kxtj3_check_digital_communication_reg_value(kxtj3_context dev, uint8_t expected_val);

/**
@brief Gets the count value from a given time (in seconds) for the wake-up function.
Used by the wake-up motion counter and non-activity counter before another wake-up functions.

@param dev The sensor context
@param time_sec Time in seconds to be converted
@return the count value as a uint8_t
*/
static uint8_t kxtj3_get_wakeup_count_from_time_sec(kxtj3_context dev, float time_sec);

/**
@brief Gets the count value from a given acceleration threshold (in g) for the wake-up function.
Used by wake-up threshold counter functionality.

@param dev The sensor context
@param g_threshold acceleration value in g to be converted
@return the count value as a uint16_t (expected range up to 4096)
*/
static uint16_t kxtj3_get_wakeup_threshold_count_from_g(kxtj3_context dev, float g_threshold);

// Register Read/Write helper functions
static upm_result_t kxtj3_read_register(const kxtj3_context dev, uint8_t reg, uint8_t *data)
{
    int value = mraa_i2c_read_byte_data(dev->i2c, reg);
    if (value == -1)
    {
        printf("%s: mraa_i2c_read_byte_data() failed.\n", __FUNCTION__);
        return UPM_ERROR_OPERATION_FAILED;
    }

    *data = (uint8_t)value;
    return UPM_SUCCESS;
}

static upm_result_t kxtj3_read_registers(const kxtj3_context dev, uint8_t reg, uint8_t *data, int len)
{
    if (mraa_i2c_read_bytes_data(dev->i2c, reg, data, len) != (int)len)
        return UPM_ERROR_OPERATION_FAILED;

    return UPM_SUCCESS;
}

static upm_result_t kxtj3_write_register(const kxtj3_context dev, uint8_t reg, uint8_t val)
{
    if (mraa_i2c_write_byte_data(dev->i2c, val, reg) != MRAA_SUCCESS)
    {
        printf("%s: mraa_i2c_write_byte_data() failed.\n", __FUNCTION__);
        return UPM_ERROR_OPERATION_FAILED;
    }

    return UPM_SUCCESS;
}

static upm_result_t kxtj3_set_bit_on(const kxtj3_context dev, uint8_t reg, uint8_t bit_mask)
{
    uint8_t reg_value = 0;
    if (kxtj3_read_register(dev, reg, &reg_value) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    reg_value |= bit_mask;
    return kxtj3_write_register(dev, reg, reg_value);
}

static upm_result_t kxtj3_set_bit_off(const kxtj3_context dev, uint8_t reg, uint8_t bit_mask)
{
    uint8_t reg_value = 0;
    if (kxtj3_read_register(dev, reg, &reg_value) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    reg_value &= ~bit_mask;
    return kxtj3_write_register(dev, reg, reg_value);
}

static upm_result_t kxtj3_set_bits_with_mask(const kxtj3_context dev, uint8_t reg, uint8_t val, uint8_t bit_mask)
{
    uint8_t reg_val = 0;
    if (kxtj3_read_register(dev, reg, &reg_val) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    reg_val &= ~bit_mask;
    reg_val |= val;
    return kxtj3_write_register(dev, reg, reg_val);
}
// End of register Read/Write helper functions

static bool kxtj3_check_mraa_i2c_connection(kxtj3_context dev, int bus, uint8_t addr)
{
    if (mraa_init() != MRAA_SUCCESS)
    {
        printf("%s: mraa_init() failed.\n", __FUNCTION__);
        kxtj3_close(dev);
        return false;
    }

    if (!(dev->i2c = mraa_i2c_init(bus)))
    {
        printf("%s: mraa_i2c_init() failed.\n", __FUNCTION__);
        kxtj3_close(dev);
        return false;
    }

    if (mraa_i2c_address(dev->i2c, addr))
    {
        printf("%s: mraa_i2c_address() failed.\n", __FUNCTION__);
        kxtj3_close(dev);
        return false;
    }

    return true;
}

static bool kxtj3_check_who_am_i(kxtj3_context dev)
{
    uint8_t who_am_i;
    kxtj3_get_who_am_i(dev, &who_am_i);
    if (who_am_i != KXTJ3_WHO_AM_I_WIA_ID)
    {
        printf("%s: Wrong WHO AM I received, expected: 0x%x | got: 0x%x\n", __FUNCTION__,
               KXTJ3_WHO_AM_I_WIA_ID, who_am_i);
        kxtj3_close(dev);
        return false;
    }
    return true;
}

static float kxtj3_odr_val_to_sec(KXTJ3_ODR_T odr)
{
    for (size_t i = 0; i < (sizeof(odr_map_in_Hz) / sizeof(struct odr_map_t)); i++)
        if (odr == odr_map_in_Hz[i].odr_reg_bit)
            return (1 / odr_map_in_Hz[i].odr_in_Hz);

    return -1;
}

static float kxtj3_odr_val_to_sec_wakeup(KXTJ3_ODR_WAKEUP_T odr)
{
    for (size_t i = 0; i < (sizeof(odr_map_in_Hz_wakeup) / sizeof(struct odr_map_t)); i++)
        if (odr == odr_map_in_Hz_wakeup[i].odr_reg_bit)
            return (1 / odr_map_in_Hz_wakeup[i].odr_in_Hz);

    return -1;
}

static void kxtj3_set_default_values(const kxtj3_context dev)
{
    dev->g_range_mode = KXTJ3_RANGE_2G;
    dev->acceleration_scale = RANGE_2G_8BIT_STEP;
    dev->res_mode = LOW_RES;
    dev->odr = KXTJ3_ODR_50;
    dev->odr_in_sec = kxtj3_odr_val_to_sec(dev->odr);
    dev->odr_wakeup = KXTJ3_ODR_WAKEUP_0P781;
    dev->odr_in_sec_wakeup = kxtj3_odr_val_to_sec_wakeup(dev->odr_wakeup);
}

kxtj3_context kxtj3_init(int bus, uint8_t addr)
{
    kxtj3_context dev = (kxtj3_context)malloc(sizeof(struct _kxtj3_context));
    if (!dev)
        return NULL;

    dev->i2c = NULL;
    dev->interrupt_pin = NULL;

    if (!kxtj3_check_mraa_i2c_connection(dev, bus, addr))
        return NULL;

    if (!kxtj3_check_who_am_i(dev))
        return NULL;

    kxtj3_set_default_values(dev);

    kxtj3_set_odr_wakeup_function(dev, dev->odr_wakeup);
    kxtj3_sensor_init(dev, dev->odr, dev->res_mode, dev->g_range_mode);

    return dev;
}

upm_result_t kxtj3_sensor_init(const kxtj3_context dev, KXTJ3_ODR_T odr, KXTJ3_RESOLUTION_T res, KXTJ3_G_RANGE_T g_range)
{
    assert(dev != NULL);
    if (kxtj3_set_sensor_standby(dev) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    if (kxtj3_set_odr(dev, odr) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    uint8_t g_range_with_res = 0;
    if (res)
        g_range_with_res |= KXTJ3_CTRL_REG1_RES;

    g_range_with_res |= (g_range & KXTJ3_CTRL_REG1_GSEL_MASK);
    if (kxtj3_set_bits_with_mask(dev, KXTJ3_CTRL_REG1, g_range_with_res,
                                 KXTJ3_CTRL_REG1_RES | KXTJ3_CTRL_REG1_GSEL_MASK) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    dev->g_range_mode = g_range;
    dev->res_mode = res;
    kxtj3_map_g_range_to_resolution(dev, dev->g_range_mode);

    if (kxtj3_set_sensor_active(dev) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    return UPM_SUCCESS;
}

upm_result_t kxtj3_get_who_am_i(const kxtj3_context dev, uint8_t *data)
{
    assert(dev != NULL);
    return kxtj3_read_register(dev, KXTJ3_WHO_AM_I, data);
}

void kxtj3_close(kxtj3_context dev)
{
    assert(dev != NULL);

    if (dev->i2c)
    {
        mraa_i2c_stop(dev->i2c);
    }

    if (dev->interrupt_pin)
        kxtj3_uninstall_isr(dev);

    free(dev);
}

upm_result_t kxtj3_set_sensor_active(const kxtj3_context dev)
{
    assert(dev != NULL);
    return kxtj3_set_bit_on(dev, KXTJ3_CTRL_REG1, KXTJ3_CTRL_REG1_PC);
}

upm_result_t kxtj3_set_sensor_standby(const kxtj3_context dev)
{
    assert(dev != NULL);
    return kxtj3_set_bit_off(dev, KXTJ3_CTRL_REG1, KXTJ3_CTRL_REG1_PC);
}

static void kxtj3_map_g_range_to_resolution(kxtj3_context dev, KXTJ3_G_RANGE_T g_range)
{
    if (dev->res_mode == LOW_RES)
        switch (g_range)
        {
        case KXTJ3_RANGE_2G:
            dev->acceleration_scale = RANGE_2G_8BIT_STEP;
            break;
        case KXTJ3_RANGE_4G:
            dev->acceleration_scale = RANGE_4G_8BIT_STEP;
            break;
        case KXTJ3_RANGE_8G:
            dev->acceleration_scale = RANGE_8G_8BIT_STEP;
            break;
        case KXTJ3_RANGE_16G:
        case KXTJ3_RANGE_16G_2:
        case KXTJ3_RANGE_16G_3:
            dev->acceleration_scale = RANGE_16G_8BIT_STEP;
            break;

        case KXTJ3_RANGE_8G_14:
            kxtj3_set_resolution(dev, HIGH_RES);
            dev->acceleration_scale = RANGE_8G_14BIT_STEP;
            break;
        case KXTJ3_RANGE_16G_14:
            kxtj3_set_resolution(dev, HIGH_RES);
            dev->acceleration_scale = RANGE_16G_14BIT_STEP;
            break;
        }
    else
        switch (g_range)
        {
        case KXTJ3_RANGE_2G:
            dev->acceleration_scale = RANGE_2G_12BIT_STEP;
            break;
        case KXTJ3_RANGE_4G:
            dev->acceleration_scale = RANGE_4G_12BIT_STEP;
            break;
        case KXTJ3_RANGE_8G:
            dev->acceleration_scale = RANGE_8G_12BIT_STEP;
            break;
        case KXTJ3_RANGE_16G:
        case KXTJ3_RANGE_16G_2:
        case KXTJ3_RANGE_16G_3:
            dev->acceleration_scale = RANGE_16G_12BIT_STEP;
            break;

        case KXTJ3_RANGE_8G_14:
            dev->acceleration_scale = RANGE_8G_14BIT_STEP;
            break;
        case KXTJ3_RANGE_16G_14:
            dev->acceleration_scale = RANGE_16G_14BIT_STEP;
            break;
        }
}

upm_result_t kxtj3_set_g_range(const kxtj3_context dev, KXTJ3_G_RANGE_T g_range)
{
    assert(dev != NULL);

    if (kxtj3_set_bits_with_mask(dev, KXTJ3_CTRL_REG1, g_range, KXTJ3_CTRL_REG1_GSEL_MASK) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    dev->g_range_mode = g_range;
    kxtj3_map_g_range_to_resolution(dev, g_range);
    return UPM_SUCCESS;
}

upm_result_t kxtj3_set_resolution(const kxtj3_context dev, KXTJ3_RESOLUTION_T resolution)
{
    assert(dev != NULL);

    if (resolution == HIGH_RES)
    {
        if (kxtj3_set_bit_on(dev, KXTJ3_CTRL_REG1, KXTJ3_CTRL_REG1_RES) != UPM_SUCCESS)
            return UPM_ERROR_OPERATION_FAILED;
    }
    else
    {
        if (kxtj3_set_bit_off(dev, KXTJ3_CTRL_REG1, KXTJ3_CTRL_REG1_RES != UPM_SUCCESS))
            return UPM_ERROR_OPERATION_FAILED;
    }

    dev->res_mode = resolution;
    kxtj3_map_g_range_to_resolution(dev, dev->g_range_mode);
    return UPM_SUCCESS;
}

upm_result_t kxtj3_set_odr(const kxtj3_context dev, KXTJ3_ODR_T odr)
{
    assert(dev != NULL);
    if (kxtj3_set_bits_with_mask(dev, KXTJ3_DATA_CTRL_REG, odr, KXTJ3_DATA_CTRL_REG_OSA_MASK) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    dev->odr = odr;
    dev->odr_in_sec = kxtj3_odr_val_to_sec(odr);

    return UPM_SUCCESS;
}

upm_result_t kxtj3_set_odr_wakeup_function(const kxtj3_context dev, KXTJ3_ODR_WAKEUP_T odr)
{
    assert(dev != NULL);
    if (kxtj3_set_bits_with_mask(dev, KXTJ3_CTRL_REG2, odr, KXTJ3_CTRL_REG2_OWUF_MASK) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;
    dev->odr_wakeup = odr;
    dev->odr_in_sec_wakeup = kxtj3_odr_val_to_sec_wakeup(odr);

    return UPM_SUCCESS;
}

static bool kxtj3_check_digital_communication_reg_value(kxtj3_context dev, uint8_t expected_val)
{
    uint8_t dcst_reg = 0;
    if (kxtj3_read_register(dev, KXTJ3_DCST_RESP, &dcst_reg) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    if (dcst_reg != expected_val)
        return false;

    return true;
}

upm_result_t kxtj3_self_test_digital_communication(kxtj3_context dev)
{
    assert(dev != NULL);
    if (!kxtj3_check_digital_communication_reg_value(dev, KXTJ3_DCST_RESP_DCSTR_BEFORE))
        return UPM_ERROR_OPERATION_FAILED;

    if (kxtj3_set_bit_on(dev, KXTJ3_CTRL_REG2, KXTJ3_CTRL_REG2_DCST) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    if (!kxtj3_check_digital_communication_reg_value(dev, KXTJ3_DCST_RESP_DCSTR_AFTER))
        return UPM_ERROR_OPERATION_FAILED;

    if (!kxtj3_check_digital_communication_reg_value(dev, KXTJ3_DCST_RESP_DCSTR_BEFORE))
        return UPM_ERROR_OPERATION_FAILED;

    return UPM_SUCCESS;
}

static struct Coordinates kxtj3_get_sample_averaged_data(kxtj3_context dev)
{
    struct Coordinates coordinates_averaged_sample;
    coordinates_averaged_sample.x = 0.0f;
    coordinates_averaged_sample.y = 0.0f;
    coordinates_averaged_sample.z = 0.0f;

    float wait_time = kxtj3_get_acceleration_sampling_period(dev) * SECOND_IN_MICRO_S;
    float x, y, z;
    for (size_t i = 0; i < SELF_TEST_SAMPLE_COUNT; i++)
    {
        kxtj3_get_acceleration_data(dev, &x, &y, &z);
        coordinates_averaged_sample.x += fabs((x / EARTH_GRAVITY));
        coordinates_averaged_sample.y += fabs((y / EARTH_GRAVITY));
        coordinates_averaged_sample.z += fabs((z / EARTH_GRAVITY));
        upm_delay_us(wait_time);
    }

    coordinates_averaged_sample.x /= SELF_TEST_SAMPLE_COUNT;
    coordinates_averaged_sample.y /= SELF_TEST_SAMPLE_COUNT;
    coordinates_averaged_sample.z /= SELF_TEST_SAMPLE_COUNT;

    return coordinates_averaged_sample;
}

static bool kxtj3_check_self_test_difference(struct Coordinates before, struct Coordinates during)
{
    struct Coordinates difference;
    difference.x = fabs(before.x - during.x);
    difference.y = fabs(before.y - during.y);
    difference.z = fabs(before.z - during.z);

    if (difference.x > SELF_TEST_DIFFERENCE_THRESHOLD)
    {
        printf("%s: X-asix FAILED, change on X difference: %.2f\n", __FUNCTION__, difference.x);
        return false;
    }

    if (difference.y > SELF_TEST_DIFFERENCE_THRESHOLD)
    {
        printf("%s: Y-asix FAILED, change on Y difference: %.2f\n", __FUNCTION__, difference.y);
        return false;
    }

    if (difference.z > SELF_TEST_DIFFERENCE_THRESHOLD)
    {
        printf("%s: Z-asix FAILED, change on Z difference: %.2f\n", __FUNCTION__, difference.z);
        return false;
    }

    return true;
}

upm_result_t kxtj3_sensor_self_test(kxtj3_context dev)
{
    assert(dev != NULL);

    struct Coordinates coordinates_before_test, coordinates_during_test;
    coordinates_before_test = kxtj3_get_sample_averaged_data(dev);

    uint8_t stpol_val;
    kxtj3_read_register(dev, KXTJ3_INT_CTRL_REG1, &stpol_val);

    kxtj3_set_sensor_standby(dev);
    kxtj3_set_bit_on(dev, KXTJ3_INT_CTRL_REG1, KXTJ3_INT_CTRL_REG1_STPOL);
    kxtj3_write_register(dev, KXTJ3_SELF_TEST, KXTJ3_SELF_TEST_MEMS_TEST_ENABLE);
    kxtj3_set_bit_off(dev, KXTJ3_INT_CTRL_REG1, KXTJ3_INT_CTRL_REG1_STPOL);
    kxtj3_set_sensor_active(dev);

    coordinates_during_test = kxtj3_get_sample_averaged_data(dev);
    kxtj3_write_register(dev, KXTJ3_SELF_TEST, KXTJ3_SELF_TEST_MEMS_TEST_DISABLE);

    if (!kxtj3_check_self_test_difference(coordinates_before_test, coordinates_during_test))
        return UPM_ERROR_OPERATION_FAILED;

    kxtj3_set_sensor_standby(dev);
    if (kxtj3_self_test_digital_communication(dev) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;
    return kxtj3_set_sensor_active(dev);
}

upm_result_t kxtj3_sensor_software_reset(const kxtj3_context dev)
{
    assert(dev != NULL);
    if (kxtj3_set_bit_on(dev, KXTJ3_CTRL_REG2, KXTJ3_CTRL_REG2_SRST) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    uint8_t ctrl_reg2_data = 0;
    kxtj3_read_register(dev, KXTJ3_CTRL_REG2, &ctrl_reg2_data);

    uint8_t srst_counter = 0;
    while ((ctrl_reg2_data & KXTJ3_CTRL_REG2_SRST) != 0x00 && srst_counter < SW_RESET_MAX_LOOP_COUNT)
    {
        upm_delay_us(SW_RESET_READ_WAIT_MICRO_S);
        kxtj3_read_register(dev, KXTJ3_CTRL_REG2, &ctrl_reg2_data);
        srst_counter++;
    }

    if (srst_counter == SW_RESET_MAX_LOOP_COUNT)
        return UPM_ERROR_OPERATION_FAILED;

    return UPM_SUCCESS;
}

static bool kxtj3_is_14_bit_range(KXTJ3_G_RANGE_T g_range_mode)
{
    return g_range_mode == KXTJ3_RANGE_8G_14 || g_range_mode == KXTJ3_RANGE_16G_14;
}

upm_result_t kxtj3_get_acceleration_data_raw(const kxtj3_context dev, float *x, float *y, float *z)
{
    uint8_t buffer[DATA_BUFFER_LENGTH];
    if (kxtj3_read_registers(dev, KXTJ3_XOUT_L, buffer, DATA_BUFFER_LENGTH) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    if (dev->res_mode == HIGH_RES)
    {
        uint8_t shift_amount = 4;
        if (kxtj3_is_14_bit_range(dev->g_range_mode))
            shift_amount = 2;

        if (x)
            *x = (float)((int16_t)((buffer[1] << 8) | buffer[0]) >> shift_amount);
        if (y)
            *y = (float)((int16_t)((buffer[3] << 8) | buffer[2]) >> shift_amount);
        if (z)
            *z = (float)((int16_t)((buffer[5] << 8) | buffer[4]) >> shift_amount);
    }
    else
    {
        if (x)
            *x = (float)(int8_t)buffer[1];
        if (y)
            *y = (float)(int8_t)buffer[3];
        if (z)
            *z = (float)(int8_t)buffer[5];
    }

    return UPM_SUCCESS;
}

upm_result_t kxtj3_get_acceleration_data(const kxtj3_context dev, float *x, float *y, float *z)
{
    float x_raw, y_raw, z_raw;

    if (kxtj3_get_acceleration_data_raw(dev, &x_raw, &y_raw, &z_raw) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    if (x)
        *x = (x_raw * dev->acceleration_scale) * EARTH_GRAVITY;
    if (y)
        *y = (y_raw * dev->acceleration_scale) * EARTH_GRAVITY;
    if (z)
        *z = (z_raw * dev->acceleration_scale) * EARTH_GRAVITY;

    return UPM_SUCCESS;
}

float kxtj3_get_acceleration_sampling_period(kxtj3_context dev)
{
    return dev->odr_in_sec;
}

float kxtj3_get_wakeup_sampling_period(kxtj3_context dev)
{
    return dev->odr_in_sec_wakeup;
}

upm_result_t kxtj3_enable_data_ready_interrupt(const kxtj3_context dev)
{
    assert(dev != NULL);
    return kxtj3_set_bit_on(dev, KXTJ3_CTRL_REG1, KXTJ3_CTRL_REG1_DRDYE);
}

upm_result_t kxtj3_disable_data_ready_interrupt(const kxtj3_context dev)
{
    assert(dev != NULL);
    return kxtj3_set_bit_off(dev, KXTJ3_CTRL_REG1, KXTJ3_CTRL_REG1_DRDYE);
}

upm_result_t kxtj3_enable_wakeup_interrupt(const kxtj3_context dev)
{
    assert(dev != NULL);
    return kxtj3_set_bit_on(dev, KXTJ3_CTRL_REG1, KXTJ3_CTRL_REG1_WUFE);
}

upm_result_t kxtj3_disable_wakeup_interrupt(const kxtj3_context dev)
{
    assert(dev != NULL);
    return kxtj3_set_bit_off(dev, KXTJ3_CTRL_REG1, KXTJ3_CTRL_REG1_WUFE);
}

upm_result_t kxtj3_enable_interrupt_pin(const kxtj3_context dev, KXTJ3_INTERRUPT_POLARITY_T polarity,
                                        KXTJ3_INTERRUPT_RESPONSE_T response_type)
{
    assert(dev != NULL);
    uint8_t int_reg_value = 0;
    kxtj3_read_register(dev, KXTJ3_INT_CTRL_REG1, &int_reg_value);

    if (polarity)
        polarity = KXTJ3_INT_CTRL_REG1_IEA;
    if (response_type)
        response_type = KXTJ3_INT_CTRL_REG1_IEL;

    int_reg_value &= ~(KXTJ3_INT_CTRL_REG1_IEA | KXTJ3_INT_CTRL_REG1_IEL);
    int_reg_value |= (KXTJ3_INT_CTRL_REG1_IEN | polarity | response_type);

    return kxtj3_write_register(dev, KXTJ3_INT_CTRL_REG1, int_reg_value);
}

upm_result_t kxtj3_disable_interrupt_pin(const kxtj3_context dev)
{
    assert(dev != NULL);
    return kxtj3_set_bit_off(dev, KXTJ3_INT_CTRL_REG1, KXTJ3_INT_CTRL_REG1_IEN);
}

upm_result_t kxtj3_set_interrupt_polarity(const kxtj3_context dev, KXTJ3_INTERRUPT_POLARITY_T polarity)
{
    assert(dev != NULL);
    if (polarity == ACTIVE_HIGH)
        return kxtj3_set_bit_on(dev, KXTJ3_INT_CTRL_REG1, KXTJ3_INT_CTRL_REG1_IEA);

    return kxtj3_set_bit_off(dev, KXTJ3_INT_CTRL_REG1, KXTJ3_INT_CTRL_REG1_IEA);
}

upm_result_t kxtj3_set_interrupt_response(const kxtj3_context dev, KXTJ3_INTERRUPT_RESPONSE_T response_type)
{
    assert(dev != NULL);
    if (response_type == LATCH_UNTIL_CLEARED)
        return kxtj3_set_bit_on(dev, KXTJ3_INT_CTRL_REG1, KXTJ3_INT_CTRL_REG1_IEL);

    return kxtj3_set_bit_off(dev, KXTJ3_INT_CTRL_REG1, KXTJ3_INT_CTRL_REG1_IEL);
}

bool kxtj3_get_interrupt_status(const kxtj3_context dev)
{
    assert(dev != NULL);
    uint8_t status_reg_value = 0;
    kxtj3_read_register(dev, KXTJ3_STATUS_REG, &status_reg_value);
    if (!(status_reg_value & KXTJ3_STATUS_REG_INT))
        return false;

    return true;
}

upm_result_t kxtj3_read_interrupt_source1_reg(const kxtj3_context dev, uint8_t *reg_value)
{
    assert(dev != NULL);
    return kxtj3_read_register(dev, KXTJ3_INT_SOURCE1, reg_value);
}

KXTJ3_INTERRUPT_SOURCE_T kxtj3_get_interrupt_source(const kxtj3_context dev)
{
    assert(dev != NULL);
    if (kxtj3_get_interrupt_status(dev))
    {
        uint8_t int_source_reg;
        kxtj3_read_interrupt_source1_reg(dev, &int_source_reg);

        int_source_reg &= (KXTJ3_INT_SOURCE1_DRDY | KXTJ3_INT_SOURCE1_WUFS);
        switch (int_source_reg)
        {
        case KXTJ3_INT_SOURCE1_DRDY:
            return KXTJ3_DATA_READY_INTERRUPT;
        case KXTJ3_INT_SOURCE1_WUFS:
            return KXTJ3_WAKEUP_INTERRUPT;
        case KXTJ3_INT_SOURCE1_DRDY | KXTJ3_INT_SOURCE1_WUFS:
            return KXTJ3_DATA_READY_AND_WAKEUP_INT;
        }
    }
    return NO_INTERRUPT;
}

upm_result_t kxtj3_install_isr(const kxtj3_context dev, mraa_gpio_edge_t edge, int pin, void (*isr)(void *), void *isr_args)
{
    assert(dev != NULL);
    mraa_gpio_context isr_gpio = NULL;
    if (!(isr_gpio = mraa_gpio_init(pin)))
    {
        printf("%s: mraa_gpio_init() failed.\n", __FUNCTION__);
        return UPM_ERROR_OPERATION_FAILED;
    }

    mraa_gpio_dir(isr_gpio, MRAA_GPIO_IN);

    if (mraa_gpio_isr(isr_gpio, edge, isr, isr_args) != MRAA_SUCCESS)
    {
        mraa_gpio_close(isr_gpio);
        printf("%s: mraa_gpio_isr() failed.\n", __FUNCTION__);
        return UPM_ERROR_OPERATION_FAILED;
    }

    dev->interrupt_pin = isr_gpio;
    return UPM_SUCCESS;
}

void kxtj3_uninstall_isr(const kxtj3_context dev)
{
    assert(dev != NULL);
    mraa_gpio_isr_exit(dev->interrupt_pin);
    mraa_gpio_close(dev->interrupt_pin);
    dev->interrupt_pin = NULL;
}

upm_result_t kxtj3_clear_interrupt_information(kxtj3_context dev)
{
    assert(dev != NULL);
    uint8_t int_rel_value;
    return kxtj3_read_register(dev, KXTJ3_INT_REL, &int_rel_value);
}

upm_result_t kxtj3_enable_wakeup_single_axis_direction(kxtj3_context dev, KXTJ3_WAKEUP_SOURCE_T axis)
{
    assert(dev != NULL);
    return kxtj3_set_bit_on(dev, KXTJ3_INT_CTRL_REG2, axis);
}

upm_result_t kxtj3_disable_wakeup_single_axis_direction(kxtj3_context dev, KXTJ3_WAKEUP_SOURCE_T axis)
{
    assert(dev != NULL);
    return kxtj3_set_bit_off(dev, KXTJ3_INT_CTRL_REG2, axis);
}

kxtj3_wakeup_axes kxtj3_get_wakeup_axis_and_direction(kxtj3_context dev)
{
    assert(dev != NULL);

    uint8_t int_source2_value = 0;
    kxtj3_read_register(dev, KXTJ3_INT_SOURCE2, &int_source2_value);

    kxtj3_wakeup_axes wakeup_axis;
    wakeup_axis.X_NEGATIVE = false;
    wakeup_axis.X_POSITIVE = false;
    wakeup_axis.Y_POSITIVE = false;
    wakeup_axis.Y_NEGATIVE = false;
    wakeup_axis.Z_POSITIVE = false;
    wakeup_axis.Z_NEGATIVE = false;

    if (int_source2_value & KXTJ3_INT_SOURCE2_XPWU)
        wakeup_axis.X_POSITIVE = true;
    else if (int_source2_value & KXTJ3_INT_SOURCE2_XNWU)
        wakeup_axis.X_NEGATIVE = true;
    if (int_source2_value & KXTJ3_INT_SOURCE2_YPWU)
        wakeup_axis.Y_POSITIVE = true;
    else if (int_source2_value & KXTJ3_INT_SOURCE2_YNWU)
        wakeup_axis.Y_NEGATIVE = true;
    if (int_source2_value & KXTJ3_INT_SOURCE2_ZPWU)
        wakeup_axis.Z_POSITIVE = true;
    else if (int_source2_value & KXTJ3_INT_SOURCE2_ZNWU)
        wakeup_axis.Z_NEGATIVE = true;

    return wakeup_axis;
}

upm_result_t kxtj3_enable_wakeup_latch(kxtj3_context dev)
{
    assert(dev != NULL);
    return kxtj3_set_bit_off(dev, KXTJ3_INT_CTRL_REG2, KXTJ3_INT_CTRL_REG2_ULMODE);
}

upm_result_t kxtj3_disable_wakeup_latch(kxtj3_context dev)
{
    assert(dev != NULL);
    return kxtj3_set_bit_on(dev, KXTJ3_INT_CTRL_REG2, KXTJ3_INT_CTRL_REG2_ULMODE);
}

upm_result_t kxtj3_set_wakeup_motion_counter(kxtj3_context dev, uint8_t count)
{
    assert(dev != NULL);
    if (count == 0)
        return UPM_ERROR_OPERATION_FAILED;
    return kxtj3_write_register(dev, KXTJ3_WAKEUP_COUNTER, count);
}

static uint8_t kxtj3_get_wakeup_count_from_time_sec(kxtj3_context dev, float time_sec)
{
    return time_sec / dev->odr_in_sec_wakeup;
}

upm_result_t kxtj3_set_wakeup_motion_time(kxtj3_context dev, float desired_time)
{
    assert(dev != NULL);
    uint8_t count = kxtj3_get_wakeup_count_from_time_sec(dev, desired_time);
    return kxtj3_set_wakeup_motion_counter(dev, count);
}

upm_result_t kxtj3_get_wakeup_motion_time(kxtj3_context dev, float *out_time)
{
    assert(dev != NULL);
    uint8_t motion_count = 0;
    if (kxtj3_read_register(dev, KXTJ3_WAKEUP_COUNTER, &motion_count) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    *out_time = (float)motion_count * dev->odr_in_sec_wakeup;

    return UPM_SUCCESS;
}

upm_result_t kxtj3_set_wakeup_non_activity_counter(kxtj3_context dev, uint8_t count)
{
    assert(dev != NULL);
    if (count == 0)
        return UPM_ERROR_OPERATION_FAILED;
    return kxtj3_write_register(dev, KXTJ3_NA_COUNTER, count);
}

upm_result_t kxtj3_set_wakeup_non_activity_time(kxtj3_context dev, float desired_time)
{
    assert(dev != NULL);
    uint8_t count = kxtj3_get_wakeup_count_from_time_sec(dev, desired_time);
    return kxtj3_set_wakeup_non_activity_counter(dev, count);
}

upm_result_t kxtj3_get_wakeup_non_activity_time(kxtj3_context dev, float *out_time)
{
    assert(dev != NULL);
    uint8_t non_activity_reg_count = 0;
    if (kxtj3_read_register(dev, KXTJ3_NA_COUNTER, &non_activity_reg_count) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    *out_time = (float)non_activity_reg_count * dev->odr_in_sec_wakeup;

    return UPM_SUCCESS;
}

upm_result_t kxtj3_set_wakeup_threshold_counter(kxtj3_context dev, uint16_t count)
{
    assert(dev != NULL);
    if (count == 0)
        return UPM_ERROR_OPERATION_FAILED;
    if (kxtj3_write_register(dev, KXTJ3_WAKEUP_THRESHOLD_H, (uint8_t)(count >> 4)) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    if (kxtj3_write_register(dev, KXTJ3_WAKEUP_THRESHOLD_L, (uint8_t)(count << 4)) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    return UPM_SUCCESS;
}

static uint16_t kxtj3_get_wakeup_threshold_count_from_g(kxtj3_context dev, float g_threshold)
{
    return g_threshold * 256;
}

upm_result_t kxtj3_set_wakeup_threshold_g_value(kxtj3_context dev, float g_threshold)
{
    assert(dev != NULL);
    uint16_t count = kxtj3_get_wakeup_threshold_count_from_g(dev, g_threshold);
    return kxtj3_set_wakeup_threshold_counter(dev, count);
}

upm_result_t kxtj3_get_wakeup_threshold(kxtj3_context dev, float *out_threshold)
{
    assert(dev != NULL);
    uint8_t reg_value_h = 0, reg_value_l = 0;
    if (kxtj3_read_register(dev, KXTJ3_WAKEUP_THRESHOLD_H, &reg_value_h) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    if (kxtj3_read_register(dev, KXTJ3_WAKEUP_THRESHOLD_L, &reg_value_l) != UPM_SUCCESS)
        return UPM_ERROR_OPERATION_FAILED;

    *out_threshold = (float)((uint16_t)((reg_value_h << 8) | reg_value_l) >> 4) / 256;

    return UPM_SUCCESS;
}
