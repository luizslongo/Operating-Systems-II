// EPOS ARM PrimeCell PL011 UART Mediator Declarations

#ifndef __pl011_h
#define __pl011_h

#include <architecture/cpu.h>
#define __common_only__
#include <machine/uart.h>
#undef __common_only__

__BEGIN_SYS

class BSC_UART: public UART_Common
{
    // This is a hardware object.
    // Use with something like "new (Memory_Map::UARTx_BASE) PL011".

private:
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg32 Reg32;

    static const unsigned int CLOCK = Traits<UART>::CLOCK / 16;

public:
    // Registers offsets from BASE (i.e. this)
    enum {                              // Description                  Type    Value after reset
        GPFSEL1         = 0x00004,
        GPSET0          = 0x0001c,
        GPCLR0          = 0x00028,
        GPPUD           = 0x00094,
        GPPUDCLK0       = 0x00098,
        AUX_ENABLES     = 0x15004,
        AUX_MU_IO_REG   = 0x15040,
        AUX_MU_IER_REG  = 0x15044,
        AUX_MU_IIR_REG  = 0x15048,
        AUX_MU_LCR_REG  = 0x1504c,
        AUX_MU_MCR_REG  = 0x15050,
        AUX_MU_LSR_REG  = 0x15054,
        AUX_MU_MSR_REG  = 0x15058,
        AUX_MU_SCRATCH  = 0x1505c,
        AUX_MU_CNTL_REG = 0x15060,
        AUX_MU_STAT_REG = 0x15064,
        AUX_MU_BAUD_REG = 0x15068
        //PCellID3        = 0xffc         // PrimeCell Identification 3   ro      0x000000b1
    };

    // Useful Bits in the Flag Register
    enum {                              // Description                  Type    Value after reset
        CTS             = 1 <<  0,      // Clear to Send                r/w     0
        RXFE            = 1 <<  0,      // Receive FIFO Empty           r/w     1
        TXFF            = 1 <<  5,      // Transmit FIFO Full           r/w     0
        // RXFF            = 1 <<  6,      // Receive FIFO Full            r/w     0
        // TXFE            = 1 <<  7,      // Transmit FIFO Empty          r/w     1
    };

    // Useful Bits in the Control Register
    enum {                              // Description                  Type    Value after reset
        UEN             = 1 <<  0,      // Enable                       r/w     0
        // LBE             = 1 <<  7,      // Loop Back Enable             r/w     0
        TXE             = 1 <<  0,      // Transmit Enable              r/w     1
        RXE             = 1 <<  1       // Receive Enable               r/w     1
    };
public:
    void config(unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits) {
        unsigned int ra;

        /* initialize UART */
        uart(AUX_ENABLES) = UEN;
        uart(AUX_MU_IER_REG) = 0;
        uart(AUX_MU_CNTL_REG) = 0;
        uart(AUX_MU_LCR_REG) = 3;           // 8 bits
        uart(AUX_MU_MCR_REG) = 0;


        uart(AUX_MU_IER_REG) = 0x5;

        uart(AUX_MU_IIR_REG) = 0xC6;        //disable interrupts
        uart(AUX_MU_BAUD_REG) = 270;        //sets baudrate to 115200
        
        /* map UART1 to GPIO pins */
        ra = uart(GPFSEL1);
        ra &= ~(7<<12);                     //gpio14
        ra |= 2<<12;                        //alt5
        ra &=~ (7<<15);                       //gpio15
        ra |= 2<<15;                          //alt5
        
        uart(GPFSEL1) = ra;
        uart(GPPUD) = 0;
        for(ra=0;ra<150;ra++) asm volatile ("nop"); //150 cycles to synchronize
        uart(GPPUDCLK0) = (1<<14)|(1<<15);
        for(ra=0;ra<150;ra++) asm volatile ("nop"); //150 cycles to synchronize
        uart(GPPUDCLK0) = 0;                // flush GPIO setup
        uart(AUX_MU_CNTL_REG) = TXE | RXE;          // enable Tx, Rx
    }

    void config(unsigned int * baud_rate, unsigned int * data_bits, unsigned int * parity, unsigned int * stop_bits) {
        // Reg32 lcrh = uart(LCRH);
        // *data_bits = 5 + (lcrh & WLEN8);
        // *parity = (lcrh & PEN) ? (1 + (lcrh & EPS)) : 0;
        // *baud_rate = (CLOCK * 300) / (uart(FBRD) * 1000 + uart(IBRD) * 300);
        // *stop_bits = (uart(LCRH) & STP2) ? 2 : 1;
    }

    Reg8 rxd() { return uart(DR); }
    void txd(Reg8 c) { uart(DR) = c; }


    bool rxd_ok() { return !(uart(AUX_MU_LSR_REG) & RXFE); }
    bool txd_ok() { return !(uart(AUX_MU_LSR_REG) & TXFF); }

    // bool rxd_full() { return (uart(FR) & RXFF); }
    // bool txd_empty() { return (uart(FR) & TXFE) && !(uart(FR) & BUSY); }

    // bool busy() { return (uart(FR) & BUSY); }

    void enable() { uart(AUX_ENABLES) = UEN;}
    void disable() { uart(AUX_ENABLES) &= ~UEN; }

    void int_enable(bool receive = true, bool transmit = true, bool line = true, bool modem = true) {
        // uart(UIM) &= ~((receive ? UIMRX : 0) | (transmit ? UIMTX : 0));
    }
    void int_disable(bool receive = true, bool transmit = true, bool line = true, bool modem = true) {
        // uart(UCR) |= (receive ? UIMRX : 0) | (transmit ? UIMTX : 0);
    }

    void reset() {
        unsigned int b, db, p, sb;
        config(&b, &db, &p, &sb);
        config(b, db, p, sb);
    }

    void loopback(bool flag) {
        // if(flag)
        //     uart(UCR) |= int(LBE);
        // else
        //     uart(UCR) &= ~LBE;
    }

private:
    volatile Reg32 & uart(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(this)[o / sizeof(Reg32)]; }
};

__END_SYS

#endif
