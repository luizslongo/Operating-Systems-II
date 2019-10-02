// EPOS ARM Cortex-M I2C Mediator Declarations

// TODO: it looks like this mediator only implements Master operation

#ifndef __emote3_i2c_h
#define __emote3_i2c_h

#include <architecture/cpu.h>
#include <machine/cortex/engines/pl061.h>
#define __common_only__
#include <machine/i2c.h>
#undef __common_only__
#include "sysctrl.h"
#include "ioctrl.h"
#include "memory_map.h"

__BEGIN_SYS

class I2C_Engine: public I2C_Common
{
    // This is a hardware object.
    // Use with something like "new (Memory_Map::ADC0_BASE) eMote3_ADC"

private:
    typedef CPU::Reg32 Reg32;
    typedef GPIO_Common::Port Port;
    typedef GPIO_Common::Pin Pin;

public:
    // Registers offsets from BASE (i.e. this)
    enum {
        I2CM_SA         = 0x00,
        I2CM_CTRL       = 0x04,
        I2CM_STAT       = I2CM_CTRL,
        I2CM_DR         = 0x08,
        I2CM_TPR        = 0x0c,
        I2CM_IMR        = 0x10,
        I2CM_RIS        = 0x14,
        I2CM_MIS        = 0x18,
        I2CM_ICR        = 0x1c,
        I2CM_CR         = 0x20,
    };

    // I2C slave offsets
    enum {                           // Description
        I2CS_OAR        = 0x00,      // Own Address
        I2CS_CTRL       = 0x04,      // Control and Status
        I2CS_STAT       = I2CS_CTRL, // Control and Status
        I2CS_DR         = 0x08,      // Data
        I2CS_IMR        = 0x0c,      // Interrupt Mask
        I2CS_RIS        = 0x10,      // Raw Interrupt Status
        I2CS_MIS        = 0x14,      // Masked Interrupt Status
        I2CS_ICR        = 0x18,      // Interrupt Clear
    };

    // Unified I2C Master-Slave offsets
    enum {
        I2C_SA         = 0x00,
        I2C_OAR        = I2C_SA,
        I2C_CTRL       = 0x04,
        I2C_STAT       = I2C_CTRL,
        I2C_DR         = 0x08,
        I2C_TPR        = 0x0c,
        I2C_IMR        = 0x10,
        I2C_RIS        = 0x14,
        I2C_MIS        = 0x18,
        I2C_ICR        = 0x1c,
        I2C_CR         = 0x20,
    };

    // Useful bits in the I2CM_SA register
    enum {
        I2C_SA_RS              = 0x01,
    };

    // Useful bits in the I2CM_CTRL register
    enum {
        I2C_CTRL_ACK   = 1 << 3,
        I2C_CTRL_STOP  = 1 << 2,
        I2C_CTRL_START = 1 << 1,
        I2C_CTRL_RUN   = 1 << 0,
    };

    // Useful bits in the I2CM_STAT register
    enum {                        // Description (type)
        I2C_STAT_BUSBSY = 1 << 6, // Bus Busy (RO)
        I2C_STAT_IDLE   = 1 << 5, // I2C Idle (RO)
        I2C_STAT_ARBLST = 1 << 4, // Arbitration Lost (RO)
        I2C_STAT_DATACK = 1 << 3, // Acknowledge Data (RO)
        I2C_STAT_ADRACK = 1 << 2, // Acknowledge Address (RO)
        I2C_STAT_ERROR  = 1 << 1, // Error (RO)
        I2C_STAT_BUSY   = 1 << 0, // I2C Busy (RO)
    };

    // Useful bits in the I2CM_IMR register
    enum {                  // Description (type)
        I2C_IMR_IM = 0x01, // Interrupt Mask (RW)
    };

    // Useful bits in the I2CM_RIS register
    enum {                   // Description (type)
        I2C_RIS_BIT = 0x01, // Raw Interrupt Status (RO)
    };

