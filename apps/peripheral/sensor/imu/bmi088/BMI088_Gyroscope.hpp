/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include "BMI088.hpp"

#include <lib/drivers/gyroscope/PX4Gyroscope.hpp>

#include "Bosch_BMI088_Gyroscope_Registers.hpp"

namespace Bosch::BMI088::Gyroscope {

class BMI088_Gyroscope : public BMI088 {
public:
    BMI088_Gyroscope(const I2CSPIDriverConfig &config);
    ~BMI088_Gyroscope() override;

    void RunImpl() override;
    void print_status() override;

private:
    void exit_and_cleanup() override;

    // Sensor Configuration
    static constexpr uint32_t RATE{2000}; // 2000 Hz
    static constexpr float    FIFO_SAMPLE_DT{1e6f / RATE};

    static constexpr int32_t FIFO_MAX_SAMPLES{math::min(FIFO::SIZE / sizeof(FIFO::DATA), sizeof(sensor_gyro_fifo_s::x) / sizeof(sensor_gyro_fifo_s::x[0]))};

    // Transfer data
    struct FIFOTransferBuffer {
        uint8_t    cmd{static_cast<uint8_t>(Register::FIFO_DATA) | DIR_READ};
        FIFO::DATA f[FIFO_MAX_SAMPLES]{};
    };

    // ensure no struct padding
    static_assert(sizeof(FIFOTransferBuffer) == (1 + FIFO_MAX_SAMPLES * sizeof(FIFO::DATA)));

    struct register_config_t {
        Register reg;
        uint8_t  set_bits{0};
        uint8_t  clear_bits{0};
    };

    int probe() override;

    bool Configure();
    void ConfigureGyro();
    void ConfigureSampleRate(int sample_rate = 0);
    void ConfigureFIFOWatermark(uint8_t samples);

    static int DataReadyInterruptCallback(int irq, void *context, void *arg);
    void       DataReady();
    bool       DataReadyInterruptConfigure();
    bool       DataReadyInterruptDisable();

    bool RegisterCheck(const register_config_t &reg_cfg);

    uint8_t RegisterRead(Register reg);
    void    RegisterWrite(Register reg, uint8_t value);
    void    RegisterSetAndClearBits(Register reg, uint8_t setbits, uint8_t clearbits);

    bool FIFORead(const hrt_abstime &timestamp_sample, uint8_t samples);
    void FIFOReset();

    PX4Gyroscope _px4_gyro;

    perf_counter_t _bad_register_perf{perf_alloc(PC_COUNT, MODULE_NAME "_gyro: bad register")};
    perf_counter_t _bad_transfer_perf{perf_alloc(PC_COUNT, MODULE_NAME "_gyro: bad transfer")};
    perf_counter_t _fifo_empty_perf{perf_alloc(PC_COUNT, MODULE_NAME "_gyro: FIFO empty")};
    perf_counter_t _fifo_overflow_perf{perf_alloc(PC_COUNT, MODULE_NAME "_gyro: FIFO overflow")};
    perf_counter_t _fifo_reset_perf{perf_alloc(PC_COUNT, MODULE_NAME "_gyro: FIFO reset")};
    perf_counter_t _drdy_missed_perf{nullptr};

    uint8_t _fifo_samples{static_cast<uint8_t>(_fifo_empty_interval_us / (1000000 / RATE))};

    uint8_t                  _checked_register{0};
    static constexpr uint8_t size_register_cfg{8};
    register_config_t        _register_cfg[size_register_cfg]{
        // Register                         | Set bits, Clear bits
        {Register::GYRO_RANGE, GYRO_RANGE_BIT::gyro_range_2000_dps, 0},
        {Register::GYRO_BANDWIDTH, 0, GYRO_BANDWIDTH_BIT::gyro_bw_532_Hz},
        {Register::GYRO_INT_CTRL, GYRO_INT_CTRL_BIT::fifo_en, 0},
        {Register::INT3_INT4_IO_CONF, 0, INT3_INT4_IO_CONF_BIT::Int3_od | INT3_INT4_IO_CONF_BIT::Int3_lvl},
        {Register::INT3_INT4_IO_MAP, INT3_INT4_IO_MAP_BIT::Int3_fifo, 0},
        {Register::FIFO_WM_ENABLE, FIFO_WM_ENABLE_BIT::fifo_wm_enable, 0},
        {Register::FIFO_CONFIG_0, 0, 0}, // fifo_water_mark_level_trigger_retain<6:0>
        {Register::FIFO_CONFIG_1, FIFO_CONFIG_1_BIT::FIFO_MODE, 0},
    };
};

} // namespace Bosch::BMI088::Gyroscope
