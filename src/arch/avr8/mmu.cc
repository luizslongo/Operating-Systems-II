// EPOS-- AVR8 MMU Mediator Implementation

#include <arch/avr8/mmu.h>

__BEGIN_SYS

// Class attributes
//AVR8_MMU::List AVR8_MMU::_mem_list;
AVR8_MMU::Phy_Addr AVR8_MMU::_base;
unsigned int AVR8_MMU::_top;

AVR8_MMU::Phy_Addr AVR8_MMU::alloc(unsigned int bytes) {
  db<AVR8_MMU>(TRC) << "AVR8_MMU::alloc(bytes=" << bytes << ")\n"; 

  Phy_Addr tmp = _base;
  if(!((_top - _base) >= bytes)) return 0xffff;
  _base += bytes;

  return tmp;
}

void AVR8_MMU::free(Phy_Addr addr, int n) {
  db<AVR8_MMU>(TRC) << "AVR8_MMU::free(addr=" << (void *)addr 
		    << ",n=" << n << ")\n";
}

__END_SYS