    // Useful bits in the I2CM_MIS register
    enum {                   // Description (type)
        I2C_MIS_MIS = 0x01, // Masked Interrupt Status (RO)
    };

    // Useful bits in the I2CM_ICR register
    enum {                  // Description (type)
        I2C_ICR_IC = 0x01, // Interrupt Clear (WO)
    };

    // Useful bits in the I2CM_CR register
    enum {                     // Description (type)
        I2C_CR_SFE  = 1 << 5, // I2C Slave Function Enable (RW)
        I2C_CR_MFE  = 1 << 4, // I2C Master Function Enable (RW)
        I2C_CR_LPBK = 1 << 0, // I2C Loopback (RW)
    };

    // Useful bits in the I2CS_STAT register
    enum {                      // Description (type)
        I2C_STAT_FBR  = 1 << 2, // First Byte Received (RO)
        I2C_STAT_TREQ = 1 << 1, // Transmit Request (RO)
        I2C_STAT_RREQ = 1 << 0, // Receive Request (RO)
    };

    // Useful bits in the I2CS_CTRL register
    enum {                   // Description (type)
        I2C_CTRL_DA = 0x01, // Device Active (WO)
    };

    // Useful bits in the I2CS_IMR register
    enum {                         // Description (type)
        I2C_IMR_STOPIM  = 1 << 2, // Stop Condition Interrupt Mask (RO)
        I2C_IMR_STARTIM = 1 << 1, // Start Condition Interrupt Mask (RO)
        I2C_IMR_DATAIM  = 1 << 0, // Data Interrupt Mask (RW)
    };

    // Useful bits in the I2CS_RIS register
    enum {                          // Description (type)
        I2C_RIS_STOPRIS  = 1 << 2, // Stop Condition Raw Interrupt Status (RO)
        I2C_RIS_STARTRIS = 1 << 1, // Start Condition Raw Interrupt Status (RO)
        I2C_RIS_DATARIS  = 1 << 0, // Data Interrupt Status (RO)
    };

    // Useful bits in the I2CS_MIS register
    enum {                          // Description (type)
        I2C_MIS_STOPMIS  = 1 << 2, // Stop Condition Masked Interrupt Status (RO)
        I2C_MIS_STARTMIS = 1 << 1, // Start Condition Masked Interrupt Status (RO)
        I2C_MIS_DATAMIS  = 1 << 0, // Data Masked Interrupt Status (RO)
    };

    // Useful bits in the I2CS_ICR register
    enum {                          // Description (type)
        I2C_ICR_STOPIC   = 1 << 2, // Stop Condition Interrupt Clear (WO)
        I2C_ICR_STARTIC  = 1 << 1, // Start Condition Interrupt Clear (WO)
        I2C_ICR_DATAIC   = 1 << 0, // Data Interrupt Clear (WO)
    };

public:
    void config(const Role & role) {
        if(role == MASTER) {
            i2c(I2C_CR) = I2C_CR_MFE;
            i2c(I2C_TPR) = 0x3; // 400kHz, assuming a system clock of 32MHz
        } else
            i2c(I2C_CR) = I2C_CR_SFE;
    }

    bool get(char slave_address, char * data, bool stop = true) {
        // Specify the slave address and that the next operation is a read (last bit = 1)
        i2c(I2C_SA) = (slave_address << 1) | 0x01;
        return get_byte(data, I2C_CTRL_RUN | I2C_CTRL_START | (stop ? I2C_CTRL_STOP : 0));

    }

    bool put(unsigned char slave_address, char data, bool stop = true) {
        // Specify the slave address and that the next operation is a write (last bit = 0)
        i2c(I2C_SA) = (slave_address << 1);
        return send_byte(data, I2C_CTRL_RUN | I2C_CTRL_START | (stop ? I2C_CTRL_STOP : 0));
    }

