/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file ms5611_i2c.cpp
 *
 * I2C interface for MS5611
 */

#include <drivers/device/i2c.h>

#if defined(CONFIG_I2C)

#   include "ms5611.h"

class MS5611_I2C : public device::I2C {
public:
    MS5611_I2C(uint8_t bus, ms5611::prom_u &prom_buf, int bus_frequency);
    ~MS5611_I2C() override = default;

    int read(unsigned offset, void *data, unsigned count) override;
    int ioctl(unsigned operation, unsigned &arg) override;

protected:
    int probe() override;

private:
    ms5611::prom_u &_prom;

    int _probe_address(uint8_t address);

    /**
	 * Send a reset command to the MS5611.
	 *
	 * This is required after any bus reset.
	 */
    int _reset();

    /**
	 * Send a measure command to the MS5611.
	 *
	 * @param addr		Which address to use for the measure operation.
	 */
    int _measure(unsigned addr);

    /**
	 * Read the MS5611 PROM
	 *
	 * @return		RT_EOK if the PROM reads successfully.
	 */
    int _read_prom();
};

device::Device *
MS5611_i2c_interface(ms5611::prom_u &prom_buf, uint32_t devid, uint8_t busnum, int bus_frequency) {
    return new MS5611_I2C(busnum, prom_buf, bus_frequency);
}

MS5611_I2C::MS5611_I2C(uint8_t bus, ms5611::prom_u &prom, int bus_frequency) :
    I2C(DRV_BARO_DEVTYPE_MS5611, MODULE_NAME, bus, 0, bus_frequency),
    _prom(prom) {
}

int MS5611_I2C::read(unsigned offset, void *data, unsigned count) {
    union _cvt {
        uint8_t  b[4];
        uint32_t w;
    } *cvt = (_cvt *)data;

    uint8_t buf[3];

    /* read the most recent measurement */
    uint8_t cmd = 0;
    int     ret = transfer(&cmd, 1, &buf[0], 3);

    if (ret == RT_EOK) {
        /* fetch the raw value */
        cvt->b[0] = buf[2];
        cvt->b[1] = buf[1];
        cvt->b[2] = buf[0];
        cvt->b[3] = 0;
    }

    return ret;
}

int MS5611_I2C::ioctl(unsigned operation, unsigned &arg) {
    int ret;

    switch (operation) {
    case IOCTL_RESET:
        ret = _reset();
        break;

    case IOCTL_MEASURE:
        ret = _measure(arg);
        break;

    default:
        ret = EINVAL;
    }

    return ret;
}

int MS5611_I2C::probe() {
    if ((RT_EOK == _probe_address(MS5611_ADDRESS_1)) || (RT_EOK == _probe_address(MS5611_ADDRESS_2))) {
        return RT_EOK;
    }

    _retries = 1;

    return -EIO;
}

int MS5611_I2C::_probe_address(uint8_t address) {
    /* select the address we are going to try */
    set_device_address(address);

    /* send reset command */
    if (RT_EOK != _reset()) {
        return -EIO;
    }

    /* read PROM */
    if (RT_EOK != _read_prom()) {
        return -EIO;
    }

    return RT_EOK;
}

int MS5611_I2C::_reset() {
    unsigned old_retrycount = _retries;
    uint8_t  cmd            = ADDR_RESET_CMD;
    int      result;

    /* bump the retry count */
    _retries = 3;
    result   = transfer(&cmd, 1, nullptr, 0);
    _retries = old_retrycount;

    return result;
}

int MS5611_I2C::_measure(unsigned addr) {
    uint8_t cmd = addr;
    return transfer(&cmd, 1, nullptr, 0);
}

int MS5611_I2C::_read_prom() {
    uint8_t prom_buf[2];

    union {
        uint8_t  b[2];
        uint16_t w;
    } cvt;

    /*
	 * Wait for PROM contents to be in the device (2.8 ms) in the case we are
	 * called immediately after reset.
	 */
    px4_usleep(3000);

    uint8_t last_val   = 0;
    bool    bits_stuck = true;

    /* read and convert PROM words */
    for (int i = 0; i < 8; i++) {
        uint8_t cmd = ADDR_PROM_SETUP + (i * 2);

        if (RT_EOK != transfer(&cmd, 1, &prom_buf[0], 2)) {
            break;
        }

        /* check if all bytes are zero */
        if (i == 0) {
            /* initialize to first byte read */
            last_val = prom_buf[0];
        }

        if ((prom_buf[0] != last_val) || (prom_buf[1] != last_val)) {
            bits_stuck = false;
        }

        /* assemble 16 bit value and convert from big endian (sensor) to little endian (MCU) */
        cvt.b[0]   = prom_buf[1];
        cvt.b[1]   = prom_buf[0];
        _prom.c[i] = cvt.w;
    }

    /* calculate CRC and return success/failure accordingly */
    return (ms5611::crc4(&_prom.c[0]) && !bits_stuck) ? RT_EOK : -EIO;
}

#endif // CONFIG_I2C
