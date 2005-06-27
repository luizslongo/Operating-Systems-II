// EPOS-- Task Abstraction Initialization

#include <utility/elf.h>
#include <task.h>

__BEGIN_SYS

int Task::init(System_Info * si)
{
    db<Init>(TRC) << "Task::init()\n";

    if(si->bm.app_off != -1)
	db<Init>(INF) << "Task:: additional tasks at " << (void *)si->pmm.img 
		      << "( " << si->pmm.img_size << ")\n";

    return 0;
}

__END_SYS