    bool read(char slave_address, char * data, unsigned int size, bool stop) {
        unsigned int i;
        bool ret = true;
        // Specify the slave address and that the next operation is a read (last bit = 1)
        i2c(I2C_SA) = (slave_address << 1) | 0x01;
        for(i = 0; i < size; i++, data++) {
            if(i == 0)
                ret = get_byte(data, I2C_CTRL_START | I2C_CTRL_RUN | I2C_CTRL_ACK);
            else if(i + 1 == size)
                ret = get_byte(data, I2C_CTRL_RUN | (stop ? I2C_CTRL_STOP : 0));
            else
                ret = get_byte(data, I2C_CTRL_RUN | I2C_CTRL_ACK);
            if(!ret)
                return ret;
        }
        return ret;
    }

    bool write(unsigned char slave_address, const char * data, unsigned int size, bool stop) {
        bool ret = true;
        // Specify the slave address and that the next operation is a write (last bit = 0)
        i2c(I2C_SA) = (slave_address << 1) & 0xFE;
        for(unsigned int i = 0; i < size; i++) {
            if(i == 0) //first byte to be sent
                ret = send_byte(data[i], I2C_CTRL_RUN | I2C_CTRL_START);
            else if(i + 1 == size)
                ret = send_byte(data[i], I2C_CTRL_RUN | (stop ? I2C_CTRL_STOP : 0));
            else
                ret = send_byte(data[i], I2C_CTRL_RUN);

            if(!ret)
                return false;
        }
        return ret;
    }

    bool ready_to_get() { return ready_to_put(); }
    bool ready_to_put() { return !(i2c(I2C_STAT) & I2C_STAT_BUSY); }

private:
    bool send_byte(char data, int mode) {
        i2c(I2C_DR) = data;
        i2c(I2C_CTRL) = mode;
        while(!ready_to_put());
        return !(i2c(I2C_STAT) & I2C_STAT_ERROR);
    }

    bool get_byte(char * data, int mode) {
        i2c(I2C_CTRL) = mode;
        while(!ready_to_get());
        if(i2c(I2C_STAT) & I2C_STAT_ERROR) {
            return false;
        } else {
            *data = i2c(I2C_DR);
            return true;
        }
    }

    volatile Reg32 & i2c(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(this)[o / sizeof(Reg32)]; }
};

class I2C: private I2C_Engine
{
    friend Machine;

private:
    typedef I2C_Engine Engine;

public:
    I2C(const Role & role, const Port & port_sda, const Pin & pin_sda, const Port &  port_scl, const Pin & pin_scl)
    : _i2c((role == MASTER) ? reinterpret_cast<Engine *>(Memory_Map::I2C_MASTER_BASE) : reinterpret_cast<I2C_Engine *>(Memory_Map::I2C_SLAVE_BASE)) {
        assert((port_sda >= 'A') && (port_sda <= 'D'));
        assert(pin_sda <= 7);
        assert((port_scl >= 'A') && (port_scl <= 'D'));
        assert(pin_scl <= 7);

        PL061 * sda = new(reinterpret_cast<void *>(Memory_Map::GPIOA_BASE + port_sda * 0x1000)) PL061;
        PL061 * scl = new(reinterpret_cast<void *>(Memory_Map::GPIOA_BASE + port_scl * 0x1000)) PL061;

        scr()->clock_i2c();

        sda->select_pin_function(1 << pin_sda, PL061::FUN_ALTERNATE);
        sda->direction(1 << pin_sda, PL061::INOUT);
        scl->select_pin_function(1 << pin_scl, PL061::FUN_ALTERNATE);
        scl->direction(1 << pin_scl, PL061::INOUT);

        ioc()->enable_i2c(port_dsa, pin_sda, port_scl, pin_scl);

        _i2c->config(role);
    }

    using Engine::get;
    using Engine::put;

    using Engine::read;
    using Engine::write;

    using Engine::ready_to_get;
    using Engine::ready_to_put;

private:
    static SysCtrl * scr() { return reinterpret_cast<SysCtrl *>(Memory_Map::SCR_BASE); }
    static IOCtrl * ioc() { return reinterpret_cast<IOCtrl *>(Memory_Map::IOC_BASE); }

