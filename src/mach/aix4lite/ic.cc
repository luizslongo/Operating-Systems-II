// EPOS AIX4LITE_IC Implementation

#include <machine.h>
#include <ic.h>

__BEGIN_SYS

// Class attributes
AIX4LITE_IC::MM_Reg * AIX4LITE_IC::_regs = reinterpret_cast<AIX4LITE_IC::MM_Reg *>(AIX4LITE_IC::BASE_ADDRESS);
AIX4LITE_IC::Interrupt_Handler AIX4LITE_IC::_int_vector[AIX4LITE_IC::INTS];

void AIX4LITE_IC::int_no_handler(Interrupt_Id interrupt) {
	db<AIX4LITE_IC>(WRN) << "Intr " << interrupt << " occurred but had no handler associated !\n";
}

void AIX4LITE_IC::int_handler(Interrupt_Id i) {
	db<AIX4LITE_IC>(TRC) << "\n\n$AIX4LITE_IC::int_handler$\n\n";
	
	// Check pending and not masked interrupts
    unsigned int int_pending = _regs->pending & ~_regs->mask;

    // handle the first one set
    int int_no, int_bit;
    for (int_no=0, int_bit=1; int_no < INTS; int_no++, int_bit <<= 1){
      if (int_pending & int_bit){
	    //Ack Interrupt
	    _regs->pending = int_bit; 
        //dispatch specific handler
		void (*h)(unsigned int);
	    h = AIX4LITE_IC::int_vector(int_no);
	    h(int_no);
	    if (Traits<AIX4LITE_IC>::ATTEND_ONLY_ONE) {
			CPU::int_enable(); //Check why this is necessary !
			return;
		} 
      }
    }

    CPU::int_enable(); //Check why this is necessary !
	
}

__END_SYS
