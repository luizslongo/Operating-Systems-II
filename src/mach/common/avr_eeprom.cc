// EPOS-- AVR_EEPROM Mediator Common Package Implementation
// Author: Arliones

#include <system/config.h>
#include <mach/common/avr_eeprom.h>

__BEGIN_SYS

unsigned char AVR_EEPROM::read_byte(Log_Addr addr)
{
  unsigned char value;
  ASMV("push r17\n");
  ASMV("push r18\n");
  ASMV("mov  r17,%0\n" :: "r"((unsigned char)addr));
  ASMV("mov  r18,%0\n" :: "r"((unsigned char)(addr>>8)));
  ASMV("EERead:\n");
  ASMV("sbic 0x1c,0x01\n");
  ASMV("rjmp EERead\n");
  ASMV("out  0x1f,r18\n");
  ASMV("out  0x1e,r17\n");
  ASMV("sbi  0x1c,0x00\n");
  ASMV("in   %0,0x1d\n" : "=r"(value));
  ASMV("pop  r18\n");
  ASMV("pop  r17\n");
  return value;
}


void AVR_EEPROM::write_byte(Log_Addr addr, unsigned char data)
{
  ASMV("push r16\n");
  ASMV("push r17\n");
  ASMV("push r18\n");
  ASMV("mov  r16,%0\n" :: "r"(data));
  ASMV("mov  r17,%0\n" :: "r"((unsigned char)addr));
  ASMV("mov  r18,%0\n" :: "r"((unsigned char)(addr>>8)));
  ASMV("EEWrite:\n");
  ASMV("sbic 0x1c,0x01\n");
  ASMV("rjmp EEWrite\n");
  ASMV("out  0x1f,r18\n");
  ASMV("out  0x1e,r17\n");
  ASMV("out  0x1d,r16\n");
  ASMV("cli\n");
  ASMV("sbi  0x1c,0x02\n");
  ASMV("sbi  0x1c,0x01\n");
  ASMV("sei\n");
  ASMV("pop  r18\n");
  ASMV("pop  r17\n");
  ASMV("pop  r16\n");
}

__END_SYS