    static void init() {}

private:
    I2C_Engine * _i2c;
};

// Si7020 Temperature and Humidity sensor
class I2C_Sensor_Engine
{
private:
    //Addresses
    enum {
        I2C_ADDR        = 0x40, // The 7-bit base slave address is 0x40
        RH_HOLD         = 0xE5, // Measure Relative Humidity, Hold Master Mode
        RH_NOHOLD       = 0xF5, // Measure Relative Humidity, No Hold Master Mode
        TEMP_HOLD       = 0xE3, // Measure Temperature, Hold Master Mode
        TEMP_NOHOLD     = 0xF3, // Measure Temperature, No Hold Master Mode
        TEMP_PREV       = 0xE0, // Read Temperature Value from Previous RH Measurement
        RESET_SI        = 0xFE, // Reset
        WREG            = 0xE6, // Write RH/T User Register 1
        RREG            = 0xE7, // Read RH/T User Register 1
        WHCR            = 0x51, // Write Heater Control Register
        RHCR            = 0x11, // Read Heater Control Register
    };

    //Resolution
    enum {
        RH_12B_TEMP_14B = 0x0, /// < 12 bits for RH, 14 bits for Temp
        RH_8B_TEMP_12B  = 0x1, /// < 8  bits for RH, 12 bits for Temp
        RH_10B_TEMP_13B = 0x2, /// < 10 bits for RH, 13 bits for Temp
        RH_11B_TEMP_11B = 0x3, /// < 11 bits for RH, 11 bits for Temp
    };

public:
    static bool reset(I2C * i2c) { 
        bool ret = i2c->put(I2C_ADDR, RESET_SI, true);
        if(ret)
            Machine::delay(15000); // Si7020 may take up to 15ms to power up after a soft reset
        return ret;
    }

    static int relative_humidity(I2C * i2c) {
        char data[2];
        i2c->put(I2C_ADDR, RH_HOLD, false);

        i2c->get(I2C_ADDR, data, 2, true);

        int ret = (data[0] << 8 ) | data[1];

        ret = (1250 * ret) / 65536 - 60;
        ret = (ret + 5) / 10;

        // the measured value of %RH may be slightly greater than 100
        // when the actual RH level is close to or equal to 100
        if(ret < 0) ret = 0;
        else if(ret > 100) ret = 100;
        return ret;
    }

    static int celsius(I2C * i2c) {
        char data[2];
        i2c->put(I2C_ADDR, TEMP_HOLD, false);

        i2c->get(I2C_ADDR, data, 2, true);

        int ret = (data[0] << 8) | data[1];
        ret = ((17572 * ret)) / 65536 - 4685;
        ret = ret + 50 / 100;
        return ret;
    }
};

class I2C_Temperature_Sensor: private I2C_Sensor_Engine
{
public:
    I2C_Temperature_Sensor(char port_sda = 'B', unsigned int pin_sda = 1, char port_scl = 'B', unsigned int pin_scl = 0) : _i2c(I2C_Common::MASTER, port_sda, pin_sda, port_scl, pin_scl) { I2C_Sensor_Engine::reset(&_i2c); }

    int get() { while(!_i2c.ready_to_get()); return I2C_Sensor_Engine::celsius(&_i2c); }

private:
    I2C _i2c;
};

class I2C_Humidity_Sensor: private I2C_Sensor_Engine
{
public:
    I2C_Humidity_Sensor(char port_sda = 'B', unsigned int pin_sda = 1, char port_scl = 'B', unsigned int pin_scl = 0) : _i2c(I2C_Common::MASTER, port_sda, pin_sda, port_scl, pin_scl) { I2C_Sensor_Engine::reset(&_i2c); }

    int get() { while(!_i2c.ready_to_get()); return I2C_Sensor_Engine::relative_humidity(&_i2c); }

private:
    I2C _i2c;
};

__END_SYS

#endif

#endif
