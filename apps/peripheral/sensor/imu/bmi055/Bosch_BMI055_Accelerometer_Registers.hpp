/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __BOSCH_BMI055_ACCELEROMETER_REGISTERS_H__
#define __BOSCH_BMI055_ACCELEROMETER_REGISTERS_H__


#pragma once

namespace Bosch::BMI055::Accelerometer {

// TODO: move to a central header
static constexpr uint8_t Bit0 = (1 << 0);
static constexpr uint8_t Bit1 = (1 << 1);
static constexpr uint8_t Bit2 = (1 << 2);
static constexpr uint8_t Bit3 = (1 << 3);
static constexpr uint8_t Bit4 = (1 << 4);
static constexpr uint8_t Bit5 = (1 << 5);
static constexpr uint8_t Bit6 = (1 << 6);
static constexpr uint8_t Bit7 = (1 << 7);

static constexpr uint32_t SPI_SPEED = 10 * 1000 * 1000; // 10MHz SPI serial interface
static constexpr uint8_t  DIR_READ  = 0x80;

static constexpr uint8_t chip_id = 0b11111010;

enum class Register : uint8_t {
    BGW_CHIPID = 0x00,

    ACCD_TEMP = 0x08,

    INT_STATUS_1 = 0x0A,

    FIFO_STATUS = 0x0E,
    PMU_RANGE   = 0x0F,

    ACCD_HBW      = 0x13,
    BGW_SOFTRESET = 0x14,

    INT_EN_1 = 0x17,

    INT_MAP_1 = 0x1A,

    INT_OUT_CTRL = 0x20,

    FIFO_CONFIG_0 = 0x30,

    FIFO_CONFIG_1 = 0x3E,
    FIFO_DATA     = 0x3F,
};

// INT_STATUS_1
enum INT_STATUS_1_BIT : uint8_t {
    data_int      = Bit7,
    fifo_wm_int   = Bit6,
    fifo_full_int = Bit5,
};

// FIFO_STATUS
enum FIFO_STATUS_BIT : uint8_t {
    fifo_overrun       = Bit7,
    fifo_frame_counter = Bit6 | Bit5 | Bit4 | Bit3 | Bit2 | Bit1 | Bit0, // fifo_frame_counter<6:0>
};

// ACCD_HBW
enum ACCD_HBW_BIT : uint8_t {
    data_high_bw = Bit7, // 1 -> unfiltered
};

// PMU_RANGE
enum PMU_RANGE_BIT : uint8_t {
    // range<3:0>
    range_16g_set   = Bit3 | Bit2, //  ́1100b ́ -> ±16g range
    range_16g_clear = Bit1 | Bit0,

    range_8g_set   = Bit3, //  ́1000b ́ -> ±8g range
    range_8g_clear = Bit2 | Bit1 | Bit0,

    range_4g_set   = Bit2 | Bit0, //  ́0101b ́ -> ±4g range
    range_4g_clear = Bit3 | Bit1,

    range_2g_set   = Bit1 | Bit0, //  ́0011b ́ -> ±2g range
    range_2g_clear = Bit3 | Bit2,
};

// INT_EN_1
enum INT_EN_1_BIT : uint8_t {
    int_fwm_en   = Bit6,
    int_ffull_en = Bit5,
    data_en      = Bit4,
};

// INT_MAP_1
enum INT_MAP_1_BIT : uint8_t {
    int2_data  = Bit7,
    int2_fwm   = Bit6,
    int2_ffull = Bit5,

    int1_ffull = Bit2,
    int1_fwm   = Bit1,
    int1_data  = Bit0,
};

// INT_OUT_CTRL
enum INT_OUT_CTRL_BIT : uint8_t {
    int1_od  = Bit1,
    int1_lvl = Bit0,
};

// FIFO_CONFIG_1
enum FIFO_CONFIG_1_BIT : uint8_t {
    fifo_mode = Bit6,
};

namespace FIFO {
struct DATA {
    uint8_t ACCD_X_LSB;
    uint8_t ACCD_X_MSB;
    uint8_t ACCD_Y_LSB;
    uint8_t ACCD_Y_MSB;
    uint8_t ACCD_Z_LSB;
    uint8_t ACCD_Z_MSB;
};

static_assert(sizeof(DATA) == 6);

static constexpr size_t SIZE = sizeof(DATA) * 32; // up to 32 frames of accelerometer data

} // namespace FIFO
} // namespace Bosch::BMI055::Accelerometer

#endif // __BOSCH_BMI055_ACCELEROMETER_REGISTERS_H__
